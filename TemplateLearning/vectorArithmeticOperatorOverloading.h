#pragma once

#include <iostream>
#include <vector>

template<typename T>
class MyVectorA {
	std::vector<T> cont;

public:
	// MyVector with initial size
	MyVectorA(const std::size_t n) : cont(n) {}

	// MyVector with initial size and value
	MyVectorA(const std::size_t n, const double initialValue) : cont(n, initialValue) {}

	// size of underlying container
	std::size_t size() const {
		return cont.size();
	}

	// index operators
	T operator[](const std::size_t i) const {
		return cont[i];
	}

	T& operator[](const std::size_t i) {
		return cont[i];
	}

};

// function template for the + operator
template<typename T>
MyVectorA<T> operator+ (const MyVectorA<T>& a, const MyVectorA<T>& b) {
	MyVectorA<T> result(a.size());
	for (std::size_t s = 0; s <= a.size()-1; ++s) {
		result[s] = a[s] + b[s];
	}
	return result;
}

// function template for the * operator
template<typename T>
MyVectorA<T> operator* (const MyVectorA<T>& a, const MyVectorA<T>& b) {
	MyVectorA<T> result(a.size());
	for (std::size_t s = 0; s <= a.size()-1; ++s) {
		result[s] = a[s] * b[s];
	}
	return result;
}

// function template for << operator
template<typename T>
std::ostream& operator<<(std::ostream& os, const MyVectorA<T>& cont) {
	std::cout << std::endl;
	for (int i = 0; i < cont.size(); ++i) {
		os << cont[i] << ' ';
	}
	os << std::endl;
	return os;
}
