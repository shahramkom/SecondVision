#ifndef AllocatorH
#define AllocatorH

#include <memory>


namespace Moya {

template <class T, std::size_t growSize = 1024>
class MemoryPool
{
    struct Block
    {
        Block *next;
    };

    class Buffer
    {
        static const std::size_t blockSize = sizeof(T) > sizeof(Block) ? sizeof(T) : sizeof(Block);
        uint8_t data[blockSize * growSize];

        public:

            Buffer *const next;

			explicit Buffer(Buffer *next)
				: data{}
    			, next(next)
			{
			}

            T* getBlock(const std::size_t index)
            {
                return reinterpret_cast<T*>(&data[blockSize * index]);
            }
    };

    Block* firstFreeBlock = nullptr;
    Buffer* firstBuffer = nullptr;
    std::size_t bufferedBlocks = growSize;


    public:
		MemoryPool() = default;
        MemoryPool(MemoryPool &&memoryPool) = delete;
        MemoryPool(const MemoryPool &memoryPool) = delete;
        MemoryPool operator =(MemoryPool &&memoryPool) = delete;
        MemoryPool operator =(const MemoryPool &memoryPool) = delete;

        ~MemoryPool()
        {
            while (firstBuffer)
			{
                Buffer *buffer = firstBuffer;
                firstBuffer = buffer->next;
                delete buffer;
            }
        }

        T* allocate()
        {
            if (firstFreeBlock) {
                Block* block = firstFreeBlock;
                firstFreeBlock = block->next;
                return reinterpret_cast<T*>(block);
            }

            if (bufferedBlocks >= growSize) {
                firstBuffer = new Buffer(firstBuffer);
                bufferedBlocks = 0;
            }

            return firstBuffer->getBlock(bufferedBlocks++);
        }

        void deallocate(T* pointer)
        {
	        auto block = reinterpret_cast<Block*>(pointer);
            block->next = firstFreeBlock;
            firstFreeBlock = block;
        }
};

template <class T, std::size_t growSize = 1024>
class Allocator : private MemoryPool<T, growSize>
{
#ifdef _WIN32
    Allocator* copyAllocator;
    std::allocator<T>* rebindAllocator = nullptr;
#endif

    public:
		typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;

        template <class U>
        struct rebind
        {
            typedef Allocator<U, growSize> other;
        };

#ifdef _WIN32
        Allocator() = default;

        Allocator(Allocator &allocator)
			: copyAllocator(&allocator)
        {
        }

        template <class U>
        explicit Allocator(const Allocator<U, growSize>& other)
			: copyAllocator(nullptr)
		{
			if (!std::is_same<T, U>::value)
				rebindAllocator = new std::allocator<T>();
		}

		~Allocator()
        {
            delete rebindAllocator;
        }
#endif

        pointer allocate(size_type N, const void* hint = nullptr)
        {
#ifdef _WIN32
            if (copyAllocator)
                return copyAllocator->allocate(N, hint);

            if (rebindAllocator)
                return rebindAllocator->allocate(N, hint);
#endif
            if (N != 1 || hint)
                throw std::bad_alloc();

            return MemoryPool<T, growSize>::allocate();
        }

        void deallocate(pointer p, size_type N)
        {
#ifdef _WIN32
            if (copyAllocator)
			{
                copyAllocator->deallocate(p, N);
                return;
            }

            if (rebindAllocator) 
			{
                rebindAllocator->deallocate(p, N);
                return;
            }
#endif
            MemoryPool<T, growSize>::deallocate(p);
        }

		static void construct(const pointer p, const_reference val)
        {
            new (p) T(val);
        }

		static void destroy(pointer p)
        {
            p->~T();
        }
};

}

#endif
