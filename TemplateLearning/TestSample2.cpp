#include "includes.h"
#include <omp.h>
#include <numeric>

#include "distance.h"
#include "unit.h"
#include "vectorArithmeticOperatorOverloading.h"
#include "vectorArithmeticExpressionTemplates.h"
#include "spinlockAcquireRelease.h"

using namespace Distance::Unit;

static const int NUM = 100000;

#define _USE_MATH_DEFINES

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#pragma region Variable template
template<class T>
constexpr T pi = T(3.1415926535897932385L);  // variable template

template<class T>
T circular_area(T r) // function template
{
	return pi<T>* r* r; // pi<T> is a variable template instantiation
}

// struct matrix_constants
// {
// 	template<class T>
// 	using pauli = hermitian_matrix<T, 2>; // alias template
// 
// 	template<class T> // static data member template
// 	static constexpr pauli<T> sigmaX = { { 0, 1 }, { 1, 0 } };
// 
// 	template<class T>
// 	static constexpr pauli<T> sigmaY = { { 0, -1i }, { 1i, 0 } };
// 
// 	template<class T>
// 	static constexpr pauli<T> sigmaZ = { { 1, 0 }, { 0, -1 } };
// };

struct limits {
	template<typename T>
	static const T min; // declaration of a static data member template
};
template<typename T>
const T limits::min = { }; // definition of a static data member template

template<class T>
class X {
	static T s; // declaration of a non-template static data member of a class template
};
template<class T>
T X<T>::s = 0; // definition of a non-template data member of a class template

#pragma endregion Variable template

#pragma region ENUM
enum OldEnum {
	one = 1,
	ten = 10,
	hundred = 100,
	thousand = 1000
};

enum struct NewEnum {
	one = 1,
	ten = 10,
	hundred = 100,
	thousand = 1000
};

enum struct Colour0 : bool {
	red,     // 0
	blue     // 1
};

enum Colour1 {
	red = -5,
	blue,      // -4
	green      // -3
};

enum struct Colour2 : char {
	red = 100,
	blue, // 101
	green // 102
};

enum class Colour3 : long long int {
	//red= 	std::numeric_limits<long long int>::min();
	red = LLONG_MIN,
	blue,    // std::numeric_limits<long long int>::min() + 1
	green    // std::numeric_limits<long long int>::min() + 2
};

#pragma endregion ENUM

#pragma region parallel_sum
template <typename RandomIt>
int parallel_sum(RandomIt beg, RandomIt end)
{
	auto len = end - beg;
	if (len < 1000)
		return std::accumulate(beg, end, 0);

	RandomIt mid = beg + len / 2;
	auto handle = std::async(std::launch::async, parallel_sum<RandomIt>, mid, end);
	int sum = parallel_sum(beg, mid);
	return sum + handle.get();
}
#pragma endregion

#pragma region packaged_task
// unique function to avoid disambiguating the std::pow overload set
int f(int x, int y) { return std::pow(x, y); }

void task_lambda()
{
	std::packaged_task<int(int, int)> task([](int a, int b) {
		return std::pow(a, b);
	});
	std::future<int> result = task.get_future();

	task(2, 9);

	std::cout << "task_lambda:\t" << result.get() << '\n';
}

void task_bind()
{
	std::packaged_task<int()> task(std::bind(f, 2, 11));
	std::future<int> result = task.get_future();

	task();

	std::cout << "task_bind:\t" << result.get() << '\n';
}

void task_thread()
{
	std::packaged_task<int(int, int)> task(f);
	std::future<int> result = task.get_future();

	std::thread task_td(std::move(task), 2, 10);
	task_td.join();

	std::cout << "task_thread:\t" << result.get() << '\n';
}
#pragma endregion

struct node {
	int left;
	int right;
};

