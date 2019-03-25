#include "includes.h"


#pragma region ATOMIC1
template <typename T>
T fetch_mult(std::atomic<T>& shared, T mult)
{
	// 1
	T oldValue = shared.load(); // 2
	while (!shared.compare_exchange_strong(oldValue, oldValue * mult)); // 3
	return oldValue;
}

#pragma endregion ATOMIC1

#pragma region ATOMIC2
std::atomic<int> S_cnt = {0};

void add()
{
	// 1
	for (auto n = 0; n < 1000; ++n)
		S_cnt.fetch_add(1, std::memory_order_relaxed); // 2
}
#pragma endregion ATOMIC2

#pragma region MATRIX
template <typename T, template <typename, typename> class Cont> // (1)
class Matrix
{
public:
	explicit Matrix(std::initializer_list<T> inList)
		: data(inList)
	{
		// (2)
		for (auto d : data)
			std::cout << d << " ";
	}

	int getSize() const
	{
		return data.size();
	}

private:
	Cont<T, std::allocator<T>> data; // (3)                               
};
#pragma endregion MATRIX

#pragma region MATRIX

template <typename T>
struct SMS
{
	template <typename U>
	void func()
	{
	}
};

template <typename T>
void func2()
{
	SMS<T> s;
	s.template func<T>(); // (1)
	s.template func<T>(); // (2)
}
#pragma endregion MATRIX

#pragma region FORWARD
template <typename T, typename ... Args>
T create(Args&& ... args)
{
	return T(std::forward<Args>(args)...);
}

struct MyStruct
{
	MyStruct(int i, double d, std::string s)
	{
	}
};
#pragma endregion FORWARD

#pragma region MultipleParams

bool allVarF()
{
	return true;
}

template <typename T, typename ...Ts>
bool allVarF(T t, Ts ... ts)
{
	return t && allVarF(ts...);
}

template <typename... Args>
bool allT(Args ... args)
{
	return (... && args);
}

template <typename... Args>
bool allTS(Args ... args)
{
	return (true && ... && args);
}

#pragma endregion MultipleParams


#include <type_traits>
#include <cstddef>

template <typename T>
using getName_t = decltype(std::declval<T&>().getName());

class Object
{
public:
	template <typename T>
	Object(const T& obj) : object(std::make_shared<Model<T>>(std::move(obj)))
	{
		//static_assert(std::experimental::is_detected < getName_t, decltype(obj)>::value, "No method getName available!");
	}

	std::string getName() const
	{
		return object->getName();
	}

	struct Concept
	{
		virtual ~Concept()
		= default;

		virtual std::string getName() const = 0;
	};

	template <typename T>
	struct Model final : Concept
	{
		explicit Model(const T& t) : object(t)
		{
		}

		std::string getName() const override
		{
			return object.getName();
		}

	private:
		T object;
	};

	std::shared_ptr<const Concept> object;
};

void printName(std::vector<Object> vec)
{
	for (const auto& v : vec)
		std::cout << v.getName() << std::endl;
}

struct Bar
{
	static std::string getName()
	{
		return "Bar";
	}
};

struct Foo
{
	static std::string getName()
	{
		return "Foo";
	}
};


template <typename...>
using void_t = void;

template <class T>
using copy_assign_t = decltype(std::declval<T&>() = std::declval<const T&>());

template <class T>
using diff_t = typename T::difference_type;


#pragma region POINT

template <typename T, int N>
struct Point
{
	Point(std::initializer_list<T> initList)
		: coord(initList)
	{
	}

	template <typename T2>
	Point<T, N>& operator=(const Point<T2, N>& point)
	{
		// (1)
		static_assert(std::is_convertible<T2, T>::value, "Cannot convert source type to destination type!");
		coord.clear();
		coord.insert(coord.begin(), point.coord.begin(), point.coord.end());
		return *this;
	}

	std::vector<T> coord;
};

#pragma endregion POINT


#pragma region MY_PRINT

void myPrintf(const char* format)
{
	// (3)
	std::cout << format;
}

