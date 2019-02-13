#include "includes.h"
#include "NamedTuple.h"

template <class F>
class pipeable
{
private:
	F f;
public:
	pipeable(F&& f) : f(std::forward<F>(f)) {}

	template<class... Xs>
	auto operator()(Xs&& ... xs) -> decltype(std::bind(f, std::placeholders::_1, std::forward<Xs>(xs)...)) const {
		return std::bind(f, std::placeholders::_1, std::forward<Xs>(xs)...);
	}
};

template <class F>
auto piped(F&& f) { return pipeable<F>{std::forward<F>(f)}; }

template<class T, class F>
auto operator|(T&& x, const F& f) -> decltype(f(std::forward<T>(x)))
{
	return f(std::forward<T>(x));
}

template <typename T>
T whereInList(const T& list, std::function<bool(decltype(list.at(0)))> f)
{
	T result;
	for (auto& item : list)
		if (f(item))
			result.push_back(item);
	return result;
}

template <typename T>
T mapToList(const T & list, std::function<int(decltype(list.at(0)))> f)
{
	T result;
	for (auto& item : list)
		result.push_back(f(item));
	return result;
}

template <typename T>
T logList(const T & list)
{
	std::cout << "List: ";
	for (auto& item : list)
		std::cout << item << " ";
	std::cout << std::endl;
	return list;
}

template <typename F>
auto piped1Arg(F f) -> decltype(piped(std::bind(f, placeholders::_1)))
{
	return piped(std::bind(f, placeholders::_1));
}

template <typename F>
auto piped2Args(F f) -> decltype(piped(std::bind(f, placeholders::_1, placeholders::_2)))
{
	return piped(std::bind(f, placeholders::_1, placeholders::_2));
}

auto add = piped([](int x, int y) { return x + y; });
auto mul = piped([](int x, int y) { return x * y; });

vector<int> numbers{ 4,8,15,16,23,42 };
using setType = decltype(numbers);

auto where = piped2Args(whereInList<setType>);
auto myMap = piped2Args(mapToList<setType>);
auto myLg = piped1Arg(logList<setType>);


int mainPip()
{
	auto y2 = 5 | add(2) | piped([](int x, int y) { return x * y; })(5) | piped([](int x) { return x + 1; })(); // Output: 36
	auto y = 5 | add(2) | mul(5) | add(1); // Output: 36
	cout << "y:" << y << endl;
	cout << "y2:" << y2 << endl;

	auto result = numbers | where([](int x) { return (x > 10); }) | myMap([](int x) { return x + 5; }) | myLg();

	//////////////////////////////////////////////////////////////////////////
	auto student0 = make_named_tuple(param("GPA") = 3.8, param("grade") = 'A', param("name") = "Lisa Simpson");
	auto gpa = student0[param("GPA")];
	auto grade = student0[param("grade")];
	auto nam = student0[param("name")];
	cout << "GPA: " << gpa << ", "
		<< "grade: " << grade << ", "
		<< "name: " << nam << '\n';
	return 0;
}