template <typename Func>
int traverse(node& n, Func&& f) {
	int left = 0, right = 0;
	define_task_block(
		[&](task_block & tb) {
		if (n.left) tb.run([&] { left = traverse(*n.left, f); });
		if (n.right) tb.run([&] { right = traverse(*n.right, f); });
	}
	);
	return f(n) + left + right;
}

#pragma region Two Phases
void func(void*) { std::puts("The call resolves to void*"); }
template<typename T> void g(T x)
{
	func(0);
}
void func(int) { std::puts("The call resolves to int"); }


void funcM(long) { std::puts("func(long)"); }

template <typename T> void meow(T t) {
	funcM(t);
}

void funcM(int) { std::puts("func(int)"); }

namespace Kitty {
	struct Peppermint {};
	void funcM(Peppermint) { std::puts("Kitty::func(Kitty::Peppermint)"); }
}

template<typename T>
typename T::TYPE funcS(typename T::TYPE*)
{
	typename T::TYPE i;
}

#pragma endregion Two Phases

//TODO What is this?
//[[transaction_unsafe]] int transactionUnsafeFunction();
#pragma region C++_is_Lazy:CRTP

#pragma region static_polymorfism
template <typename Derived>
class BaseM {
public:
	void interfaces() {
		static_cast<Derived*>(this)->implementation();
	}
	void implementation() {
		std::cout << "Implementation Base" << std::endl;
	}
};

struct Derived1 : BaseM<Derived1> {
	void implementation() {
		std::cout << "Implementation Derived1" << std::endl;
	}
};

struct Derived2 : BaseM<Derived2> {
	void implementation() {
		std::cout << "Implementation Derived2" << std::endl;
	}
};

struct Derived3 : BaseM<Derived3> {};

template <typename T>
void execute(T& base) {
 	base.interfaces();
}

#pragma endregion static_polymorfism

#pragma region Expression_templates

template <typename E>
class VecExpression {
public:
	double operator[](size_t i) const { return static_cast<E const&>(*this)[i]; }
	size_t size()               const { return static_cast<E const&>(*this).size(); }
};

class Vec : public VecExpression<Vec> {
	std::vector<double> elems;

public:
	double operator[](size_t i) const { return elems[i]; }
	double& operator[](size_t i) { return elems[i]; }
	size_t size() const { return elems.size(); }

	Vec(size_t n) : elems(n) {}

	// construct vector using initializer list 
	Vec(std::initializer_list<double>init) {
		for (auto i : init)
			elems.push_back(i);
	}

	// A Vec can be constructed from any VecExpression, forcing its evaluation.
	template <typename E>
	Vec(VecExpression<E> const& vec) : elems(vec.size()) {
		for (size_t i = 0; i != vec.size(); ++i) {
			elems[i] = vec[i];
		}
	}
};

template <typename E1, typename E2>
class VecSum : public VecExpression<VecSum<E1, E2> > {
	E1 const& _u;
	E2 const& _v;

public:
	VecSum(E1 const& u, E2 const& v) : _u(u), _v(v) {
		//static_assert(u.size() == v.size());
	}

	double operator[](size_t i) const { return _u[i] + _v[i]; }
	size_t size()               const { return _v.size(); }
};

template <typename E1, typename E2>
VecSum<E1, E2> operator+(E1 const& u, E2 const& v) {
	return VecSum<E1, E2>(u, v);
}
#pragma endregion Expression_templates

#pragma region Mixins_with_CRTP
template<class Derived>
class Equality {};

template <class Derived>
bool operator == (Equality<Derived> const& op1, Equality<Derived> const& op2) {
	Derived const& d1 = static_cast<Derived const&>(op1);
	Derived const& d2 = static_cast<Derived const&>(op2);
	return !(d1 < d2) && !(d2 < d1);
}

template <class Derived>
bool operator != (Equality<Derived> const& op1, Equality<Derived> const& op2) {
	Derived const& d1 = static_cast<Derived const&>(op1);
	Derived const& d2 = static_cast<Derived const&>(op2);
	return !(op1 == op2);
}

