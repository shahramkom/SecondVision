#include "includes.h"

template<class F>
class pipable {
private:
	F func;
public:
	pipable(F&& f) : func(std::forward<F>(f)) {}
	template <class... Args>
	auto operator() (Args... args) -> decltype(std::bind(func, _1, std::forward<Args>(args)...)) const 
	{
		return std::bind(func, _1, forward<Args>(args)...);
	}
};

template < class F>
auto piped(F&& f) {
	return pipable<F>{std::forward<F>(f)};
}

template<class T, class F>
auto operator | (T&& t, const F& f) -> decltype(f(std::forward<T>(t))) {
	return f(std::forward<T>(t));
}

auto add = piped([&](double x, double y) {return x + y;});
auto adds = piped([&](string x, string y) {return x + y;});

vector<int> numbers{ 1,5,9,7,5,3,15,13,19,4 };
using setType = decltype(numbers);

template<typename T>
auto whereInList(const T& lst, std::function<bool(decltype(lst.at(0)))> f) {
	T retval;
	for (const auto& it : lst)
		if(f(it))
			retval.emplace_back(it);
	return retval;
}

template<typename T>
auto mapToList(const T& lst, std::function<int(decltype(lst.at(0)))> f) {
	T retval;
	for (const auto& it : lst)
		retval.emplace_back(it);
	return retval;
}

template<typename T>
T logList(const T& lst) {
	cout << "{ ";
	std::copy(lst.begin(), lst.end(), ostream_iterator<int>(cout, " "));
	cout << "}";
	return lst;
}

template < typename F>
auto piped2Args(F f) -> decltype(piped(bind(f, _1, _2))){
	return piped(bind(f, _1, _2));
}

template<typename F>
auto piped1Arg(F f) -> decltype(piped(bind(f, _1))) {
	return piped(bind(f, _1));
}

auto _where = piped2Args(whereInList<setType>);
auto _myMap = piped2Args(mapToList<setType>);
auto _myLg = piped1Arg(logList<setType>);

int mainPipe()
{
	cout << "My Pipe Test: " << endl;
	auto d = 5 | add(7) | piped([&](double x, double y) {return x + y;})(15);
	auto dv = 5.2 | add(4.4);
	auto dp = "ali" | adds("Reza");
	cout << "result: " << d << " " << dv << " " << dp << endl;
	auto result = numbers | _where([](int x) { return (x > 5); }) | _myMap([](int x) { return x + 5; }) | _myLg();
	getchar();
	return 0;
}

