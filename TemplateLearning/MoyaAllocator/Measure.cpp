#include <iostream>
#include <chrono>
#include <random>

#include <set>
#include <map>
#include <list>
#include <forward_list>

#include "Allocator.h"


const std::size_t growSize = 1024;
const int numberOfIterations = 1024;
const int randomRange = 1024;

template <typename Container>
class PerformanceTest
{
    virtual void testIteration(int newSize) = 0;
	
    protected:
    ~PerformanceTest() = default;
    Container container;
		std::default_random_engine randomNumberGenerator;
        std::uniform_int_distribution<int> randomNumberDistribution;
		
    public:
		PerformanceTest() 
			: randomNumberGenerator(0)
            , randomNumberDistribution(0, randomRange)
        {}

        double run()
        {
            auto from = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < numberOfIterations; i++)
                testIteration(randomNumberDistribution(randomNumberGenerator));

            auto to = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::duration<double>>(to - from).count();
        }
};

template <typename Container>
class PushFrontTest : public PerformanceTest<Container>
{
    virtual void testIteration(int newSize)
    {
        int size = 0;

        while (size < newSize)
            this->container.push_front(size++);

        for (; size > newSize; size--)
            this->container.pop_front();
    }
};

template <typename Container>
class PushBackTest : public PerformanceTest<Container>
{
    virtual void testIteration(int newSize)
    {
        int size = 0;

        while (size < newSize)
            this->container.push_back(size++);

        for (; size > newSize; size--)
            this->container.pop_back();
    }
};

template <typename Container>
class MapTest : public PerformanceTest<Container>
{
    virtual void testIteration(int newSize)
    {
        int size = 0;

        while (size < newSize)
            this->container.insert(std::pair<char, int>(size++, size));

        while (size > newSize)
            this->container.erase(--size);
    }
};

template <typename Container>
class VectorTest final : public PerformanceTest<Container>
{
	void testIteration(const int newSize) override
	{
		int size = 0;

		for (; size < newSize; ++size)
			this->container.emplace_back(size);

		size = 0;
		for (; size < newSize; ++size)
			this->container.erase(this->container.begin() + size);
	}
};

template <typename Container>
class SetTest : public PerformanceTest<Container>
{
    virtual void testIteration(int newSize)
    {
        int size = 0;

        while (size < newSize)
            this->container.insert(size++);

        while (size > newSize)
            this->container.erase(--size);
    }
};

template <typename StlContainer, typename FastContainer>
void printTestStatus(const char *name, StlContainer &stlContainer, FastContainer &fastContainer)
{
    std::cout << std::fixed;
    std::cout << name << " - Default STL Allocator : " << stlContainer.run() << " seconds." << std::endl;
    std::cout << name << " - Memory Pool Allocator : " << fastContainer.run() << " seconds." << std::endl;
    std::cout << std::endl;
}

int mainMes()
{
	using DataType = int;
	using MemoryPoolAllocator = Moya::Allocator<DataType, growSize>;

    std::cout << "Allocator performance measurement example" << std::endl;
    std::cout << "Version: 1.0" << std::endl << std::endl;

    PushFrontTest<std::forward_list<DataType>> pushFrontForwardListTestStl;
    PushFrontTest<std::forward_list<DataType, MemoryPoolAllocator>> pushFrontForwardListTestFast;
    printTestStatus("ForwardList PushFront", pushFrontForwardListTestStl, pushFrontForwardListTestFast);

    PushFrontTest<std::list<DataType>> pushFrontListTestStl;
    PushFrontTest<std::list<DataType, MemoryPoolAllocator>> pushFrontListTestFast;
    printTestStatus("List PushFront", pushFrontListTestStl, pushFrontListTestFast);

	//VectorTest<std::vector<DataType>> vectorTestStl;
	//VectorTest<std::vector<DataType, MemoryPoolAllocator>> vectorTestFast;
	//printTestStatus("Vector PushFront", vectorTestStl, vectorTestFast);

    PushBackTest<std::list<DataType>> pushBackListTestStl;
    PushBackTest<std::list<DataType, MemoryPoolAllocator>> pushBackListTestFast;
    printTestStatus("List PushBack", pushBackListTestStl, pushBackListTestFast);

	MapTest<std::map<DataType, DataType, std::less<>>> mapTestStl;
	MapTest<std::map<DataType, DataType, std::less<>, MemoryPoolAllocator>> mapTestFast;
	printTestStatus("Map", mapTestStl, mapTestFast);

    SetTest<std::set<DataType, std::less<>>> setTestStl;
    SetTest<std::set<DataType, std::less<>, MemoryPoolAllocator>> setTestFast;
    printTestStatus("Set", setTestStl, setTestFast);

    return getchar();
}
