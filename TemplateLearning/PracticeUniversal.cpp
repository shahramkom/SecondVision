#include "includes.h"

template <typename T>
void printer(T t) {
	cout << t << endl;
}

template <typename T, typename...Ts>
void printer(T&& first, Ts&& ... rest) {
	printer(forward<T>(first));
	printer(forward<Ts>(rest)...);
}

void printTitle(string title, bool separate = true)
{
	if (separate)
		cout << "________________________________________________________________" << endl;
	cout << title << endl;
}

template <typename T>
int count(const T& container)
{
	return container.size();
}

template<typename T, class F>
auto operator|(T&& param, const F& f) -> decltype(f(param))
{
	return f(forward<T>(param));
}

#define prepareForTemplate(X) class tfn_##X {\
public:\
	template <typename... Args>\
	auto operator()(Args&&... args) const ->decltype(X(std::forward<Args>(args)...))\
	{ return X(std::forward<Args>(args)...); }}

prepareForTemplate(count);
prepareForTemplate(printer);

#pragma region Function Applier
//apply function on a tuple
namespace funcApplier
{
	template<int ...>
	struct intSequence {};

	template<int N, int ...S>
	struct generateIntSequence : generateIntSequence<N - 1, N - 1, S...> {};

	template<int ...S>
	struct generateIntSequence<0, S...> { typedef intSequence<S...> type; };

	template <typename F, typename... Args, int... S>
	inline auto applyFunctionToTuple(intSequence<S...>, const F& f, const tuple<Args...>& params) -> decltype(f((get<S>(params))...))
	{
		return f((get<S>(params))...);
	}
}

template <typename F, typename... Args> //f(declval<Args>()...)
inline auto applyFunctionToTuple(const F& f, const tuple<Args...>& params) -> decltype(f(declval<Args>()...))
{
	return funcApplier::applyFunctionToTuple(typename funcApplier::generateIntSequence<sizeof...(Args)>::type(), f, params);
}

template<typename... OldTupleItems, typename NewItem>
tuple<OldTupleItems..., NewItem> operator<<(const tuple<OldTupleItems...>& old, const NewItem& newitem)
{
	return tuple_cat(old, make_tuple(newitem));
}

#pragma endregion Function Applier

#pragma region Function Universal
template<typename F, typename TPackBefore = tuple<>, typename TPackAfter = tuple<>>
class FunctionUniversal {
private:
	F f;                            ///< main functor
	TPackBefore before;             ///< curryed arguments
	TPackAfter after;               ///< curryed arguments
public:
	FunctionUniversal(F&& f)
		: f(forward<F>(f))
		, after(tuple<>())
		, before(tuple<>())
	{}
	
	FunctionUniversal(const F& f, const TPackBefore& before, const TPackAfter& after)
		: f(f)
		, after(after)
		, before(before)
	{}
	
	template <typename... Args>
	auto operator()(Args... args) const -> decltype(applyFunctionToTuple(f, tuple_cat(before, make_tuple(args...), after)))
	{
		return applyFunctionToTuple(f, tuple_cat(before, make_tuple(forward<Args>(args)...), after));
	}

	template <typename T>
	auto curry(T&& param) const 
	{
		return FunctionUniversal<F, decltype(tuple_cat(before, make_tuple(param))), TPackAfter>(f, tuple_cat(before, make_tuple(forward<T>(param))), after);
	}

	template <typename T>
	auto curry_right(T&& param) const 
	{
		return FunctionUniversal<F, TPackBefore, decltype(tuple_cat(after, make_tuple(param)))>(f, before, tuple_cat(after, make_tuple(forward<T>(param))));
	}
};

template<typename UF, typename Arg>
auto operator<<(const UF& f, Arg&& arg) -> decltype(f.template curry<Arg>(forward<Arg>(arg)))
{
	return f.template curry<Arg>(forward<Arg>(arg));
}

