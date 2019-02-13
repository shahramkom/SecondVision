// Templates as first-class citizens in C++11
// Example code from http://vitiy.info/templates-as-first-class-citizens-in-cpp11/
// Written by Victor Laskin (victor.laskin@gmail.com)

#include "includes.h"

#define LOG cout
#define NL endl

// apply tuple to function...

namespace fn_detail {

	template<int ...>
	struct int_sequence {};

	template<int N, int ...S>
	struct gen_int_sequence : gen_int_sequence<N - 1, N - 1, S...> {};

	template<int ...S>
	struct gen_int_sequence<0, S...> {typedef int_sequence<S...> type;};

	template <typename F, typename... Args, int... S>
	inline auto fn_tuple_apply(int_sequence<S...>, const F& f, const std::tuple<Args...>& params) -> decltype(f((std::get<S>(params))...))
	{
		return f((std::get<S>(params))...);
	}
}

template <typename F, typename... Args> //f(std::declval<Args>()...)
inline auto fn_tuple_apply(const F& f, const std::tuple<Args...>& params) -> decltype(f(std::declval<Args>()...))
{
	return fn_detail::fn_tuple_apply(typename fn_detail::gen_int_sequence<sizeof...(Args)>::type(), f, params);
}
// end of apply tuple

// universal function / extended function wrapper !
template<typename F, typename TPackBefore = std::tuple<>, typename TPackAfter = std::tuple<>>
class fn_universal {
private:
	F f;                            ///< main functor
	TPackAfter after;               ///< curryed arguments
	TPackBefore before;             ///< curryed arguments
public:
	fn_universal(F&& f) : f(std::forward<F>(f)), after(std::tuple<>()), before(std::tuple<>()) {}
	fn_universal(const F& f, const TPackBefore& before, const TPackAfter& after) : f(f), after(after), before(before) {}
	template <typename... Args>
	auto operator()(Args... args) const -> decltype(fn_tuple_apply(f, std::tuple_cat(before, make_tuple(args...), after))) {
		// execute via tuple
		return fn_tuple_apply(f, std::tuple_cat(before, make_tuple(std::forward<Args>(args)...), after));
	}

	// curry
	template <typename T>
	auto curry(T&& param) const -> decltype(fn_universal<F, decltype(std::tuple_cat(before, std::make_tuple(param))), TPackAfter>(f, std::tuple_cat(before, std::make_tuple(param)), after))
	{
		return fn_universal<F, decltype(std::tuple_cat(before, std::make_tuple(param))), TPackAfter>(f, std::tuple_cat(before, std::make_tuple(std::forward<T>(param))), after);
	}

	template <typename T>
	auto curry_right(T&& param) const -> decltype(fn_universal<F, TPackBefore, decltype(std::tuple_cat(after, std::make_tuple(param)))>(f, before, std::tuple_cat(after, std::make_tuple(param))))
	{
		return fn_universal<F, TPackBefore, decltype(std::tuple_cat(after, std::make_tuple(param)))>(f, before, std::tuple_cat(after, std::make_tuple(std::forward<T>(param))));
	}
};

template <typename F>
auto fn_to_universal(F&& f) -> fn_universal<F>
{
	return fn_universal<F>(std::forward<F>(f));
}

// left curry by << operator
template<typename UF, typename Arg>
auto operator<<(const UF& f, Arg&& arg) -> decltype(f.template curry<Arg>(std::forward<Arg>(arg)))
{
	return f.template curry<Arg>(std::forward<Arg>(arg));
}

// right curry by >> operator
template<typename UF, typename Arg>
auto operator>>(const UF& f, Arg&& arg) -> decltype(f.template curry_right<Arg>(std::forward<Arg>(arg)))
{
	return f.template curry_right<Arg>(std::forward<Arg>(arg));
}

// OBJECT from TEMPLATE function!
// Template as first-class citizen


#define make_citizen(X) class tfn_##X { public: template <typename... Args> auto operator()(Args&&... args) const ->decltype(X(std::forward<Args>(args)...))  { return X(std::forward<Args>(args)...); } }



// tuple concatenation via << operator
template<typename... OldArgs, typename NewArg>
tuple<OldArgs..., NewArg> operator<<(const tuple<OldArgs...>& t, const NewArg& arg)
{
	return std::tuple_cat(t, std::make_tuple(arg));
}



template<typename T, class F>
auto operator|(T&& param, const F& f) -> decltype(f(param))  // typename std::result_of<F(T)>::type
{
	return f(std::forward<T>(param));
}


template <typename T>
void print(T t) {
	std::cout << t << std::endl;
}

template <typename T, typename...Ts>
void print(T&& first, Ts&& ... rest) {
	print(std::forward<T>(first));
	print(std::forward<Ts>(rest)...);
}
// template <typename... Args>
// void print(Args... args)
// {
// // 	(void)([]) { ((LOG << args), 0)... }; 
// // 	LOG << NL;
// }

make_citizen(print);

// Return count of elements as templated operator
template <typename T>
int count(const T & container)
{
	return container.size();
}

