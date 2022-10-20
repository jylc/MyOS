#ifndef MYOS_MEMORYMANAGEMENT_H
#define MYOS_MEMORYMANAGEMENT_H
#include "types.h"


namespace myos {
	namespace kernel {
		struct MemoryChunk {
			MemoryChunk* next;
			MemoryChunk* prev;
			bool allocated;
			size_t size;
		};

		class MemoryManager {
		public:
			MemoryManager(size_t start,size_t size);
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