struct Apple :public Equality<Apple> {
	Apple(int s) : size{ s } {};
	int size;
};

bool operator < (Apple const& a1, Apple const& a2) {
	return a1.size < a2.size;
}

struct Man :public Equality<Man> {
	Man(std::string n)
		: name{ n }
	{}
	std::string name;
};

bool operator < (Man const& m1, Man const& m2) {
	return m1.name < m2.name;
}
#pragma endregion Mixins_with_CRTP

#pragma endregion C++_is_Lazy:CRTP

#pragma region Optional
std::optional<int> getFirst(const std::vector<int>& vec) {
	if (!vec.empty()) return std::optional<int>(vec[0]);
	else return std::optional<int>();
}
#pragma endregion Optional

#pragma region Future
template <typename RandomIt>
int par_sum(RandomIt beg, RandomIt end)
{
	auto len = end - beg;
	if (len < 1000)
		return std::accumulate(beg, end, 0);

	RandomIt mid = beg + len / 2;
	auto handle = std::async(std::launch::async,
		par_sum<RandomIt>, mid, end);
	int sum = par_sum(beg, mid);
	return sum + handle.get();
}
#pragma endregion Future

#pragma region AtomicSharedPtr
//In C++ 20
/*
template<typename T> class concurrent_stack {
	struct Node { T t; shared_ptr<Node> next; };
	std::atomic_shared_ptr<Node> head;
	// in C++11: remove “atomic_” and remember to use the special
	// functions every time you touch the variable
	concurrent_stack(concurrent_stack&) = delete;
	void operator=(concurrent_stack&) = delete;

public:
	concurrent_stack() = default;
	~concurrent_stack() = default;
	class reference {
		shared_ptr<Node> p;
	public:
		reference(shared_ptr<Node> p_) : p{ p_ } { }
		T& operator* () { return p->t; }
		T* operator->() { return &p->t; }
	};

	auto find(T t) const {
		auto p = head.load();  // in C++11: atomic_load(&head)
		while (p && p->t != t)
			p = p->next;
		return reference(move(p));
	}
	auto front() const {
		return reference(head); // in C++11: atomic_load(&head)
	}
	void push_front(T t) {
		auto p = make_shared<Node>();
		p->t = t;
		p->next = head;         // in C++11: atomic_load(&head)
		while (!head.compare_exchange_weak(p->next, p)) {}
		// in C++11: atomic_compare_exchange_weak(&head, &p->next, p);
	}
	void pop_front() {
		auto p = head.load();
		while (p && !head.compare_exchange_weak(p, p->next)) {}
		// in C++11: atomic_compare_exchange_weak(&head, &p, p->next);
	}
};

*/

#pragma endregion AtomicSharedPtr

#pragma region Async
std::string helloFunction(const std::string& s) {
	return "Hello C++11 from " + s + ".";
}

class HelloFunctionObject {
public:
	std::string operator()(const std::string& s) const {
		return "Hello C++11 from " + s + ".";
	}
};


long long getDotProduct(std::vector<int>& v, std::vector<int>& w) {

	auto future1 = std::async([&] {return std::inner_product(&v[0], &v[v.size() / 4], &w[0], 0L);});
 	auto future2 = std::async([&] {return std::inner_product(&v[v.size() / 4], &v[v.size() / 2], &w[v.size() / 4], 0L);});
 	auto future3 = std::async([&] {return std::inner_product(&v[v.size() / 2], &v[v.size() * 3 / 4], &w[v.size() / 2], 0L);});
 	//auto future4 = std::async([&] {return std::inner_product(&v[v.size() * 3 / 4], &v[v.size()], &w[v.size() * 3 / 4], 0L);});
 	auto f1 = future1.get();
 	auto f2 = future2.get();
 	auto f3 = future3.get();
	//auto qwe = future4.get();
	return f1 + f2 + f3;
}

#pragma endregion Async


#pragma region Acquire_Release_Fences_Atomic
std::atomic<std::string*> ptrs;
int datas;
std::atomic<int> atoData;

