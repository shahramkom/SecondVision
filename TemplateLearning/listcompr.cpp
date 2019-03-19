// List comprehension in C++11 in form of SQL-like syntax
// Example code from http://vitiy.info/cpp11-writing-list-comprehension-in-form-of-sql/
// Written by Victor Laskin (victor.laskin@gmail.com)

#include "includes.h"

// This is for converting lambda / pointer into correponding std::function<>

template <typename Fun>
struct fn_is_ptr : std::integral_constant<bool, std::is_pointer<Fun>::value
	&& std::is_function< typename std::remove_pointer<Fun>::type>::value>
{};

template <typename ReturnType, typename... Args, class = typename std::enable_if<fn_is_ptr< ReturnType(*)(Args...) >::value>::type>
std::function<ReturnType(Args...)> to_fn(ReturnType(*fp)(Args...))
{
	return std::function<ReturnType(Args...)>(fp);
}


template <typename Function>
struct function_traits : public function_traits<decltype(&Function::operator())>
{};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
{
	typedef ReturnType(*pointer)(Args...);
	typedef std::function<ReturnType(Args...)> function;
};

template <typename Function, class = typename std::enable_if<!fn_is_ptr<Function>::value>::type>
typename function_traits<Function>::function to_fn(Function & lambda)
{
	return typename function_traits<Function>::function(lambda);
}

template <typename Function, class = typename std::enable_if<!fn_is_ptr<Function>::value>::type>
typename function_traits<Function>::function to_fn(const Function & lambda)
{
	return typename function_traits<Function>::function(lambda);
}

// Range of natural integers
class Naturals
{
	int minN;
	int maxN;
public:
	Naturals() : minN(1), maxN(1000) {}
	Naturals(int minN, int maxN) : minN(minN), maxN(maxN) {}
	int at(int i) const { return i + minN; };
	int size() const { return maxN - minN + 1; };

	class Iterator {
		int position;
	public:
		Iterator(int _position) :position(_position) {}
		int& operator*() { return position; }
		Iterator& operator++() { ++position; return *this; }
		bool operator!=(const Iterator& it) const { return position != it.position; }
	};

	Iterator begin() const { return { minN }; }
	Iterator end()   const { return { maxN }; }
};


// Additional options
class SelectOption {
private:
	virtual bool imPolymorphic() { return true; }
};

class SelectOptionLimit : public SelectOption {
public:
	int limit;
	SelectOptionLimit(int limit) : limit(limit) {}
};

class SelectOptionSort : public SelectOption {};

class SelectOptionDistict : public SelectOption {};

class SelectContinuation : public SelectOption {
public:
	vector<int> indexes;
	SelectContinuation() {}
	template <typename... Args>
	SelectContinuation(Args... args) : indexes{ args... } { }
};

/// Main iterations
template <typename Src, typename... Args>
inline typename std::enable_if< std::is_object<decltype(std::declval<Src>().begin()) >::value, Src&&>::type
getSource(Src&& src, Args&& ... args) {
	return std::forward<Src&&>(src);
}

template <typename F, typename... Args, typename FRes = decltype(std::declval<F>()(std::declval<Args>()...))>
inline typename std::enable_if<std::is_object< decltype(std::declval<F>()(std::declval<Args>()...)) >::value, FRes  >::type
getSource(F&& f, Args&& ... args)
{
	return std::forward<FRes>(f(std::forward<Args>(args)...));
}

template<std::size_t I = 0, typename FuncT, typename... Tp, typename... Args>
inline typename std::enable_if<I == sizeof...(Tp), bool>::type
for_each_in_sources(const std::tuple<Tp...>&, FuncT& f, Args& ... args)
{
	return f(args...);
}

template<std::size_t I = 0, typename FuncT, typename... Tp, typename... Args>
inline typename std::enable_if < I < sizeof...(Tp), bool>::type
	for_each_in_sources(const std::tuple<Tp...>& t, FuncT& f, Args& ... args)
{
	bool isFinished;
	auto&& src = getSource(std::get<I>(t), args...);
	for (auto& element : src)
	{
		isFinished = for_each_in_sources<I + 1, FuncT, Tp...>(t, f, args..., element);
		if (isFinished) break;
	}
	return isFinished;
}

template<std::size_t I = 0, typename FuncT, typename... Tp, typename... Args>
inline typename std::enable_if<I == sizeof...(Tp), bool>::type
for_each_in_sources_indexed(const std::tuple<Tp...>&, FuncT & f, vector<int> & indexes, Args && ... args)
{
	return f(args...);
}

template<std::size_t I = 0, typename FuncT, typename... Tp, typename... Args>
inline typename std::enable_if < I < sizeof...(Tp), bool>::type
	for_each_in_sources_indexed(const std::tuple<Tp...> & t, FuncT & f, vector<int> & indexes, Args && ... args)
{
	bool isFinished;
	auto&& src = getSource(std::get<I>(t), args...);
	int size = src.size();
	int i = indexes[I];
	if (I != 0) if (i >= size) i = 0;
	if (i >= size) return false;
	for (; i < size; i++)
	{
		isFinished = for_each_in_sources_indexed<I + 1, FuncT, Tp...>(t, f, indexes, args..., src.at(i));
		if (isFinished) break;
	}
	indexes[I] = ++i;
	return isFinished;
}

