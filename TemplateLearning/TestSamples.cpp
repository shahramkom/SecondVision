// TestSamples.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "includes.h"

#pragma region enable if
template <class T>
typename std::enable_if<std::is_integral<T>::value, bool>::type is_odd(T i) {
  return bool(i % 2);
}

// 2. the second template argument is only valid if T is an integral type:
template <class T,
          class = typename std::enable_if<std::is_integral<T>::value>::type>
bool is_even(T i) {
  return !bool(i % 2);
}

#pragma endregion enable if

#pragma region declval
struct Default {
  int foo() const { return 1; }
};

struct NonDefault {
  NonDefault(const NonDefault &) {}
  int foo() const { return 1; }
};
struct AS { // abstract class
  virtual int value() = 0;
};

class BS : public AS { // class with specific constructor
  int val_;

public:
  BS(int i, int j) : val_(i * j) {}
  int value() { return val_; }
};
#pragma endregion declval

#pragma region removePointer
template <class T1, class T2> void print_is_same() {
  std::cout << "Remove Pointer: " << std::is_same<T1, T2>() << '\n';
}
void print_separator() { std::cout << "-----\n"; }
#pragma endregion removePointer

#pragma region AddPointer
typedef std::add_pointer<int>::type AF;       // int*
typedef std::add_pointer<const int>::type BF; // const int*
typedef std::add_pointer<int &>::type CF;     // int*
typedef std::add_pointer<int *>::type DF;     // int**
typedef std::add_pointer<int(int)>::type EF;  // int(*)(int)
#pragma endregion AddPointer

#pragma region isFunction
int ad(int i) { return i; } // function
int (*bd)(int) = ad;        // pointer to function
struct CD {
  int operator()(int i) { return i; }
} cd; // function-like class
#pragma endregion isFunction

#pragma region decay
template <typename T, typename U>
struct decay_equiv : std::is_same<typename std::decay<T>::type, U>::type {};
#pragma endregion decay

#pragma region Template Functions
template <int N> struct Factorial {
  static int const value = N * Factorial<N - 1>::value;
};

template <> struct Factorial<1> { static int const value = 1; };

int powFunc(int m, int n) {
  if (n == 0)
    return 1;
  return m * powFunc(m, n - 1);
}

template <int m, int n> struct PowMeta {
  static int const value = m * PowMeta<m, n - 1>::value;
};

template <int m> struct PowMeta<m, 0> { static int const value = 1; };

constexpr int powConst(int m, int n) {
  int r = 1;
  for (int k = 1; k <= n; ++k)
    r *= m;
  return r;
}

template <int...> struct mySum;

template <> struct mySum<> { static const int value = 0; };

template <int head, int... tail> struct mySum<head, tail...> {
  static const int value = head + mySum<tail...>::value;
};

template <int N, int M> struct Mult {
  static const int value = Mult<N, M - 1>::value + N;
};
template <int N> struct Mult<N, 1> { static const int value = N; };

template <int N> struct Mult<N, 0> { static const int value = 0; };

template <typename T1, typename T2>
typename std::common_type<T1, T2>::type gcd(T1 a, T2 b) {
  static_assert(std::is_integral<T1>::value, "T1 should be an integral type!");
  static_assert(std::is_integral<T2>::value, "T2 should be an integral type!");
  if (b == 0) {
    return a;
  } else {
    return gcd(b, a % b);
  }
}

template <class T> bool Integral() { return is_integral<T>::value; }

template <class T> bool SignedIntegral() {
  return Integral<T>() && is_signed<T>::value;
}

template <class T> bool UnsignedIntegral() {
  return Integral<T>() && !SignedIntegral<T>();
}

template <typename T1, typename T2>
typename std::conditional<(sizeof(T1) < sizeof(T2)), T1, T2>::type gcd(T1 a,
                                                                       T2 b) {
  static_assert(std::is_integral<T1>::value, "T1 should be an integral type!");
  static_assert(std::is_integral<T2>::value, "T2 should be an integral type!");
  if (b == 0) {
    return a;
  } else {
    return gcd(b, a % b);
  }
}
#pragma endregion

