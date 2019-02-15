#include "includes.h"
#include <omp.h>
#include <numeric>

#include "distance.h"
#include "unit.h"

using namespace Distance::Unit;

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
	auto handle = std::async(std::launch::async,
		parallel_sum<RandomIt>, mid, end);
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


int main()
{
#pragma region User Defined Literals
	cout << endl;
	std::cout << "1.0_km: " << 1.0_km << std::endl;
	std::cout << "1.0_m: " << 1.0_m << std::endl;
	std::cout << "1.0_dm: " << 1.0_dm << std::endl;
	std::cout << "1.0_cm: " << 1.0_cm << std::endl;

	std::cout << std::endl;

	std::cout << "0.001 * 1.0_km: " << 0.001 * 1.0_km << std::endl;
	std::cout << "10 * 1_dm: " << 10 * 1.0_dm << std::endl;
	std::cout << "100 * 1.0cm: " << 100 * 1.0_cm << std::endl;
	std::cout << "1_km / 1000: " << 1.0_km / 1000 << std::endl;

	std::cout << std::endl;
	std::cout << "1.0_km + 2.0_dm +  3.0_dm + 4.0_cm: " << 1.0_km + 2.0_dm + 3.0_dm + 4.0_cm << std::endl;
	std::cout << std::endl;

	auto work = 63.0_km;
	auto workPerDay = 2 * work;
	auto abbrevationToWork = 5400.0_m;
	auto workout = 2 * 1600.0_m;
	auto shopping = 2 * 1200.0_m;

	auto distPerWeek1 = 4 * workPerDay - 3 * abbrevationToWork + workout + shopping;
	auto distPerWeek2 = 4 * workPerDay - 3 * abbrevationToWork + 2 * workout;
	auto distPerWeek3 = 4 * workout + 2 * shopping;
	auto distPerWeek4 = 5 * workout + shopping;

	std::cout << "distPerWeek1: " << distPerWeek1 << std::endl;

	auto averageDistance = getAverageDistance({ distPerWeek1,distPerWeek2,distPerWeek3,distPerWeek4 });
	std::cout << "averageDistance: " << averageDistance << std::endl;

	std::cout << std::endl;
#pragma endregion User Defined Literals

#pragma region Chrono litral
	std::cout << std::endl;
	auto schoolHour = 45min;
	auto shortBreak = 300s;
	auto longBreak = 0.25h;
	auto schoolWay = 15min;
	auto homework = 2h;
	auto schoolDayInSeconds = 2 * schoolWay + 6 * schoolHour + 4 * shortBreak + longBreak + homework;
	std::cout << "School day in seconds: " << schoolDayInSeconds.count() << std::endl;
	std::cout << "School day in minutes: " << schoolDayInSeconds.count() / 60 << std::endl;
	std::cout << "School day in hours: " << schoolDayInSeconds.count() / 3600 << std::endl;
	std::cout << std::endl;
#pragma endregion Chrono litral

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
	std::cout << std::endl;

	int a = 0;
	int* b = 0;
	auto c = 0;
	std::cout << "C Type: "<<typeid(c).name() << std::endl;

	auto res = a + b + c;
	std::cout << "res value: " << res << std::endl;
	std::cout << "res Type Name: " << typeid(res).name() << std::endl;

	std::cout << std::endl;
#pragma endregion TypeInfo

#pragma region Concurrency::parallel_for
	int width = 10;
	std::cout << "Concurrency::parallel_for: ";
	Concurrency::parallel_for(0, width, [=](int x)
	{
		std::cout << x << " ";
	});
	std::cout << std::endl;
#pragma endregion Concurrency::parallel_for

#pragma region OpenMP
#pragma omp parallel
	printf("Hello, world.\n");

	auto startPar = chrono::steady_clock::now().time_since_epoch().count();
	int sab[100000] = { 0, };
#pragma omp parallel for
	for (int i = 0; i < 100000; i++) {
		sab[i] = 2 * i;
	}
	auto endPar = chrono::steady_clock::now().time_since_epoch().count();
	cout << "Duration parallel:" << (endPar - startPar) << endl;

	auto startNor = chrono::steady_clock::now().time_since_epoch().count();
	int saz[100000] = { 0, };
	for (int i = 0; i < 100000; i++) {
		saz[i] = 2 * i;
	}
	auto endNor = chrono::steady_clock::now().time_since_epoch().count();
	cout << "Duration normal:" << (endNor - startNor) << endl;

	int nthreads, tid;
	/* Fork a team of threads giving them their own copies of variables */
#pragma omp parallel private(nthreads, tid)
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
	std::vector<int> v(10000, 1);
	std::cout << "The sum is " << parallel_sum(v.begin(), v.end()) << '\n';


	concurrent_vector<int> vc(10000, 1);
	std::cout << "The sum Conc is " << parallel_sum(vc.begin(), vc.end()) << '\n';
#pragma endregion
	   	
#pragma region Package_task
	task_lambda();
	task_bind();
	task_thread();
#pragma endregion


	return getchar();
}