template <typename R, typename... Args, typename... Sources, typename... Options>
vector<R> select(const std::function<R(Args...)> & f,                ///< output expression
	const std::tuple<Sources...> & sources,             ///< list of sources
	const std::function<bool(Args...)> & filter,        ///< composition of filters
	const Options & ... options)                          ///< other options and flags
{
	int limit = -1;
	bool isDistinct = false;
	bool isSorted = false;
	vector<int>* indexes = nullptr;

	for_each_argument([&](const SelectOption & option) {
		if (auto opt = dynamic_cast<const SelectOptionLimit*>(&option)) { limit = opt->limit; };
		if (dynamic_cast<const SelectOptionSort*>(&option)) { isSorted = true; };
		if (dynamic_cast<const SelectOptionDistict*>(&option)) { isDistinct = true; };
		if (auto opt = dynamic_cast<const SelectContinuation*>(&option)) { indexes = &((const_cast<SelectContinuation*>(opt))->indexes); };
	}, (options)...);

	vector<R> result;
	int count = 0;
	auto process = [&](const Args & ... args) {
		if (filter(args...))
		{
			result.push_back(f(args...));
			count++;
		}
		return (count == limit); // isFinished
	};

	if (indexes == nullptr)
		for_each_in_sources(sources, process);
	else
	{
		if (indexes->size() == 0)
		{
			indexes->resize(std::tuple_size<std::tuple<Sources...>>::value);
			std::fill(indexes->begin(), indexes->end(), 0);
		}

		for_each_in_sources_indexed(sources, process, *indexes);
	}

	// sort results
	if ((isDistinct) || (isSorted))
		std::sort(result.begin(), result.end());

	// remove duplicates
	if (isDistinct)
	{
		auto last = std::unique(result.begin(), result.end());
		result.erase(last, result.end());
	}

	return result;
}

template <typename R, typename... Args, typename... Sources, typename... Options>
inline vector<R> select(std::function<R(Args...)> f,
	const std::tuple<Sources...> & sources,
	const Options & ... options)
{
	return select(f, sources, to_fn([](Args... args) { return true; }), options...);
}

// EVIL WAY

#define SELECT(X) select(to_fn(X),
//#define FROM(...) std::forward_as_tuple(__VA_ARGS__)
#define FROM(...) std::make_tuple(__VA_ARGS__)
#define WHERE(...) ,to_fn(__VA_ARGS__)
#define SORT ,SelectOptionSort()
#define DISTINCT ,SelectOptionDistict()
#define LIMIT(X) ,SelectOptionLimit(X))
#define NOLIMIT LIMIT(-1)


template <typename R, typename... Args, typename... Sources, typename... Options>
std::function<vector<R>()> select_lazy(const std::function<R(Args...)> & f,                ///< output expression
	const std::tuple<Sources...> & sources,             ///< list of sources
	const std::function<bool(Args...)> & filter,        ///< composition of filters
	const Options & ... options)                         ///< other options and flags
{
	return to_fn([=]() { return select(f, sources, filter, options...); });
}


template <typename R, typename... Args, typename... Sources, typename... Options>
std::function<vector<R>(const SelectContinuation & job)> select_concurrent(
	const std::function<R(Args...)> & f,                ///< output expression
	const std::tuple<Sources...> & sources,             ///< list of sources
	const std::function<bool(Args...)> & filter,        ///< composition of filters
	const Options & ... options)                        ///< other options and flags
{
	return to_fn([=](const SelectContinuation & job) { return select(f, sources, filter, job, options...); });
}

#define CONCURRENTSELECT(X) select_concurrent(to_fn(X),
#define LAZYSELECT(X) select_lazy(to_fn(X),
#define LAZY(X) ,(X)


/// Call function for each argument
template <class F, class... Args>
void for_each_argument(F f, Args && ... args) {
	(void) int[] { (f(forward<Args>(args)), 0)... };
}

int main() {

	// EXAMPLES

	auto print = [](vector<int> & list) { cout << "List: "; for (int x : list) cout << x << " "; cout << endl;};

	cout << "10 naturals" << endl;
	auto result = SELECT([](int x) { return x; }) FROM(Naturals()) LIMIT(10);
	print(result);

	cout << "Distinct test" << endl;
	result = SELECT([](int x, int y) { return x + y; }) FROM(Naturals(), Naturals())
		WHERE([](int x, int y) { return (x * x + y * y < 25); }) DISTINCT LIMIT(10);
	print(result);

	cout << "Intersection" << endl;
	vector<int> ints = { 11,2,1,5,6,7 };
	vector<int> ints2 = { 3,4,5,7,8,11 };

	result = SELECT([](int x, int y) { return x; })
		FROM(ints, ints2)
		WHERE([](int x, int y) { return (x == y); }) SORT NOLIMIT;
	print(result);

	// Simple map operation as list comprehension
	cout << "Map" << endl;
	result = SELECT([](int x) { return x + 5; }) FROM(ints) NOLIMIT;
	print(result);

	cout << "Pythagorean Triplets" << endl;
	SelectContinuation state;

	auto lazypyth = LAZYSELECT([&](int x, int y, int z) { return make_tuple(z, y, x); })
		FROM(Naturals(1, 100000000),
			[](int x) { return Naturals(1, x); },
			[](int x, int y) { return Naturals(1, y); })
		WHERE([&](int x, int y, int z) {  return x * x == y * y + z * z; })
		LAZY(state) LIMIT(5);

	auto pyth = lazypyth();
	cout << "Part1:" << endl;
	for (auto line : pyth) cout << " -> " << get<0>(line) << " " << get<1>(line) << " " << get<2>(line) << endl;
	pyth = lazypyth();
	cout << "Part2:" << endl;
	for (auto line : pyth) cout << " -> " << get<0>(line) << " " << get<1>(line) << " " << get<2>(line) << endl;

	return getchar();
}