make_citizen(count);

/// Universal operators

// MAP
template <typename T, typename... TArgs, template <typename...>class C, typename F>
auto fn_map(const C<T, TArgs...> & container, const F & f) -> C<decltype(f(std::declval<T>()))>
{
	using resultType = decltype(f(std::declval<T>()));
	C<resultType> result;
	for (const auto& item : container)
		result.push_back(f(item));
	return result;
}

// REDUCE (FOLD)
template <typename TResult, typename T, typename... TArgs, template <typename...>class C, typename F>
TResult fn_reduce(const C<T, TArgs...> & container, const TResult & startValue, const F & f)
{
	TResult result = startValue;
	for (const auto& item : container)
		result = f(result, item);
	return result;
}

// FILTER
template <typename T, typename... TArgs, template <typename...>class C, typename F>
C<T, TArgs...> fn_filter(const C<T, TArgs...> & container, const F & f)
{
	C<T, TArgs...> result;
	for (const auto& item : container)
		if (f(item))
			result.push_back(item);
	return result;
}



#define make_universal(NAME, F) make_citizen(F); const auto NAME = fn_to_universal(tfn_##F());

make_universal(fmap, fn_map);
make_universal(reduce, fn_reduce);
make_universal(filter, fn_filter);


template <typename T, typename... Args>
T sum_impl(T arg, Args... args)
{
	T result = arg;
	[&result](...) {}((result += args, 0)...);
	return result;
}

make_universal(sum, sum_impl);

template <typename T, typename TName>
bool isNameEqualImpl(const T & obj, const TName & name)
{
	return (obj->name == name);
}

make_universal(isName, isNameEqualImpl);

template <typename T, typename TId>
bool isIdEqualImpl(const T & obj, const TId & id)
{
	return (obj->id == id);
}

make_universal(isId, isIdEqualImpl);

template <typename T>
bool isNotNullImpl(const T & t)
{
	return (t != nullptr);
}

make_universal(isNotNull, isNotNullImpl);


template <typename F, typename TKey, typename T, typename... TArgs, template <typename...>class C>
T findOneImpl(const C<T, TArgs...> & container, const F & f, const TKey & key)
{
	for (const auto& item : container)
		if (f(item, key))
			return item;
	return nullptr;
}

make_universal(ffind, findOneImpl);


// -------------------- chain of functors --------------------->
// The chain of functors ... is actualy just a tuple of functors
template <typename... FNs>
class fn_chain {
private:
	const std::tuple<FNs...> functions;

	template <std::size_t I, typename Arg>
	inline typename std::enable_if<I == sizeof...(FNs) - 1, decltype(std::get<I>(functions)(std::declval<Arg>())) >::type
		call(Arg arg) const
	{
		return std::get<I>(functions)(std::forward<Arg>(arg));
	}


	template <std::size_t N, std::size_t I, typename Arg>
	struct final_type : final_type<N - 1, I + 1, decltype(std::get<I>(functions)(std::declval<Arg>())) > {};

	template <std::size_t I, typename Arg>
	struct final_type<0, I, Arg> {
		using type = decltype(std::get<I>(functions)(std::declval<Arg>()));
	};

	template <std::size_t I, typename Arg>
	inline typename std::enable_if < I < sizeof...(FNs) - 1, typename final_type<sizeof...(FNs) - 1 - I, I, Arg>::type >::type
		call(Arg arg) const
	{
		return this->call<I + 1>(std::get<I>(functions)(std::forward<Arg>(arg)));
	}



public:
	fn_chain() : functions(std::tuple<>()) {}
	fn_chain(std::tuple<FNs...> functions) : functions(functions) {}

	// add function into chain
	template< typename F >
	inline auto add(const F& f) const -> fn_chain<FNs..., F>
	{
		return fn_chain<FNs..., F>(std::tuple_cat(functions, std::make_tuple(f)));
	}


	// call whole functional chain
	template <typename Arg>
	inline auto operator()(Arg arg) const -> decltype(this->call<0, Arg>(arg))

	{
		return call<0>(std::forward<Arg>(arg));
	}

};
// pipe function into functional chain via | operator
template<typename... FNs, typename F>
inline auto operator|(fn_chain<FNs...> && chain, F && f) -> decltype(chain.add(f))
{
	return chain.add(std::forward<F>(f));
}
// ------------------------------- Monads ---------------------------------->



enum class maybe_state { normal, empty };

template <typename T>
typename std::enable_if< std::is_object<decltype(T()) >::value, T>::type
set_empty() { return T(); }

template<> int set_empty<int>() { return 0; }
template<> string set_empty<string>() { return ""; }

template<typename T>
class maybe {
private:
	const maybe_state state;
	const T x;

	template <typename R>
	maybe<R> fromValue(R&& result) const
	{
		return maybe<R>(std::forward<R>(result));
	}