template<typename UF, typename Arg>
auto operator>>(const UF& f, Arg&& arg) -> decltype(f.template curry_right<Arg>(forward<Arg>(arg)))
{
	return f.template curry_right<Arg>(forward<Arg>(arg));
}

template <typename F>
auto applyFunctionToUniversality(F&& f) -> FunctionUniversal<F>
{
	return FunctionUniversal<F>(forward<F>(f));
}


#pragma endregion Function Universal

#pragma region Filter Map
template <typename T, typename... TArgs, template <typename...>class C, typename F>
auto FunctionalMap(const C<T, TArgs...>& container, const F& f) -> C<decltype(f(declval<T>()))>
{
	using resultType = decltype(f(declval<T>()));
	C<resultType> result;
	for (const auto& item : container)
		result.emplace_back(f(item));
	return result;
}

template <typename TResult, typename T, typename... TArgs, template <typename...>class C, typename F>
TResult FunctionalReduce(const C<T, TArgs...> & container, const TResult& startValue, const F& f)
{
	TResult result = startValue;
	for (const auto& item : container)
		result = f(result, item);
	return result;
}

template <typename T, typename... Args>
T sum_impl(T arg, Args... args)
{
	T result = arg;
	[&result](...) {}((result += args, 0)...);
	return result;
}

#define prepareForUniversal(NAME, F) prepareForTemplate(F); const auto NAME = applyFunctionToUniversality(tfn_##F());

prepareForUniversal(fmap, FunctionalMap);
prepareForUniversal(reduce, FunctionalReduce);
prepareForUniversal(sum, sum_impl);

#pragma endregion Filter Map

#pragma region User Data Structure
template <typename T, typename... TArgs, template <typename...>class C, typename F>
C<T, TArgs...> Filter(const C<T, TArgs...> & container, const F & f)
{
	using resultType = C<T, TArgs...>;
	resultType result;
	for (const auto& item : container)
		if (f(item))
			result.emplace_back(item);
	return result;
}

template <typename F, typename TKey, typename ReturnType, typename... TArgs, template <typename...>class C>
ReturnType FindOneItem(const C<ReturnType, TArgs...>& container, const F& f, const TKey& key)
{
	for (const auto& item : container)
		if (f(item, key))
			return item;
	return nullptr;
}

template <typename ObjectDataStruct, typename SearchName>
bool isNameEqual(const ObjectDataStruct& obj, const SearchName& name)
{
	return (obj->name == name);
}

template <typename ObjectDataStruct, typename SearchId>
bool isIdEqual(const ObjectDataStruct& obj, const SearchId& id)
{
	return (obj->id == id);
}

template <typename ObjectDataStruct>
bool isNotNullImpl(const ObjectDataStruct& t)
{
	return (t != nullptr);
}

prepareForUniversal(filter, Filter);
prepareForUniversal(ffind, FindOneItem);
prepareForUniversal(isName, isNameEqual);
prepareForUniversal(isId, isIdEqual);
prepareForUniversal(isNotNull, isNotNullImpl);

class UserData {
public:
	const int id;
	const string name;
	const int parent;
	UserData(int id, string name, int parent) : id(id), name(name), parent(parent) {}
};
using User = shared_ptr<UserData>;

template <class ReturnType, class... P>
inline auto make(P&& ... args) -> ReturnType {
	return make_shared<typename ReturnType::element_type>(forward<P>(args)...);
}
#pragma endregion User Data Structure

#pragma region XML
string xmlWrapper(string name, string item)
{
	return "<" + name + ">" + item + "</" + name + ">";
}

prepareForUniversal(xmlWrap, xmlWrapper);

#pragma endregion XML

#pragma region chain
template <typename... Functions>
class Chains {
private:
	const tuple<Functions...> functions;

	template <size_t I, typename Arg>
	inline typename enable_if<I == sizeof...(Functions) - 1, decltype(get<I>(functions)(declval<Arg>()))>::type call(Arg arg) const
	{
		return get<I>(functions)(forward<Arg>(arg));
	}