#pragma region Common_Type
struct Base {};
struct Derived : Base {};

typedef std::common_type<char, short, int>::type A;        // int
typedef std::common_type<float, double>::type B;           // double
typedef std::common_type<Derived, Base>::type C;           // Base
typedef std::common_type<Derived *, Base *>::type D;       // Base*
typedef std::common_type<const int, volatile int>::type E; // int
#pragma endregion Common_Type

#pragma region underlying_type
enum e1 {};
enum class e2 : int {};
enum class ASP { a, b, c };
enum BSP : short { x, y, z };
typedef std::underlying_type<ASP>::type ASP_under; // int
typedef std::underlying_type<BSP>::type BSP_under; // short
#pragma endregion underlying_type

#pragma region Conditional
typedef std::conditional<true, int, double>::type Type1;  // int
typedef std::conditional<false, int, double>::type Type2; // double
typedef std::conditional<sizeof(int) >= sizeof(double), int, double>::type
    Type3; // double

typedef std::conditional<true, int, float>::type Atp;  // int
typedef std::conditional<false, int, float>::type Btp; // float
typedef std::conditional<std::is_integral<A>::value, long, int>::type
    Ctp; // long
typedef std::conditional<std::is_integral<B>::value, long, int>::type
    Dtp; // int
#pragma endregion Conditional

#pragma region Fill
namespace my {
template <typename I, typename T, bool b>
void fill_impl(I first, I last, const T &val,
               const std::integral_constant<bool, b> &) {
  while (first != last) {
    *first = val;
    ++first;
  }
}

template <typename T>
void fill_impl(T *first, T *last, const T &val, const std::true_type &) {
  std::memset(first, val, last - first);
}

template <class I, class T> inline void fill(I first, I last, const T &val) {
  // typedef std::integral_constant<bool,std::has_trivial_copy_assign<T>::value
  // && (sizeof(T) == 1)> boolType;
  typedef std::integral_constant<
      bool, std::is_trivially_copy_assignable<T>::value && (sizeof(T) == 1)>
      boolType;
  fill_impl(first, last, val, boolType());
}
} // namespace my
const int arraySize = 100000000;
char charArray1[arraySize] = {
    0,
};
char charArray2[arraySize] = {
    0,
};
#pragma endregion Fill

#pragma region is_trivially
struct ACX {};
struct BCX : ACX {};
struct CCX {
  CCX &operator=(const ACX &) { return *this; }
};

class AM {};
class BM {
  BM() {}
};
class CM : BM {};
class DM {
  virtual void fn() {}
};

struct AE {};
struct BE {
  BE &operator=(const BE &) = delete;
};
#pragma endregion is_trivially

#pragma region is_assignable
struct AL {};
struct BL {
  BL &operator=(const AL &) { return *this; }
};

struct AT {};
struct BT {
  BT &operator=(AT &&) { return *this; }
};
#pragma endregion is_assignable

#pragma region Type Traits Library
struct ATT {
  int a;
  int f(int) { return 2011; }
};

enum ETT {
  e = 1,
};

union UTT {
  int u;
};
#pragma endregion Type Traits Library

template <typename T, // (1)
          typename std::enable_if<std::is_integral<T>::value, T>::type = 0>
T gcd(T a, T b) {
  if (b == 0) {
    return a;
  } else {
    return gcd(b, a % b); // (2)
  }
}

#pragma region SFINAE
void test(...) { std::cout << "Catch-all overload called\n"; }

// this overload is added to the set of overloads if
// C is a reference-to-class type and F is a pointer to member function of C
template <class C, class F>
auto test(C c, F f) -> decltype((void)(c.*f)(), void()) {
  std::cout << "\nReference overload called\n";
}

// this overload is added to the set of overloads if
// C is a pointer-to-class type and F is a pointer to member function of C
template <class C, class F>
auto test(C c, F f) -> decltype((void)((c->*f)()), void()) {
  std::cout << "Pointer overload called\n";
}

struct X {
  void f() {}
};
#pragma endregion SFINAE

