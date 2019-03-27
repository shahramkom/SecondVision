#include "includes.h"
#include <scoped_allocator>

template <class Tp>
struct SimpleAllocator 
{
	typedef Tp value_type;
	SimpleAllocator(/*ctor args*/);
	template <class T> 
	SimpleAllocator(const SimpleAllocator<T>& other);
	Tp* allocate(std::size_t n);
	void deallocate(Tp* p, std::size_t n);
};

template <class T, class U>
bool operator==(const SimpleAllocator<T>&, const SimpleAllocator<U>&);
template <class T, class U>
bool operator!=(const SimpleAllocator<T>&, const SimpleAllocator<U>&);

class MyTest
{
public:
	MyTest();

	explicit MyTest(const int a)
		:aa(a)
	{}

	explicit MyTest(const string b)
		:bb(b)
	{}

	~MyTest();

private:
	int aa;
	string bb;
};

MyTest::MyTest()
{
	cout << "construct" << endl;
}

MyTest::~MyTest()
{
	cout << "destruct" << endl;
}



/*
template<typename T>
struct My_alloc { // use an Arena to allocate and deallocate bytes
	string& a;
	My_alloc(string& aa) 
		: a(aa) 
	{ }
	My_alloc()
		: a()
	{
	}

	// usual allocator stuff
};

using svec0 = vector<string>;
svec0 v0;

// vector (only) uses My_alloc and string uses its own allocator (the default):
using Svec1 = vector<string, My_alloc<string>>;
Svec1 v1{ My_alloc<string>{} };

// vector and string use My_alloc (as above):
using Xstring = basic_string<char, char_traits<char>, My_alloc<char>>;
using Svec2 = vector<Xstring, scoped_allocator_adaptor<My_alloc<Xstring>>>;
Svec2 v2{ scoped_allocator_adaptor<My_alloc<Xstring>>{} };

// vector uses its own allocator (the default) and string uses My_alloc:
using Xstring2 = basic_string<char, char_traits<char>, My_alloc<char>>;
using Svec3 = vector<Xstring2, scoped_allocator_adaptor<My_alloc<Xstring>, My_alloc<char>>>;
Svec3 v3{ scoped_allocator_adaptor<My_alloc<Xstring2>,My_alloc<char>>{} };
*/




template <class T>
struct Mallocator {
	typedef T value_type;
	Mallocator() = default;
	template <class U> 
	constexpr Mallocator(const Mallocator<U>&) noexcept 
	{}
	[[nodiscard]] T* allocate(std::size_t n) 
	{
		if (n > std::size_t(-1) / sizeof(T)) 
			throw std::bad_alloc();
		if (auto p = static_cast<T*>(std::malloc(n * sizeof(T)))) 
			return p;
		throw std::bad_alloc();
	}
	void deallocate(T* p, std::size_t) noexcept
	{
		std::free(p);
	}
};

template <class T, class U>
bool operator==(const Mallocator<T>&, const Mallocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const Mallocator<T>&, const Mallocator<U>&) { return false; }


namespace mmap_allocator_namespace
{
	// See StackOverflow replies to this answer for important commentary about inheriting from std::allocator before replicating this code.
	template <typename T>
	class mmap_allocator : public std::allocator<T>
	{
	public:
		template<typename _Tp1>
		struct rebind
		{
			typedef mmap_allocator<_Tp1> other;
		};

		T* allocate(size_t N, const void* hint = nullptr)
		{
			fprintf(stderr, "Alloc %d bytes.\n", N * sizeof(T));
			return std::allocator<T>::allocate(N, hint);
		}

		void deallocate(T* pointer, size_t N)
		{
			fprintf(stderr, "Dealloc %d bytes (%p).\n", N * sizeof(T), pointer);
			return std::allocator<T>::deallocate(pointer, N);
		}

		mmap_allocator() 
			throw() : std::allocator<T>()
		{
			fprintf(stderr, "Hello allocator!\n");
		}
		
		mmap_allocator(const mmap_allocator& a) 
			throw() : std::allocator<T>(a)
		{ }
		