void producerAtomic() {
	std::string* p = new std::string("C++11");
	datas = 2011;
	atoData.store(2014, std::memory_order_relaxed);
	ptrs.store(p, std::memory_order_release);
}

void consumerAtomic() {
	std::string* p2;
	while (!(p2 = ptrs.load(std::memory_order_acquire)));
	std::cout << "*p2 Atomic: " << *p2 << std::endl;
	std::cout << "data Atomic: " << datas << std::endl;
	std::cout << "atoData Atomic: " << atoData.load(std::memory_order_relaxed) << std::endl;
}
#pragma endregion Acquire_Release_Fences_Atomic

#pragma region Acquire_Release_Fences_Barriers
std::atomic<std::string*> ptrM;
int dataM;
std::atomic<int> atoDataM;

void producerBarriers() {
	std::string* p = new std::string("C++11");
	dataM = 2011;
	atoDataM.store(2014, std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_release);
	ptrM.store(p, std::memory_order_relaxed);
}

void consumerBarriers() {
	std::string* p2;
	while (!(p2 = ptrM.load(std::memory_order_relaxed)));
	std::atomic_thread_fence(std::memory_order_acquire);
	std::cout << "*p2 Barriers: " << *p2 << std::endl;
	std::cout << "data Barriers: " << dataM << std::endl;
	std::cout << "atoData Barriers: " << atoData.load(std::memory_order_relaxed) << std::endl;
}

#pragma endregion Acquire_Release_Fences_Barriers


std::atomic<int> cnt = { 0 };

void fCnt()
{
	for (int n = 0; n < 1000; ++n) {
		cnt.fetch_add(1, std::memory_order_relaxed);
	}
}