#pragma region Function
double add(double a, double b) { return a + b; }

struct Sub {
  double operator()(double a, double b) { return a - b; }
};

double multThree(double a, double b, double c) { return a * b * c; }

#pragma endregion Function

#pragma region TagDispatching
template <typename InputIterator, typename Distance>
void advance_impl(InputIterator &i, Distance n, std::input_iterator_tag) {
  std::cout << "InputIterator used" << std::endl;
  while (n--)
    ++i;
}

template <typename BidirectionalIterator, typename Distance>
void advance_impl(BidirectionalIterator &i, Distance n,
                  std::bidirectional_iterator_tag) {
  std::cout << "BidirectionalIterator used" << std::endl;
  if (n >= 0)
    while (n--)
      ++i;
  else
    while (n++)
      --i;
}

template <typename RandomAccessIterator, typename Distance>
void advance_impl(RandomAccessIterator &i, Distance n,
                  std::random_access_iterator_tag) {
  std::cout << "RandomAccessIterator used" << std::endl;
  i += n;
}

template <typename InputIterator, typename Distance>
void advance_(InputIterator &i, Distance n) {
  typename std::iterator_traits<InputIterator>::iterator_category
      category;                 // (1)
  advance_impl(i, n, category); // (2)
}
#pragma endregion TagDispatching

#pragma region StaticAssert
template < typename T, int Line, int Column >
struct Matrix {
	static_assert(Line >= 0, "Line number must be positive.");
	static_assert(Column >= 0, "Column number must be positive.");
//	static_assert(Line + Column > 0, "Line and Column must be greater than 0.");//Show Error on compile Time
};

#pragma endregion StaticAssert

