// TestSamples.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "includes.h"

#pragma region enable if
template <class T>
typename std::enable_if<std::is_integral<T>::value, bool>::type
is_odd(T i) { return bool(i % 2); }

// 2. the second template argument is only valid if T is an integral type:
template < class T, class = typename std::enable_if<std::is_integral<T>::value>::type>
bool is_even(T i) { return !bool(i % 2); }

#pragma endregion enable if

#pragma region declval
	struct Default { int foo() const { return 1; } };

	struct NonDefault
	{
		NonDefault(const NonDefault&) { }
		int foo() const { return 1; }
	};
	struct AS {              // abstract class
		virtual int value() = 0;
	};

	class BS : public AS {    // class with specific constructor
		int val_;
	public:
		BS(int i, int j) :val_(i* j) {}
		int value() { return val_; }
	};
#pragma endregion declval

#pragma region removePointer
	template<class T1, class T2>
	void print_is_same()
	{
		std::cout << "Remove Pointer: " <<std::is_same<T1, T2>() << '\n';
	}
	void print_separator()
	{
		std::cout << "-----\n";
	}
#pragma endregion removePointer

#pragma region AddPointer
	typedef std::add_pointer<int>::type AF;        // int*
	typedef std::add_pointer<const int>::type BF;  // const int*
	typedef std::add_pointer<int&>::type CF;       // int*
	typedef std::add_pointer<int*>::type DF;       // int**
	typedef std::add_pointer<int(int)>::type EF;   // int(*)(int)
#pragma endregion AddPointer

#pragma region isFunction
	int ad(int i) { return i; }                           // function
	int(*bd)(int) = ad;                                   // pointer to function
	struct CD { int operator()(int i) { return i; } } cd;  // function-like class
#pragma endregion isFunction

#pragma region decay
	template <typename T, typename U>
	struct decay_equiv :
		std::is_same<typename std::decay<T>::type, U>::type
	{};
#pragma endregion decay


#pragma region Template Functions
	template <int N>
	struct Factorial {
		static int const value = N * Factorial<N - 1>::value;
	};

	template <>
	struct Factorial<1> {
		static int const value = 1;
	};

	int powFunc(int m, int n) {
		if (n == 0) return 1;
		return m * powFunc(m, n - 1);
	}

	template<int m, int n>
	struct PowMeta {
		static int const value = m * PowMeta<m, n - 1>::value;
	};

	template<int m>
	struct PowMeta<m, 0> {
		static int const value = 1;
	};

	constexpr int powConst(int m, int n) {
		int r = 1;
		for (int k = 1; k <= n; ++k) r *= m;
		return r;
	}

	template<int ...>
	struct mySum;

	template<>
	struct mySum<> {
		static const int value = 0;
	};

	template<int head, int ... tail>
	struct mySum<head, tail...> {
		static const int value = head + mySum<tail...>::value;
	};

	template <int N, int M>
	struct Mult {
		static const int value = Mult<N, M - 1>::value + N;
	};
	template <int N>
	struct Mult<N, 1> {
		static const int value = N;
	};

	template <int N>
	struct Mult<N, 0> {
		static const int value = 0;
	};


	template<typename T1, typename T2>
	typename std::common_type<T1, T2>::type gcd(T1 a, T2 b) {
		static_assert(std::is_integral<T1>::value, "T1 should be an integral type!");
		static_assert(std::is_integral<T2>::value, "T2 should be an integral type!");
		if (b == 0) { return a; }
		else {
			return gcd(b, a % b);
		}
	}

#pragma endregion