int main()
{
	cout << "Circular area :" << circular_area<long double>(50) << endl; 
#pragma region Literals

#pragma region User Defined Literals
	cout << endl;
	cout << "1.0_km: " << 1.0_km << endl;
	cout << "1.0_m: " << 1.0_m << endl;
	cout << "1.0_dm: " << 1.0_dm << endl;
	cout << "1.0_cm: " << 1.0_cm << endl;
	cout << "0.001 * 1.0_km: " << 0.001 * 1.0_km << endl;
	cout << "10 * 1_dm: " << 10 * 1.0_dm << endl;
	cout << "100 * 1.0cm: " << 100 * 1.0_cm << endl;
	cout << "1_km / 1000: " << 1.0_km / 1000 << endl;
	cout << "1.0_km + 2.0_dm +  3.0_dm + 4.0_cm: " << 1.0_km + 2.0_dm + 3.0_dm + 4.0_cm << endl;
	auto work = 63.0_km;
	auto workPerDay = 2 * work;
	auto abbrevationToWork = 5400.0_m;
	auto workout = 2 * 1600.0_m;
	auto shopping = 2 * 1200.0_m;
	auto distPerWeek1 = 4 * workPerDay - 3 * abbrevationToWork + workout + shopping;
	auto distPerWeek2 = 4 * workPerDay - 3 * abbrevationToWork + 2 * workout;
	auto distPerWeek3 = 4 * workout + 2 * shopping;
	auto distPerWeek4 = 5 * workout + shopping;
	cout << "distPerWeek1: " << distPerWeek1 << endl;
	auto averageDistance = getAverageDistance({ distPerWeek1,distPerWeek2,distPerWeek3,distPerWeek4 });
	cout << "averageDistance: " << averageDistance << endl;
#pragma endregion User Defined Literals

#pragma region Chrono litral
	cout << endl;
	auto schoolHour = 45min;
	auto shortBreak = 300s;
	auto longBreak = 0.25h;
	auto schoolWay = 15min;
	auto homework = 2h;
	auto schoolDayInSeconds = 2 * schoolWay + 6 * schoolHour + 4 * shortBreak + longBreak + homework;
	cout << "School day in seconds: " << schoolDayInSeconds.count() << endl;
	cout << "School day in minutes: " << schoolDayInSeconds.count() / 60 << endl;
	cout << "School day in hours: " << schoolDayInSeconds.count() / 3600 << endl;
	cout << endl;
#pragma endregion Chrono litral

#pragma endregion Literals

#pragma region ENUM
	std::cout << std::endl;
	std::cout << "ENUM C++11= " << 2 * thousand + 0 * hundred + 1 * ten + 1 * one << std::endl;
	std::cout << "ENUM C++11= " << 2 * static_cast<int>(NewEnum::thousand) +
		0 * static_cast<int>(NewEnum::hundred) +
		1 * static_cast<int>(NewEnum::ten) +
		1 * static_cast<int>(NewEnum::one) << std::endl;

	std::cout << "ENUM sizeof(Colour0)= " << sizeof(Colour0) << std::endl;
	std::cout << "ENUM sizeof(Colour1)= " << sizeof(Colour1) << std::endl;
	std::cout << "ENUM sizeof(Colour2)= " << sizeof(Colour2) << std::endl;
	std::cout << "ENUM sizeof(Colour3)= " << sizeof(Colour3) << std::endl;

	std::cout << "ENUM Colour0::red: " << static_cast<bool>(Colour0::red) << std::endl;
	std::cout << "ENUM red: " << red << std::endl;
	std::cout << "ENUM Colour2::red: " << static_cast<char>(Colour2::red) << std::endl;
	std::cout << "ENUM Colour3::red: " << static_cast<long long int>(Colour3::red) << std::endl;

#pragma endregion ENUM

#pragma region TypeInfo
	cout << endl;
	int a = 0;
	int* b = 0;
	auto c = 0;
	cout << "C Type: "<<typeid(c).name() << endl;
	auto res = a + b + c;
	cout << "res value: " << res << endl;
	cout << "res Type Name: " << typeid(res).name() << endl<< endl;
#pragma endregion TypeInfo

#pragma region Concurrency::parallel_for
	cout << endl;
	int width = 50;
	cout << "Concurrency::parallel_for: ";
	Concurrency::parallel_for(0, width, [=](int x)
	{
		cout << x << ",";
	});
	cout << endl;
#pragma endregion Concurrency::parallel_for

#pragma region OpenMP
	cout << endl;
#pragma omp parallel
	printf("Hello, world.\n");

	auto startPar = chrono::steady_clock::now().time_since_epoch().count();
	int sab[100000] = { 0, };
#pragma omp parallel for
	for (int i = 0; i < 100000; i++)
		sab[i] = 2 * i;
	auto endPar = chrono::steady_clock::now().time_since_epoch().count();
	cout << "Duration parallel:" << (endPar - startPar) << endl;

	auto startNor = chrono::steady_clock::now().time_since_epoch().count();
	int saz[100000] = { 0, };
	for (int i = 0; i < 100000; i++)
		saz[i] = 2 * i;
	auto endNor = chrono::steady_clock::now().time_since_epoch().count();
	cout << "Duration normal:" << (endNor - startNor) << endl<< endl;

	int nthreads, tid;
#pragma omp parallel private(nthreads, tid)
	/* Fork a team of threads giving them their own copies of variables */
	{
		/* Obtain thread number */
		tid = omp_get_thread_num();
		printf("Hello World from thread = %d\n", tid);

		/* Only master thread does this */
		if (tid == 0)
		{
			nthreads = omp_get_num_threads();
			printf("Number of threads = %d\n", nthreads);
		}

	}  /* All threads join master thread and disband */



#pragma endregion

#pragma region parallel_sum
	cout << endl;
	auto startNr = std::chrono::steady_clock::now().time_since_epoch().count();
	vector<int> v(100000, 1);
	cout << "The sum is " << parallel_sum(v.begin(), v.end()) << '\n';
	auto endNr = std::chrono::steady_clock::now().time_since_epoch().count();
	std::cout << "Duration Vector Parallel Sum Normal:" << (endNr - startNr) << std::endl << std::endl;

	auto startPr = std::chrono::steady_clock::now().time_since_epoch().count();
 	concurrent_vector<int> vc(100000, 1);
 	cout << "The sum Count is " << parallel_sum(vc.begin(), vc.end()) << '\n';
	auto endPr = std::chrono::steady_clock::now().time_since_epoch().count();
	std::cout << "Duration Concurrent Vector Parallel Sum:" << (endPr - startPr) << std::endl << std::endl;

#pragma endregion
	   	
#pragma region Package_task
	cout << endl;
	task_lambda();
	task_bind();
	task_thread();
#pragma endregion

#pragma region Two Phases
	cout << endl;
	g(3.14);
	cout << endl;
	meow(1729);
	Kitty::Peppermint pepper;
	meow(pepper);
#pragma endregion Two Phases
	
#pragma region static_polymorfism
	std::cout << std::endl;

	Derived1 d1;
	execute(d1);

	Derived2 d2;
	execute(d2);

	Derived3 d3;
	execute(d3);

	std::cout << std::endl;
#pragma endregion static_polymorfism

#pragma region Mixins_with_CRTP
	std::cout << std::boolalpha << std::endl;

	Apple apple1{ 5 };
	Apple apple2{ 10 };
	std::cout << "apple1 == apple2: " << (apple1 == apple2) << std::endl;

	Man man1{ "grimm" };
	Man man2{ "jaud" };
	std::cout << "man1 != man2: " << (man1 != man2) << std::endl;

	std::cout << std::endl;
#pragma endregion Mixins_with_CRTP

#pragma region Expression_templates
	Vec v0 = { 23.4,12.5,144.56,90.56 };
	Vec v1 = { 67.12,34.8,90.34,89.30 };
	Vec v2 = { 34.90,111.9,45.12,90.5 };

	// Following assignment will call the ctor of Vec which accept type of 
	// `VecExpression<E> const&`. Then expand the loop body to 
	// a.elems[i] + b.elems[i] + c.elems[i]
	Vec sum_of_vec_type = v0 + v1 + v2;

	std::cout << "sum_of_vec_type -> ";
	for (int i = 0; i < sum_of_vec_type.size(); ++i)
		std::cout << sum_of_vec_type[i] << " ";
	cout << std::endl;
#pragma endregion Expression_templates

#pragma region VectorArithmeticOperatorOverloading
	cout << endl;
 	MyVectorA<double> xA(10, 5.4);
 	MyVectorA<double> yA(10, 10.3);
 	MyVectorA<double> resultA(10);
 	resultA = (xA + xA) + (yA * yA);
	std::cout<<"Vector Arithmetic Operator Overloading -> " << resultA << std::endl<<endl;
#pragma endregion VectorArithmeticOperatorOverloading

#pragma region vectorArithmeticExpressionTemplates
	cout << endl;
	MyVector<double> x(10, 5.4);
	MyVector<double> y(10, 10.3);
	MyVector<double> result(10);
	result = (x + x) + (y * y);
	std::cout<< "Vector Arithmetic Expression Templates -> " << result << std::endl;
#pragma endregion vectorArithmeticExpressionTemplates

#pragma region Optional
	std::vector<int> myVec{ 1, 2, 3 };
	std::vector<int> myEmptyVec;

	auto myInt = getFirst(myVec);

	if (myInt) {
		std::cout << "*myInt: " << *myInt << std::endl;
		std::cout << "myInt.value(): " << myInt.value() << std::endl;
		std::cout << "myInt.value_or(2017):" << myInt.value_or(2017) << std::endl;
	}

	std::cout << std::endl;

	auto myEmptyInt = getFirst(myEmptyVec);

	if (!myEmptyInt) {
		std::cout << "myEmptyInt.value_or(2017):" << myEmptyInt.value_or(2017) << std::endl;
	}
#pragma endregion Optional

#pragma region Future
	std::vector<int> vsp(10000, 1);
	std::cout << "The sum is " << par_sum(vsp.begin(), vsp.end()) << '\n';
#pragma endregion Future
	
#pragma region Atomic
	std::shared_ptr<int> ptr = std::make_shared<int>(2011);

	for (auto i = 0;i < 10;i++) {
		std::thread([&ptr] {
			auto localPtr = std::make_shared<int>(2014);
			std::atomic_store(&ptr, localPtr);
		}).detach();
	}
#pragma endregion Atomic


#pragma region Async
#pragma region Async1
	std::cout << std::endl;

	// future with function
	auto futureFunction = std::async(helloFunction, "function");

	// future with function object
	HelloFunctionObject helloFunctionObject;
	auto futureFunctionObject = std::async(helloFunctionObject, "function object");

	// future with lambda function
	auto futureLambda = std::async([](const std::string & s) {return "Hello C++11 from " + s + ".";}, "lambda function");

	std::cout << futureFunction.get() << "\n"<< futureFunctionObject.get() << "\n"<< futureLambda.get() << std::endl;

	std::cout << std::endl<< std::endl;
#pragma endregion Async1
	
#pragma region Async2
	auto begin = std::chrono::system_clock::now();

	auto asyncLazy = std::async(std::launch::deferred, [] { return  std::chrono::system_clock::now();});

	auto asyncEager = std::async(std::launch::async, [] { return  std::chrono::system_clock::now();});

	std::this_thread::sleep_for(std::chrono::seconds(1));

	auto lazyStart = asyncLazy.get() - begin;
	auto eagerStart = asyncEager.get() - begin;

	auto lazyDuration = std::chrono::duration<double>(lazyStart).count();
	auto eagerDuration = std::chrono::duration<double>(eagerStart).count();

	std::cout << "asyncLazy evaluated after : " << lazyDuration << " seconds." << std::endl;
	std::cout << "asyncEager evaluated after: " << eagerDuration << " seconds." << std::endl;

	std::cout << std::endl<< std::endl;
#pragma endregion Async2
	
#pragma region Async3
	// get NUM random numbers from 0 .. 100
	std::random_device seed;

	// generator
	std::mt19937 engine(seed());

	// distribution
	std::uniform_int_distribution<int> dist(0, 100);

	// fill the vectors
	std::vector<int> vsh, w;
	vsh.reserve(NUM);
	w.reserve(NUM);
	for (int i = 0; i < NUM; ++i) {
		vsh.push_back(dist(engine));
		w.push_back(dist(engine));
	}

	// measure the execution time
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	std::cout << "getDotProduct(v,w): " << getDotProduct(vsh, w) << std::endl;
	std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
	std::cout << "Parallel Execution: " << dur.count() << std::endl;

	std::cout << std::endl;
#pragma endregion Async3

#pragma endregion Async

#pragma region Acquire_Release_Fences
	std::cout << std::endl;

	std::thread t1A(producerAtomic);
	std::thread t2A(consumerAtomic);

	t1A.join();
	t2A.join();

	delete ptrs;

	std::cout << std::endl;
#pragma endregion Acquire_Release_Fences

#pragma region Acquire_Release_Fences_Barriers
	std::cout << std::endl;

	std::thread t1M(producerBarriers);
	std::thread t2M(consumerBarriers);

	t1M.join();
	t2M.join();

	delete ptrM;

	std::cout << std::endl;
#pragma endregion Acquire_Release_Fences_Barriers
	
	std::thread tAF(workOnResourceAF);
	std::thread tAF2(workOnResourceAF);

	tAF.join();
	tAF2.join();

	std::vector<std::thread> vCnt;
	for (int n = 0; n < 10; ++n) 
		vCnt.emplace_back(fCnt);
	
	for (auto& ts : vCnt) {
		ts.join();
	}
	std::cout << "Final counter value is " << cnt << '\n';

	return getchar();
}