	template <typename R>
	maybe<std::shared_ptr<R>> fromValue(std::shared_ptr<R>&& result) const
	{
		if (result == nullptr)
			return maybe<std::shared_ptr<R>>();
		else
			return maybe<std::shared_ptr<R>>(std::forward<std::shared_ptr<R>>(result));
	}


public:
	// monadic return
	maybe(T && x) : x(std::forward<T>(x)), state(maybe_state::normal) {}
	maybe() : x(set_empty<T>()), state(maybe_state::empty) {}

	// monadic bind
	template <typename F>
	auto operator()(F f) const -> maybe<decltype(f(std::declval<T>()))>
	{
		using ResultType = decltype(f(std::declval<T>()));
		if (state == maybe_state::empty)
			return maybe<ResultType>();
		return fromValue(f(x));
	}

	// extract value
	T getOr(T && anotherValue) const { return (state == maybe_state::empty) ? anotherValue : x; };
};

template<typename T, typename F>
inline auto operator|(maybe<T> && monad, F && f) -> decltype(monad(f))
{
	return monad(std::forward<F>(f));
}


template<typename T, typename TDefault>
inline T operator||(maybe<T> && monad, TDefault && t)
{
	return monad.getOr(std::forward<TDefault>(t));
}

template <typename T>
maybe<T> just(T && t)
{
	return maybe<T>(std::forward<T>(t));
}



string xmlWrapImpl(string name, string item)
{
	return "<" + name + ">" + item + "</" + name + ">";
}

make_universal(xmlWrap, xmlWrapImpl);


/// Simple immutable data
class UserData {
public:
	const int id;
	const string name;
	const int parent;
	UserData(int id, string name, int parent) : id(id), name(name), parent(parent) {}
};

/// Shared pointer to immutable data
using User = std::shared_ptr<UserData>;

template <class T, class... P>
inline auto make(P && ... args) -> T {
	return std::make_shared<typename T::element_type>(std::forward<P>(args)...);
}

int mainIO() {
	// let's test citizenship...
	tfn_print print;
	print(5);
	print("hello");
	// let's call function by providing tuple
	auto f = [](int x, int y, int z) { return x + y - z; };
	auto params = make_tuple(1, 2, 3);
	auto res = fn_tuple_apply(f, params);
	print(res);
	// combine tuple using overloaded operators and apply function
	auto list = make_tuple(1, 4);
	auto res2 = fn_tuple_apply(f, list << 4);
	print(res2);
	// ok, now i want piped templates
	tfn_count count;
	vector<string> slist = { "one", "two", "three" };
	slist | count | print;
	// piping....
	auto ucount = fn_to_universal(count);
	auto uprint = fn_to_universal(print);
	slist | ucount | uprint;
	// currying....
	auto uf = fn_to_universal(f);
	auto uf1 = uf << 1;
	auto uf2 = uf1 << 2 << 5;
	uf2() | print;
	1 | (uf << 4 << 6) | print; // 4+6-1 = 9
	3 | (uf >> 6 >> 7) | print; // 3+6-7 = 2
	// count sum length of all strings in the list
	slist | (fmap >> count) | (reduce >> 0 >> sum) | (uprint << "Total: " >> " chars");
	slist | (reduce >> string("") >> sum) | (uprint << "All: ");
	vector<int>{1, 2, 3} | (reduce >> 0 >> sum) | (uprint << "Sum: ");
	
	// Some real objects...
	vector<User> users{ make<User>(1, "John", 0), make<User>(2, "Bob", 1), make<User>(3, "Max", 1) };
		users | (filter >> (isName >> "Bob")) | ucount | uprint;

	vector<int>{1, 2, 6} | (fmap >> (ffind << users << isId)) | (filter >> isNotNull) | ucount | uprint;

	// Produce XML
	users | fmap >> [](User u) { return u->name; } | fmap >> (xmlWrap << "name") | reduce >> string("") >> sum | xmlWrap << "users" | print;

	// Use functional chain:
	auto f1 = [](int x) { return x + 3; };
	auto f2 = [](int x) { return x * 2; };
	auto f3 = [](int x) { return (double)x / 2.0; };
	auto f4 = [](double x) { std::stringstream ss; ss << x; return ss.str(); };
	auto f5 = [](string s) { return "Result: " + s; };
	auto testChain = fn_chain<>() | f1 | f2 | f3 | f4 | f5;
	testChain(3) | print;

	auto countUsers = fn_chain<>() | (fmap >> (ffind << users << isId)) | (filter >> isNotNull) | ucount;
	vector<int>{1, 2, 6} | countUsers | (uprint << "count of users: ");


	// Ok, pipe through monadic execution!

	maybe<int>(2) | (ffind << users << isId) | [](User u) { return u->name; } | [](string s) { LOG << s << NL; return s; };
	(maybe<int>(6) | (ffind << users << isId) | [](User u) { return u->name; }).getOr("Not found") | (uprint << "Found user: ");
	just(vector<int>{1, 2, 6}) | countUsers | [&](int count) { count | (uprint << "Count: "); return count; };
	(just(vector<int>{1, 2, 6}) | countUsers || -1) | (uprint << "Count:");

	return getchar();
}