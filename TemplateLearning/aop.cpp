// This is example from
// http://vitiy.info/c11-functional-decomposition-easy-way-to-do-aop/
// by Victor Laskin

#include "includes.h"

#define LOG std::cout
#define NL std::endl

/// Simple immutable data
class UserData {
public:
	const uint16_t id;
	const string name;
	const uint16_t parent;
	UserData(uint16_t id, string name, uint16_t parent)
		: id(id)
		, name(name)
		, parent(parent)
	{}
};

/// Error type - code + description
class Error {
public:
	Error(uint16_t code, string message)
		: code(code)
		, message(message)
	{}
	Error(const Error& e) 
		: code(e.code)
		, message(e.message)
	{}
	const uint16_t code;
	const string message;
};

/// Shared pointer to immutable data
using User = std::shared_ptr<UserData>;

/// MayBe monad from Haskel over shared_ptr
/// with additional error field
template < typename T >
class Maybe {
private:
	const T data;
	const shared_ptr<Error> error;
public:
	Maybe() 
		: data(nullptr)
		, error(nullptr)
	{}
	Maybe(decltype(nullptr) nothing)
		: data(nullptr)
		, error(nullptr)
	{}
	Maybe(T data)
		: data(std::forward<T>(data))
		, error(nullptr)
	{}
	Maybe(Error&& error)
		: data(nullptr)
		, error(make_shared<Error>(error))
	{}

	bool isEmpty() { return (data == nullptr); };
	bool hasError() { return (error != nullptr); };
	T operator()() { return data; };
	shared_ptr<Error> getError() { return error; };
};

template <class T>
Maybe<T> just(T t)
{
	return Maybe<T>(t);
}

// Helpers to convert lambda into std::function

template <typename Function>
struct function_traits : public function_traits<decltype(&Function::operator())>{};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
{
	typedef ReturnType(*pointer)(Args...);
	typedef function<ReturnType(Args...)> function;
};

template <typename Function>
typename function_traits<Function>::function
to_function(Function& lambda)
{
	return static_cast<typename function_traits<Function>::function>(lambda);
}


// Aspect logging duration of execution
template <typename ReturnType, typename ...Args>
function<ReturnType(Args...)> logged(string name, function<ReturnType(Args...)> func)
{
	return [func, name](Args... args) {
		LOG << "_____________________________________________" << NL;
		LOG << name << " start" << NL;
		auto start = chrono::high_resolution_clock::now();
		ReturnType result = func(forward<Args>(args)...);
		auto end = chrono::high_resolution_clock::now();
		auto total = chrono::duration_cast<chrono::microseconds>(end - start).count();
		LOG << "Elapsed: " << total << "us" << NL;
		return result;
	};
}

// Security checking
template <typename ReturnType, typename ...Args, typename S>
function<Maybe<ReturnType>(Args...)> secured(S session, function<Maybe<ReturnType>(Args...)> func)
{
	// if user is not valid - return nothing
	return [func, &session](Args... args) -> Maybe<ReturnType>
	{
		return (session.isValid()) ? func(forward<Args>(args)...) : Error(403, "Forbidden");
	};
}

// Use local cache (memorize)
template <typename ReturnType, typename Cache, typename ...Args>
function<Maybe<ReturnType>(Args...)> cached(Cache& cache, function<Maybe<ReturnType>(Args...)> func)
{
	return [func, &cache](Args... args) {
		// get key as tuple of arguments
		auto key = make_tuple(args...);
		if (cache.count(key) > 0)
			return just(cache[key]);
		else
		{
			Maybe<ReturnType> result = func(forward<Args>(args)...);
			if (!result.hasError())
				cache.insert(pair<decltype(key), ReturnType>(key, result())); //add to cache
			return result;
		}
	};
}

// If there was error - try again
template <typename ReturnType, typename ...Args>
function<Maybe<ReturnType>(Args...)> triesTwice(function<Maybe<ReturnType>(Args...)> func)
{
	return [func](Args... args) {
		Maybe<ReturnType> result = func(forward<Args>(args)...);
		if (result.hasError())
			return func(forward<Args>(args)...);
		return result;
	};
}

// Treat empty state as error
template <typename ReturnType, typename ...Args>
function<Maybe<ReturnType>(Args...)> notEmpty(function<Maybe<ReturnType>(Args...)> func)
{
	return [func](Args... args) -> Maybe<ReturnType>
	{
		Maybe<ReturnType> result = func(forward<Args>(args)...);
		if ((!result.hasError()) && (result.isEmpty()))
			return Error(404, "Not Found");
		return result;
	};
}

template <typename ReturnType, typename ...Args>
function<ReturnType(Args...)> locked(mutex & mtx, function<ReturnType(Args...)> func)
{
	return [func, &mtx](Args... args) {
		unique_lock<mutex> lock(mtx);
		return func(forward<Args>(args)...);
	};
}

template <class T, class... P>
inline auto make(P&& ... args) -> T {
	return make_shared<typename T::element_type>(forward<P>(args)...);
}

template<typename ID,typename Name,typename Parent>
auto oneUser(ID id, Name name, Parent parent)
{
	return make<User>(id, name, parent);
}

using Users = vector<User>;
inline auto insertUsers(Users& users)
{
	users.emplace_back(oneUser<uint16_t,string,uint16_t>(1, "John", 0));
	users.emplace_back(oneUser<uint16_t,string,uint16_t>(2, "Bob", 1));
	users.emplace_back(oneUser<uint16_t,string,uint16_t>(3, "Max", 1));
}

class Session
{
public:
	bool isValid() { return true; }
} session;

int mainAOP() {
	Users users;
	mutex lockMutex;
	map<tuple<int>, User> userCache;

	insertUsers(users);

	auto findUser = [&users](uint16_t id) -> Maybe<User>
	{
		for (User user : users) {
			if (user->id == id)
				return user;
		}
		return nullptr;
	};


	// Main functional factorization
	auto findUserFinal = locked(lockMutex, secured(session, notEmpty(cached(userCache, triesTwice(logged("findUser", to_function(findUser)))))));

	// TEST:
	auto testUser = [&](uint16_t id) {
		auto user = findUserFinal(id);
		LOG << (user.hasError() ? "ERROR: " + user.getError()->message : "NAME:" + user()->name) << NL;
	};

	testUser(2);
	testUser(30);
	testUser(2);
	testUser(1);

	return getchar();
}