	template <size_t N, size_t I, typename Arg>
	struct final_type : final_type<N - 1, I + 1, decltype(get<I>(functions)(declval<Arg>()))> {};

	template <size_t I, typename Arg>
	struct final_type<0, I, Arg> {
		using type = decltype(get<I>(functions)(declval<Arg>()));
	};

	template <size_t I, typename Arg>
	inline typename enable_if <I<sizeof...(Functions) - 1, typename final_type<sizeof...(Functions) - 1 - I, I, Arg>::type>::type call(Arg arg) const
	{
		return this->call<I + 1>(get<I>(functions)(forward<Arg>(arg)));
	}
	   
public:
	Chains() : functions(tuple<>()) {}
	Chains(tuple<Functions...> functions)
		: functions(functions)
	{}

	template< typename Function >
	inline auto add(const Function& f) const -> Chains<Functions..., Function>
	{
		return Chains<Functions..., Function>(tuple_cat(functions, make_tuple(f)));
	}

	template <typename Arg>
	inline auto operator()(Arg arg) const -> decltype(this->call<0, Arg>(arg))
	{
		return call<0>(forward<Arg>(arg));
	}
};

template<typename... Functions, typename Function>
inline auto operator|(Chains<Functions...>&& chain, Function&& f) -> decltype(chain.add(f))
{
	return chain.add(forward<Function>(f));
}

#pragma endregion chain

#pragma region MayBe
enum class MaybeState { NORMAL, EMPTY };

template <typename T>
typename enable_if< is_object<decltype(T()) >::value, T>::type
setEmpty() { return T(); }

template<> int setEmpty<int>() { return 0; }
template<> string setEmpty<string>() { return ""; }

template<typename T>
class Maybe {
private:
	const MaybeState state;
	const T x;

	template <typename R>
	Maybe<R> fromValue(R&& result) const
	{
		return Maybe<R>(forward<R>(result));
	}

	template <typename R>
	Maybe<shared_ptr<R>> fromValue(shared_ptr<R>&& result) const
	{
		return (result == nullptr) ? Maybe<shared_ptr<R>>() : Maybe<shared_ptr<R>>(forward<shared_ptr<R>>(result));
	}

public:
	Maybe(T&& x)
		: x(std::forward<T>(x))
		, state(MaybeState::NORMAL)
	{}
	
	Maybe()
		: x(setEmpty<T>())
		, state(MaybeState::EMPTY)
	{}

	template <typename F>
	auto operator()(F f) const -> Maybe<decltype(f(declval<T>()))>
	{
		using ResultType = decltype(f(declval<T>()));
		if (state == MaybeState::EMPTY)
			return Maybe<ResultType>();
		return fromValue(f(x));
	}

	T getOr(T&& anotherValue) const
	{ 
		return (state == MaybeState::EMPTY) ? anotherValue : x;
	};
};

template<typename T, typename F>
inline auto operator|(Maybe<T> && monad, F && f) -> decltype(monad(f))
{
	return monad(forward<F>(f));
}

template<typename T, typename TDefault>
inline T operator||(Maybe<T> && monad, TDefault && t)
{
	return monad.getOr(forward<TDefault>(t));
}

template <typename T>
Maybe<T> just(T && t)
{
	return Maybe<T>(forward<T>(t));
}

#pragma endregion MayBe

