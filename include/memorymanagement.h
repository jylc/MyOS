#ifndef MYOS_MEMORYMANAGEMENT_H
#define MYOS_MEMORYMANAGEMENT_H
#include "types.h"


namespace myos {
	namespace kernel {

		// sizeof(MemoryChunk)==16B，在内存中结构体会字节对齐bool占4B
		struct MemoryChunk {
			// 32bit os 指针4Bytes
			MemoryChunk* next; //4B
			MemoryChunk* prev; //4B
			bool allocated;    //1B
			size_t size;	   //4B
		};

		class MemoryManager {
		public:
			MemoryManager(size_t start, size_t size);
			~MemoryManager();

			void* malloc(size_t size);
			void free(void* ptr);

			static MemoryManager* activeMemoryManager;
		private:
			MemoryChunk* first;

		};
	}
}
void* operator new(myos::size_t size);
void* operator new[](myos::size_t size);

void* operator new(myos::size_t size, void* ptr);
void* operator new[](myos::size_t size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);


#endif // !MYOS_MEMORYMANAGEMENT_H
