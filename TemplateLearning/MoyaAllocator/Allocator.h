#ifndef AllocatorH
#define AllocatorH

#include <memory>

constexpr int GROW_SIZE = 1024;

namespace Moya
{
	template <class T, std::size_t growSize = GROW_SIZE>
	class MemoryPool
	{
		struct Block
		{
			Block* next;
		};

		class Buffer
		{
			static const std::size_t blockSize = sizeof(T) > sizeof(Block) ? sizeof(T) : sizeof(Block);
			uint8_t data[blockSize * growSize];

		public:

			Buffer* const next;

			explicit Buffer(Buffer* next)
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
		MemoryPool(MemoryPool&& memoryPool) = delete;
		MemoryPool(const MemoryPool& memoryPool) = delete;
		MemoryPool operator =(MemoryPool&& memoryPool) = delete;
		MemoryPool operator =(const MemoryPool& memoryPool) = delete;

		~MemoryPool()
		{
			while (firstBuffer)
			{
				Buffer* buffer = firstBuffer;
				firstBuffer = buffer->next;
				delete buffer;
			}
		}

		T* allocate()
		{
			if (firstFreeBlock)
			{
				Block* block = firstFreeBlock;
				firstFreeBlock = block->next;
				return reinterpret_cast<T*>(block);
			}

			if (bufferedBlocks >= growSize)
			{
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

	template <class T, std::size_t growSize = GROW_SIZE>
	class Allocator : private MemoryPool<T, growSize>
	{
	public:
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using value_type = T;

		Allocator() = default;

		Allocator(Allocator& allocator)
			: copyAllocator_(&allocator)
		{}

		template <class U>
		explicit Allocator(const Allocator<U, growSize>& other)
			: copyAllocator_(nullptr)
		{
			if (!std::is_same<T, U>::value)
				rebindAllocator_ = new std::allocator<T>();
		}

		~Allocator()
		{
			delete rebindAllocator_;
		}

		template <class U>
		struct rebind
		{
			using other = Allocator<U, growSize>;
		};

		pointer allocate(size_type N, const void* hint = nullptr)
		{
			if (copyAllocator_)
				return copyAllocator_->allocate(N, hint);

			if (rebindAllocator_)
				return rebindAllocator_->allocate(N, hint);
			if (N != 1 || hint)
				throw std::bad_alloc();

			return MemoryPool<T, growSize>::allocate();
		}

		void deallocate(pointer p, size_type N)
		{
			if (copyAllocator_)
			{
				copyAllocator_->deallocate(p, N);
				return;
			}

			if (rebindAllocator_)
			{
				rebindAllocator_->deallocate(p, N);
				return;
			}
			MemoryPool<T, growSize>::deallocate(p);
		}

		static void construct(const pointer p, const_reference val)
		{
			new(p) T(val);
		}

		static void destroy(pointer p)
		{
			p->~T();
		}

	private:
		Allocator* copyAllocator_{};
		std::allocator<T>* rebindAllocator_ = nullptr;
	};
}

#endif