		template <class U>
		mmap_allocator(const mmap_allocator<U>& a) 
			throw() : std::allocator<T>(a)
		{ }
		~mmap_allocator() 
			throw()
		{ }
	};
}

int mainAlloc()
{
	
#pragma region DEFAULT_SAMPLE
	std::allocator<int> a1; // default allocator for ints
	int* a = a1.allocate(10); // space for 10 ints
	a[9] = 7;
	std::cout << a[9] << '\n';

	a1.destroy(a);
	a1.deallocate(a, 10);
	a[8] = 5;
	std::cout << a[8] << '\n';

	// default allocator for strings
	std::allocator<std::string> a2;
	// same, but obtained by rebinding from the type of a1
	decltype(a1)::rebind<std::string>::other a2_1;

	// same, but obtained by rebinding from the type of a1 via allocator_traits
	std::allocator_traits<decltype(a1)>::rebind_alloc<std::string> a2_2;

	std::string* s = a2.allocate(2); // space for 2 string

	a2.construct(s, "foo");
	a2.construct(s + 1, "bar");

	std::cout << s[0] << ' ' << s[1] << '\n';

	a2.destroy(s);
	a2.destroy(s + 1);
	a2.deallocate(s, 2);
#pragma endregion DEFAULT_SAMPLE
	/*
	// allocator for integer values 
	std::allocator<MyTest> myAllocator;

	// allocate space for three strings 
	auto myAllocer = myAllocator.allocate(3);

	// construct these 3 strings 
	myAllocator.construct(myAllocer, MyTest());
	myAllocator.construct(myAllocer + 1, MyTest(1));
	myAllocator.construct(myAllocer + 2, MyTest("SABZ"));

	// destroy these 3 strings 
	myAllocator.destroy(myAllocer);
	myAllocator.destroy(myAllocer + 1);
	myAllocator.destroy(myAllocer + 2);

	// deallocate space for 3 strings 
	myAllocator.deallocate(myAllocer, 3);
	*/

	//std::vector<char, std::allocator<char>> vectChar;

	int sz_ = 1024 * 1024;
	vector<int, mmap_allocator_namespace::mmap_allocator<int> > int_vec(sz_, 0, mmap_allocator_namespace::mmap_allocator<int>());
	vector<int, Mallocator<int>> m_vec(sz_, 0, Mallocator<int>());
	vector<int> def_vec;


	for (int a = 0; a < sz_; a++)
		def_vec.emplace_back(a);

	//for (int b = 0; b < 1024; b++)
	//	int_vec.emplace_back(b);

	//for (int c = 0; c < 1024; c++)
	//	m_vec.emplace_back(c);

	return getchar();
}

template< typename T, typename A >
void safe_swap(std::vector<T, A>& a, std::vector<T, A>& b)
{

	if ((std::allocator_traits<A>::propagate_on_container_swap::value) ||
		(a.get_allocator() == b.get_allocator()))
	{
		// ok, swap the allocators if required, no copy or move of elements in the vectors
		a.swap(b);
		std::cout << "swapped using std::vector<>::swap\n";
	}

	else
	{
		const auto temp(std::move(a)); // EDIT: ???
		a = std::move(b);
		b = std::move(temp);
		std::cout << "swapped by moving the elements;\n\t\t"
			"(allocators aren't equal and they can't be swapped)\n";
	}
}

struct my_allocator : public std::allocator<int>
{
	using std::allocator<int>::allocator;

	static constexpr bool is_always_equal = false;
	static constexpr bool propagate_on_container_move_assignment = false;
	friend bool operator== (const my_allocator&, const my_allocator&) { return false; }
	friend bool operator!= (const my_allocator&, const my_allocator&) { return true; }
};

int main()
{
	{
		std::cout << "std::vector with the default allocator: ";
		std::vector<int> a(100), b(200);
		safe_swap(a, b);
	}

	{
		std::cout << "\nstd::vector with the my_allocator: ";
		std::vector< int, my_allocator > a(100), b(200);
		safe_swap(a, b);
	}
}