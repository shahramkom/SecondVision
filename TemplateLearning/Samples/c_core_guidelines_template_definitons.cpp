#include <cstddef>
#include <iostream>

template <typename T, std::size_t N>
class Array_1 {
public:
	Array_1() = default;
	std::size_t getSize() const {
		return N;
	}
private:
	T elem[N];
};

int main_1() {

	Array_1<int, 100> arr1;
	std::cout << "arr1.getSize(): " << arr1.getSize() << std::endl;

	Array_1<int, 200> arr2;
	std::cout << "arr2.getSize(): " << arr2.getSize() << std::endl;
	return 0;
}

//_______________________OR______________________________________________
template<typename T>
class ArrayBase {
protected:
	ArrayBase(std::size_t n) : size(n) {}
	std::size_t getSize() const {
		return size;
	};
private:
	std::size_t size;
};

template<typename T, std::size_t n>
class Array : private ArrayBase<T> {
public:
	Array() : ArrayBase<T>(n) {}
	std::size_t getSize() const {
		return  ArrayBase<T>::getSize();
	}
private:
	T data[n];
};


int main_2() {

	Array<int, 100> arr1;
	std::cout << "arr1.getSize(): " << arr1.getSize() << std::endl;

	Array<int, 200> arr2;
	std::cout << "arr2.getSize(): " << arr2.getSize() << std::endl;
	return 0;
}
//_____________________________________________________________________________