int mainPU()
{
	printTitle("My Universal Test:",false);
	tfn_printer printer;
//////////////////////////////////////////////////////////////////////////
#pragma region sample 1
	printTitle("__Apply Function To tuple", false);
	auto func = [](int x, int y, int z) { return x + y - z; };
	auto params = make_tuple(10, 20, 3);
	auto res = applyFunctionToTuple(func, params);
	printer(res);
#pragma endregion sample 1
//////////////////////////////////////////////////////////////////////////
#pragma region sample 2
	printTitle("__Apply Function To tuple 2");
	auto list = make_tuple(1, 4);
	auto res2 = applyFunctionToTuple(func, list << 4);
	printer(res2);
	cout << endl;
#pragma endregion sample 2
//////////////////////////////////////////////////////////////////////////
	tfn_count count;
	vector<string> slist = { "one", "two", "three", "four" };
	slist | count | printer;
//////////////////////////////////////////////////////////////////////////
#pragma region sample 3
	printTitle("__Apply Function To Universality");
	slist.emplace_back("five");
	auto ucount = applyFunctionToUniversality(count);
	auto uprint = applyFunctionToUniversality(printer);
	slist | ucount | uprint;
	auto uf = applyFunctionToUniversality(func);
	auto uf1 = uf << 1;
	auto uf2 = uf1 << 2 << 5;
	1 | (uf << 4 << 6) | printer; // 4+6-1 = 9
	3 | (uf >> 6 >> 7) | printer; // 3+6-7 = 2
#pragma endregion sample 3
//////////////////////////////////////////////////////////////////////////
#pragma region sample 4
	printTitle("__Universality Section 2");
	slist | (fmap >> count) | (reduce >> 0 >> sum) | (uprint << "Total: " >> " chars");
	slist | (reduce >> string(" ") >> sum) | (uprint << "All: ");
	vector<int>{1, 2, 3} | (reduce >> 0 >> sum) | (uprint << "Sum: ");
#pragma endregion sample 4
//////////////////////////////////////////////////////////////////////////
#pragma region sample 5
	printTitle("__User Data");
	vector<User> users{ make<User>(1, "John", 0), make<User>(2, "Bob", 1), make<User>(3, "Max", 1) };
	users | (filter >> (isName >> "Bob")) | ucount | uprint << "Bob user: ";
	vector<int>{1, 2, 6} | (fmap >> (ffind << users << isId)) | (filter >> isNotNull) | ucount | uprint<<"Bob Id in New DS:" ;
#pragma endregion sample 5
//////////////////////////////////////////////////////////////////////////
#pragma region sample 6
	printTitle("__Prepare XML");
	users | fmap >> [](User u) { return u->name; } | fmap >> (xmlWrap << "name") | reduce >> string("") >> sum | xmlWrap << "users" | printer;
#pragma endregion sample 6
//////////////////////////////////////////////////////////////////////////
#pragma region sample 7
	printTitle("__Chain of Functions");
	auto f1 = [](int x) { return x + 3; };
	auto f2 = [](int x) { return x * 2; };
	auto f3 = [](int x) { return (double)x / 2.0; };
	auto f4 = [](double x) { stringstream ss; ss << x; return ss.str(); };
	auto f5 = [](string s) { return "Result: " + s; };
	auto testChain = Chains<>() | f1 | f2 | f3 | f4 | f5;
	testChain(3) | printer;
#pragma endregion sample 7
//////////////////////////////////////////////////////////////////////////
#pragma region sample 8
	printTitle("__Chain of Functions 2");
	auto countUsers = Chains<>() | (fmap >> (ffind << users << isId)) | (filter >> isNotNull) | ucount;
	vector<int>{1, 2, 6} | countUsers | (uprint << "count of users: ");
#pragma endregion sample 8
//////////////////////////////////////////////////////////////////////////
#pragma region sample 9
	printTitle("__Maybe");
	Maybe<int>(2) | (ffind << users << isId) | [](User u) { return u->name; } | [](string s) { cout << s << endl; return s; };
	(Maybe<int>(6) | (ffind << users << isId) | [](User u) { return u->name; }).getOr("Not found") | (uprint << "Found user: ");
	just(vector<int>{1, 2, 6}) | countUsers | [&](int count) { count | (uprint << "Count: "); return count; };
	(just(vector<int>{1, 2, 6}) | countUsers || -1) | (uprint << "Count:");
#pragma endregion sample 9
//////////////////////////////////////////////////////////////////////////

	return getchar();
}