class A {};
int mainMain()
{
	//////////////////////////////////////////////////////////////////////////
#pragma region Is_Pointer
	std::cout << std::boolalpha; //false
	std::cout << "std::is_pointer<A>::value       " << std::is_pointer<A>::value << '\n'; //true
	std::cout << "std::is_pointer<A**>::value     " << std::is_pointer<A * *>::value << '\n'; //true
	std::cout << "std::is_pointer<A*>::value      " << std::is_pointer<A*>::value << '\n';//false
	std::cout << "std::is_pointer<A&>::value      " << std::is_pointer<A&>::value << '\n';//false
	std::cout << "std::is_pointer<int>::value     " << std::is_pointer<int>::value << '\n';//true
	std::cout << "std::is_pointer<int*>::value    " << std::is_pointer<int*>::value << '\n';//true
	std::cout << "std::is_pointer<int**>::value   " << std::is_pointer<int**>::value << '\n';//false
	std::cout << "std::is_pointer<int[10]>::value " << std::is_pointer<int[10]>::value << '\n';//false
	std::cout << std::is_pointer<std::nullptr_t>::value << '\n';
#pragma endregion
	//////////////////////////////////////////////////////////////////////////
	uint64_t i = 127;    // code does not compile if type of i is not integral
	std::cout << std::boolalpha;
	std::cout << "i is odd: " << is_odd(i) << std::endl;
	std::cout << "i is even: " << is_even(i) << std::endl;
	//////////////////////////////////////////////////////////////////////////
#pragma region declval
	decltype(Default().foo()) n1 = 1;                   // type of n1 is int
//  decltype(NonDefault().foo()) n2 = n1;               // error: no default constructor
	decltype(std::declval<NonDefault>().foo()) n2 = n1; // type of n2 is int
	std::cout << "n1 = " << n1 << '\n' << "n2 = " << n2 << '\n';

	decltype(std::declval<AS>().value()) as;  // int a
	decltype(std::declval<BS>().value()) bs;  // int b
	decltype(BS(0, 0).value()) cs;   // same as above (known constructor)
	as = bs = BS(10, 2).value();
	std::cout << as << '\n';
#pragma endregion
	//////////////////////////////////////////////////////////////////////////
#pragma region Remove Pointer
	std::cout << std::boolalpha;
	print_is_same<int, int>();   // true
	print_is_same<int, int*>();  // false
	print_is_same<int, int**>(); // false
	print_separator();
	print_is_same<int, std::remove_pointer<int>::type>();   // true
	print_is_same<int, std::remove_pointer<int*>::type>();  // true
	print_is_same<int, std::remove_pointer<int**>::type>(); // false
	print_separator();
	print_is_same<int, std::remove_pointer<int* const>::type>();          // true
	print_is_same<int, std::remove_pointer<int* volatile>::type>();       // true
	print_is_same<int, std::remove_pointer<int* const volatile>::type>(); // true
#pragma endregion
	//////////////////////////////////////////////////////////////////////////
#pragma region Is Same
	std::cout << std::boolalpha;
	std::cout << "typedefs of int*:" << std::endl;
	std::cout << "std::is_same<int*, AF>::value A: " << std::is_same<int*, AF>::value << std::endl;
	std::cout << "std::is_same<int*, BF>::value B: " << std::is_same<int*, BF>::value << std::endl;
	std::cout << "std::is_same<int*, CF>::value C: " << std::is_same<int*, CF>::value << std::endl;
	std::cout << "std::is_same<int*, DF>::value D: " << std::is_same<int*, DF>::value << std::endl;
	std::cout << "std::is_same<int*, EF>::value E: " << std::is_same<int*, EF>::value << std::endl;
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region Is Function
	std::cout << std::boolalpha;
	std::cout << "is_function:" << std::endl;
	std::cout << "std::is_function<decltype(ad)>::value decltype(a): " << std::is_function<decltype(ad)>::value << std::endl;
	std::cout << "std::is_function<decltype(bd)>::value decltype(b): " << std::is_function<decltype(bd)>::value << std::endl;
	std::cout << "std::is_function<decltype(cd)>::value decltype(c): " << std::is_function<decltype(cd)>::value << std::endl;
	std::cout << "std::is_function<CD>::value C: " << std::is_function<CD>::value << std::endl;
	std::cout << "std::is_function<int(int)>::value int(int): " << std::is_function<int(int)>::value << std::endl;
	std::cout << "std::is_function<int(*)(int)>::value int(*)(int): " << std::is_function<int(*)(int)>::value << std::endl;
#pragma endregion
	//////////////////////////////////////////////////////////////////////////
#pragma region decay equiv
	std::cout << std::boolalpha
		<< "decay_equiv<int, int>::value              " << decay_equiv<int, int>::value << '\n'
		<< "decay_equiv<int&, int>::value             " << decay_equiv<int&, int>::value << '\n'
		<< "decay_equiv<int&&, int>::value            " << decay_equiv<int&&, int>::value << '\n'
		<< "decay_equiv<const int&, int>::value       " << decay_equiv<const int&, int>::value << '\n'
		<< "decay_equiv<int[2], int*>::value          " << decay_equiv<int[2], int*>::value << '\n'
		<< "decay_equiv<int(int), int(*)(int)>::value " << decay_equiv<int(int), int(*)(int)>::value << '\n';
#pragma endregion
	///////////////////////////////////////////////////////////////////////
#pragma region Template Function
	std::cout << Factorial<5>::value << std::endl;
	std::cout << powFunc(2, 10) << std::endl;               // 1024
	std::cout << PowMeta<2, 10>::value << std::endl;        // 1024
	std::cout << powConst(2, 10) << std::endl;              // 1024

	int sum = mySum<1, 2, 3, 4, 5>::value;  // 15

	std::cout << Mult<3, 2>::value << std::endl;


	std::list<int> lst{ 1,2,3,4,5,6,7,8,9 };
	auto itr = lst.begin();
	std::advance(itr, 2);   // BidirectionalIterator
#pragma endregion

	std::cout << std::is_same<int, std::remove_const<int>::type>::value << std::endl;;        // true
	std::cout << std::is_same<int, std::remove_const<const int>::type>::value << std::endl;;  // true
	std::cout << std::is_same<int, std::remove_const_t<int>>::value << std::endl;;        // true
	std::cout << std::is_same<int, std::remove_const_t<const int>>::value << std::endl;;  // true

	return 0;
}