template <typename T, typename ... Args>
void myPrintf(const char* format, T value, Args ... args)
{
	// (4)
	for (; *format != '\0'; format++) // (5)
	{
		if (*format == '%') // (6) 
		{
			std::cout << value;
			myPrintf(format + 1, args ...); // (7)
			return;
		}
		std::cout << *format; // (8)
	}
}
#pragma endregion MY_PRINT


///////////////////////////////M A I N///////////////////////////////////////////
int mainT3()
{
#pragma region ATOMIC1
	std::atomic<int> myInt{5};
	std::cout << myInt << std::endl;
	fetch_mult(myInt, 5);
	std::cout << myInt << std::endl;
#pragma endregion ATOMIC1

#pragma region ATOMIC2
	std::vector<std::thread> v;
	for (auto n = 0; n < 10; ++n)
	{
		v.emplace_back(add);
	}
	for (auto& t : v)
	{
		t.join();
	}

	std::cout << "Final counter value is " << S_cnt << '\n';
#pragma endregion ATOMIC2

#pragma region MATRIX
	std::cout << std::endl; // (4)
	Matrix<int, std::vector> myIntVec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::cout << std::endl;
	std::cout << "myIntVec.getSize(): " << myIntVec.getSize() << std::endl;

	std::cout << std::endl;
	Matrix<double, std::vector> myDoubleVec{1.1, 2.2, 3.3, 4.4, 5.5}; // (5)
	std::cout << std::endl;
	std::cout << "myDoubleVec.getSize(): " << myDoubleVec.getSize() << std::endl;

	std::cout << std::endl; // (6)
	Matrix<std::string, std::list> myStringList{"one", "two", "three", "four"};
	std::cout << std::endl;
	std::cout << "myStringList.getSize(): " << myStringList.getSize() << std::endl;
	std::cout << std::endl;
#pragma endregion MATRIX

#pragma region FORWARD
	std::cout << std::endl;
	// Lvalues
	int five = 5;
	int myFive = create<int>(five);
	std::cout << "myFive: " << myFive << std::endl;

	std::string str{"Lvalue"};
	std::string str2 = create<std::string>(str);
	std::cout << "str2: " << str2 << std::endl;

	// Rvalues
	int myFive2 = create<int>(5);
	std::cout << "myFive2: " << myFive2 << std::endl;

	std::string str3 = create<std::string>(std::string("Rvalue"));
	std::cout << "str3: " << str3 << std::endl;

	std::string str4 = create<std::string>(std::move(str3));
	std::cout << "str4: " << str4 << std::endl;

	// Arbitrary number of arguments
	double doub = create<double>();
	std::cout << "doub: " << doub << std::endl;

	MyStruct myStr = create<MyStruct>(2011, 3.14, str4);
#pragma endregion FORWARD

#pragma region MultipleParams
	std::cout << std::endl;


	std::cout << std::boolalpha;

	std::cout << "allVar(): " << allVarF() << std::endl;
	std::cout << "all(): " << allT() << std::endl;

	std::cout << "allVar(true): " << allVarF(true) << std::endl;
	std::cout << "all(true): " << allT(true) << std::endl;

	std::cout << "allVar(true, true, true, false): " << allVarF(true, true, true, false) << std::endl;
	std::cout << "all(true, true, true, false): " << allT(true, true, true, false) << std::endl;
#pragma endregion MultipleParams

	std::cout << std::endl;

	std::vector<Object> vec{Object(Foo()), Object(Bar())};
	printName(vec);

	std::cout << std::endl;

#pragma region POINT

	Point<double, 3> point1{1.1, 2.2, 3.3};
	Point<int, 3> point2{1, 2, 3};

	Point<int, 2> point3{1, 2};
	Point<std::string, 3> point4{"Only", "a", "test"};

	point1 = point2; // (3)

	// point2 = point3;                                     // (4)
	// point2 = point4;                                     // (5)
#pragma endregion POINT

#pragma region MY_PRINT
	myPrintf("\n"); // (1)
	myPrintf("% world% %\n", "Hello", '!', 2011); // (2)
	myPrintf("\n");
#pragma endregion MY_PRINT

	return getchar();
}