int mainSample1() {
  //////////////////////////////////////////////////////////////////////////
#pragma region Is_Pointer
  std::cout << std::boolalpha; // false
  std::cout << "\nstd::is_pointer<A>::value       " << std::is_pointer<A>::value
            << '\n'; // true
  std::cout << "std::is_pointer<A**>::value     "
            << std::is_pointer<A **>::value << '\n'; // true
  std::cout << "std::is_pointer<A*>::value      " << std::is_pointer<A *>::value
            << '\n'; // false
  std::cout << "std::is_pointer<A&>::value      " << std::is_pointer<A &>::value
            << '\n'; // false
  std::cout << "std::is_pointer<int>::value     " << std::is_pointer<int>::value
            << '\n'; // true
  std::cout << "std::is_pointer<int*>::value    "
            << std::is_pointer<int *>::value << '\n'; // true
  std::cout << "std::is_pointer<int**>::value   "
            << std::is_pointer<int **>::value << '\n'; // false
  std::cout << "std::is_pointer<int[10]>::value "
            << std::is_pointer<int[10]>::value << '\n'; // false
  std::cout << std::is_pointer<std::nullptr_t>::value << '\n';
#pragma endregion
  //////////////////////////////////////////////////////////////////////////
  uint64_t i = 127; // code does not compile if type of i is not integral
  std::cout << std::boolalpha;
  std::cout << "\ni is odd: " << is_odd(i) << std::endl;
  std::cout << "i is even: " << is_even(i) << std::endl;
  //////////////////////////////////////////////////////////////////////////
#pragma region declval
  decltype(Default().foo()) n1 = 1; // type of n1 is int
                                    //  decltype(NonDefault().foo()) n2 = n1; //
                                    //  error: no default constructor
  decltype(std::declval<NonDefault>().foo()) n2 = n1; // type of n2 is int
  std::cout << "n1 = " << n1 << '\n' << "n2 = " << n2 << '\n';

  decltype(std::declval<AS>().value()) as; // int a
  decltype(std::declval<BS>().value()) bs; // int b
  decltype(BS(0, 0).value()) cs;           // same as above (known constructor)
  as = bs = BS(10, 2).value();
  std::cout << as << '\n';
#pragma endregion
  //////////////////////////////////////////////////////////////////////////
#pragma region Remove Pointer
  std::cout << std::boolalpha;
  cout << endl;
  print_is_same<int, int>();    // true
  print_is_same<int, int *>();  // false
  print_is_same<int, int **>(); // false
  print_separator();
  print_is_same<int, std::remove_pointer<int>::type>();    // true
  print_is_same<int, std::remove_pointer<int *>::type>();  // true
  print_is_same<int, std::remove_pointer<int **>::type>(); // false
  print_separator();
  print_is_same<int, std::remove_pointer<int *const>::type>();          // true
  print_is_same<int, std::remove_pointer<int *volatile>::type>();       // true
  print_is_same<int, std::remove_pointer<int *const volatile>::type>(); // true
#pragma endregion
  //////////////////////////////////////////////////////////////////////////
#pragma region Is Same
  std::cout << std::boolalpha;
  std::cout << "\ntypedefs of int*:" << std::endl;
  std::cout << "std::is_same<int*, AF>::value A: "
            << std::is_same<int *, AF>::value << std::endl;
  std::cout << "std::is_same<int*, BF>::value B: "
            << std::is_same<int *, BF>::value << std::endl;
  std::cout << "std::is_same<int*, CF>::value C: "
            << std::is_same<int *, CF>::value << std::endl;
  std::cout << "std::is_same<int*, DF>::value D: "
            << std::is_same<int *, DF>::value << std::endl;
  std::cout << "std::is_same<int*, EF>::value E: "
            << std::is_same<int *, EF>::value << std::endl;
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region Is Function
  std::cout << std::boolalpha;
  std::cout << "\nis_function:" << std::endl;
  std::cout << "std::is_function<decltype(ad)>::value decltype(a): "
            << std::is_function<decltype(ad)>::value << std::endl;
  std::cout << "std::is_function<decltype(bd)>::value decltype(b): "
            << std::is_function<decltype(bd)>::value << std::endl;
  std::cout << "std::is_function<decltype(cd)>::value decltype(c): "
            << std::is_function<decltype(cd)>::value << std::endl;
  std::cout << "std::is_function<CD>::value C: " << std::is_function<CD>::value
            << std::endl;
  std::cout << "std::is_function<int(int)>::value int(int): "
            << std::is_function<int(int)>::value << std::endl;
  std::cout << "std::is_function<int(*)(int)>::value int(*)(int): "
            << std::is_function<int (*)(int)>::value << std::endl;
#pragma endregion
  //////////////////////////////////////////////////////////////////////////
#pragma region decay equiv
  std::cout << std::boolalpha << "\ndecay_equiv<int, int>::value              "
            << decay_equiv<int, int>::value << '\n'
            << "decay_equiv<int&, int>::value             "
            << decay_equiv<int &, int>::value << '\n'
            << "decay_equiv<int&&, int>::value            "
            << decay_equiv<int &&, int>::value << '\n'
            << "decay_equiv<const int&, int>::value       "
            << decay_equiv<const int &, int>::value << '\n'
            << "decay_equiv<int[2], int*>::value          "
            << decay_equiv<int[2], int *>::value << '\n'
            << "decay_equiv<int(int), int(*)(int)>::value "
            << decay_equiv<int(int), int (*)(int)>::value << '\n';
#pragma endregion
  ///////////////////////////////////////////////////////////////////////
#pragma region Template Function
  cout << endl;
  std::cout << Factorial<5>::value << std::endl;
  std::cout << powFunc(2, 10) << std::endl;        // 1024
  std::cout << PowMeta<2, 10>::value << std::endl; // 1024
  std::cout << powConst(2, 10) << std::endl;       // 1024

  int sum = mySum<1, 2, 3, 4, 5>::value; // 15

  std::cout << Mult<3, 2>::value << std::endl;

  std::list<int> lst{1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto itr = lst.begin();
  std::advance(itr, 2); // BidirectionalIterator

#pragma endregion

  // 	std::conditional <(sizeof(100) < sizeof(10LL)), long long, long>::type
  // uglyRes = gcd(100, 10LL); 	auto res = gcd(100, 10LL); 	auto res2 =
  // gcd(100LL, 10L); 	std::cout << "typeid(gcd(100,10LL)).name(): " <<
  // typeid(res).name()
  // << std::endl; 	std::cout << "typeid(gcd(100LL,10L)).name(): " <<
  // typeid(res2).name() << std::endl;

#pragma region typedefs
  std::cout << "\ntypedefs of int:" << std::endl;
  std::cout << "A: " << std::is_same<int, A>::value << std::endl;
  std::cout << "B: " << std::is_same<int, B>::value << std::endl;
  std::cout << "C: " << std::is_same<int, C>::value << std::endl;
  std::cout << "D: " << std::is_same<int, D>::value << std::endl;
  std::cout << "E: " << std::is_same<int, E>::value << std::endl;
#pragma endregion typedefs

#pragma region Remove Const
  std::cout << std::endl;
  std::cout << std::is_same<int, std::remove_const<int>::type>::value
            << std::endl;
  ; // true
  std::cout << std::is_same<int, std::remove_const<const int>::type>::value
            << std::endl;
  ; // true
  std::cout << std::is_same<int, std::remove_const_t<int>>::value << std::endl;
  ; // true
  std::cout << std::is_same<int, std::remove_const_t<const int>>::value
            << std::endl;
  ; // true
#pragma endregion Remove Const

#pragma region underlying_type
  bool e1_type =
      std::is_same<unsigned, typename std::underlying_type<e1>::type>::value;
  bool e2_type =
      std::is_same<int, typename std::underlying_type<e2>::type>::value;
  std::cout << "\nunderlying type for 'e1' is "
            << (e1_type ? "unsigned" : "non-unsigned") << '\n'
            << "underlying type for 'e2' is " << (e2_type ? "int" : "non-int")
            << '\n';

  std::cout << "\ntypedefs of int:" << std::endl;
  std::cout << "ASP_under: " << std::is_same<int, ASP_under>::value
            << std::endl;
  std::cout << "BSP_under: " << std::is_same<int, BSP_under>::value
            << std::endl;

  std::cout << std::endl;
  std::cout << typeid(Type1).name() << '\n';
  std::cout << typeid(Type2).name() << '\n';
  std::cout << typeid(Type3).name() << '\n';
#pragma endregion underlying_type

#pragma region typedefs
  std::cout << "\ntypedefs of int:" << std::endl;
  std::cout << "A: " << std::is_same<int, Atp>::value << std::endl;
  std::cout << "B: " << std::is_same<int, Btp>::value << std::endl;
  std::cout << "C: " << std::is_same<int, Ctp>::value << std::endl;
  std::cout << "D: " << std::is_same<int, Dtp>::value << std::endl;
#pragma endregion typedefs

#pragma region Fill
  auto begin = std::chrono::system_clock::now();
  my::fill(charArray1, charArray1 + arraySize, 1);
  auto last = std::chrono::system_clock::now() - begin;
  std::cout << "charArray1: " << std::chrono::duration<double>(last).count()
            << " seconds" << std::endl;

  begin = std::chrono::system_clock::now();
  my::fill(charArray2, charArray2 + arraySize, static_cast<char>(1));
  last = std::chrono::system_clock::now() - begin;
  std::cout << "charArray2: " << std::chrono::duration<double>(last).count()
            << " seconds" << std::endl;
#pragma endregion Fill

#pragma region is_trivially
  std::cout << "\nis_trivially_copy_assignable:" << std::endl;
  std::cout << "int=short: " << std::is_trivially_copy_assignable<int>::value
            << std::endl;
  std::cout << "A=A: " << std::is_trivially_copy_assignable<ACX>::value
            << std::endl;
  std::cout << "A=B: " << std::is_trivially_copy_assignable<ACX>::value
            << std::endl;
  std::cout << "B=A: " << std::is_trivially_copy_assignable<BCX>::value
            << std::endl;
  std::cout << "C=A: " << std::is_trivially_copy_assignable<CCX>::value
            << std::endl;

  std::cout << "\nis_trivially_move_assignable:" << std::endl;
  std::cout << "int=short: " << std::is_trivially_move_assignable<int>::value
            << std::endl;
  std::cout << "A=A: " << std::is_trivially_move_assignable<AT>::value
            << std::endl;
  std::cout << "B=A: " << std::is_trivially_move_assignable<BT>::value
            << std::endl;

  std::cout << "\nis_trivial:" << std::endl;
  std::cout << "int: " << std::is_trivial<int>::value << std::endl; // int: true
  std::cout << "A: " << std::is_trivial<AM>::value << std::endl;    // A: true
  std::cout << "B: " << std::is_trivial<BM>::value << std::endl;    // B: false
  std::cout << "C: " << std::is_trivial<CM>::value << std::endl;    // C: false
  std::cout << "D: " << std::is_trivial<DM>::value << std::endl;    // D: false
#pragma endregion is_trivially

#pragma region is_assignable
  std::cout << "\nis_copy_assignable:" << std::endl;
  std::cout << "int: " << std::is_copy_assignable<int>::value
            << std::endl; // int: true
  std::cout << "A: " << std::is_copy_assignable<AE>::value
            << std::endl; // A: true
  std::cout << "B: " << std::is_copy_assignable<BE>::value
            << std::endl; // B: true

  std::cout << "\nis_assignable:" << std::endl;
  std::cout << "A=B: " << std::is_assignable<AL, BL>::value
            << std::endl; // A=B: false
  std::cout << "B=A: " << std::is_assignable<BL, AL>::value
            << std::endl; // B=A: true
#pragma endregion is_assignable

#pragma region Type Traits Library
  std::cout << std::endl;
  std::cout << "std::is_void<void>::value ->" << std::is_void<void>::value
            << std::endl;
  std::cout << "std::is_integral<short>::value ->"
            << std::is_integral<short>::value << std::endl;
  std::cout << "std::is_floating_point<double>::value ->"
            << std::is_floating_point<double>::value << std::endl;
  std::cout << "std::is_array<int[]>::value ->" << std::is_array<int[]>::value
            << std::endl;
  std::cout << "std::is_pointer<int*>::value ->"
            << std::is_pointer<int *>::value << std::endl;
  std::cout << "std::is_null_pointer<std::nullptr_t>::value ->"
            << std::is_null_pointer<std::nullptr_t>::value << std::endl;
  std::cout << "std::is_member_object_pointer<int ATT::*>::value ->"
            << std::is_member_object_pointer<int ATT::*>::value << std::endl;
  std::cout << "std::is_member_function_pointer<int (ATT::*)(int)>::value ->"
            << std::is_member_function_pointer<int (ATT::*)(int)>::value
            << std::endl;
  std::cout << "std::is_enum<ETT>::value ->" << std::is_enum<ETT>::value
            << std::endl;
  std::cout << "std::is_union<UTT>::value ->" << std::is_union<UTT>::value
            << std::endl;
  std::cout << "std::is_class<std::string>::value ->"
            << std::is_class<std::string>::value << std::endl;
  std::cout << "std::is_function<int* (double)>::value ->"
            << std::is_function<int *(double)>::value << std::endl;
  std::cout << "std::is_lvalue_reference<int&>::value ->"
            << std::is_lvalue_reference<int &>::value << std::endl;
  std::cout << "std::is_rvalue_reference<int&&>::value ->"
            << std::is_rvalue_reference<int &&>::value << std::endl;
#pragma endregion Type Traits Library

#pragma region Trivial
  std::cout << std::endl;
  std::cout << "std::is_base_of<Base, Derived>::value: "
            << std::is_base_of<Base, Derived>::value << std::endl;
  std::cout << "std::is_base_of<Derived, Base>::value: "
            << std::is_base_of<Derived, Base>::value << std::endl;
  std::cout << "std::is_base_of<Derived, Derived>::value: "
            << std::is_base_of<Derived, Derived>::value << std::endl;
  std::cout << "std::is_convertible<Base*, Derived*>::value: "
            << std::is_convertible<Base *, Derived *>::value << std::endl;
  std::cout << "std::is_convertible<Derived*, Base*>::value: "
            << std::is_convertible<Derived *, Base *>::value << std::endl;
  std::cout << "std::is_convertible<Derived*, Derived*>::value: "
            << std::is_convertible<Derived *, Derived *>::value << std::endl;
  std::cout << "std::is_same<int, int32_t>::value: "
            << std::is_same<int, int32_t>::value << std::endl;
  std::cout << "std::is_same<int, int64_t>::value: "
            << std::is_same<int, int64_t>::value << std::endl;
  std::cout << "std::is_same<long int, int64_t>::value: "
            << std::is_same<long int, int64_t>::value << std::endl;
#pragma endregion Trivial

#pragma region SemiRegular
  std::cout << std::endl;
  operator<<(std::cout, "Argument-dependent lookup\n");
  std::cout << "gcd(100, 10)= " << gcd(100, 10) << std::endl;
  // std::cout << "gcd(3.5, 4)= " << gcd(3.5, 4.0) << std::endl;  error
  // C2668:'gcd' : ambiguous call to overloaded function
#pragma endregion SemiRegular

#pragma region SFINAE
  X x;
  test(x, &X::f);
  test(&x, &X::f);
  test(42, 1337);
#pragma endregion SFINAE

#pragma region Function
  std::cout << std::endl;
  std::map<const char, std::function<double(double, double)>> dispTable{
      // (1)
      {'+', add},                                       // (2)
      {'-', Sub()},                                     // (3)
      {'*', std::bind(multThree, 1, _1, _2)},           // (4)
      {'/', [](double a, double b) { return a / b; }}}; // (5)

  std::cout << "3.5 + 4.5 = " << dispTable['+'](3.5, 4.5) << std::endl;
  std::cout << "3.5 - 4.5 = " << dispTable['-'](3.5, 4.5) << std::endl;
  std::cout << "3.5 * 4.5 = " << dispTable['*'](3.5, 4.5) << std::endl;
  std::cout << "3.5 / 4.5 = " << dispTable['/'](3.5, 4.5) << std::endl;

#pragma endregion Function

#pragma region Lambda_Mutable
  std::cout << std::endl;
  std::vector<int> intVec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  std::for_each(intVec.begin(), intVec.end(), [sum = 0](int i) mutable {
	  sum += i;
	  std::cout << sum << std::endl;
  });
  std::cout << "\n";
#pragma endregion Lambda_Mutable
  
#pragma region Tag Dispatching
  std::cout << std::endl;

  std::vector<int> myVec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto myVecIt = myVec.begin(); // (3)
  std::cout << "*myVecIt: " << *myVecIt << std::endl;
  advance_(myVecIt, 5);
  std::cout << "*myVecIt: " << *myVecIt << std::endl;

  std::cout << std::endl;

  std::list<int> myList{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto myListIt = myList.begin(); // (4)
  std::cout << "*myListIt: " << *myListIt << std::endl;
  advance_(myListIt, 5);
  std::cout << "*myListIt: " << *myListIt << std::endl;

  std::cout << std::endl;

  std::forward_list<int> myForwardList{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto myForwardListIt = myForwardList.begin(); // (5)
  std::cout << "*myForwardListIt: " << *myForwardListIt << std::endl;
  advance_(myForwardListIt, 5);
  std::cout << "*myForwardListIt: " << *myForwardListIt << std::endl;

  std::cout << std::endl;
#pragma endregion Tag Dispatching

#pragma region StaticAssert
  static_assert(sizeof(void*) == 4, "32-bit addressing is required!");
  //static_assert(sizeof(void*) >= 8, "64-bit addressing is required!"); //Show Error on compile Time

  static_assert(sizeof(int) == sizeof(long int), "int and long int must be of the same length.");

  Matrix<int, 10, 5> intArray;
  Matrix<std::string, 3, 4> strArray;
  Matrix<double, 0, 1> doubleArray;
  Matrix<long long, 1, 0> longArray;

  Matrix<char, 0, 0> charArray;
#pragma endregion StaticAssert
  


  return getchar();
}
