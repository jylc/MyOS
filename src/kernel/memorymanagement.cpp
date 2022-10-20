#include "memorymanagement.h"

namespace myos {
	namespace kernel {
		MemoryManager* MemoryManager::activeMemoryManager = 0;
		MemoryManager::MemoryManager(size_t start, size_t size) {
			activeMemoryManager = this;

			if (size < sizeof(MemoryChunk)) {
				first = 0;
			}
			else
			{
				// 为什么用start做为地址
				first = (MemoryChunk*)start;
				first->allocated = false;
				first->prev = 0;
				first->next = 0;
				// 不包含头部大小（sizeof(MemoryChunk)）
				first->size = size - sizeof(MemoryChunk);
			}
		}

		MemoryManager::~MemoryManager() {
			if (activeMemoryManager == this) {
				activeMemoryManager = 0;
			}
		}

		// malloc 分配内存
		void* MemoryManager::malloc(size_t size) {
			MemoryChunk* result = 0;
			for (MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk->next) {
				// chunk的大小大于size且chunk没有被分配，就选取该chunk
				if (chunk->size > size && !chunk->allocated) {
					result = chunk;
				}
			}

			// 分配失败
			if (result == 0)return 0;
			// 分配过大重新分配
			if (result->size >= size + sizeof(MemoryChunk) + 1) {
				// TODO 为什么会这么做
				MemoryChunk* tmp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
				tmp->allocated = false;
				tmp->size = result->size - size - sizeof(MemoryChunk);
				tmp->prev = result;
				tmp->next = result->next;
				if (tmp->next != 0) {
					tmp->next->prev = tmp;
				}
				result->next = tmp;
				result->size = size;
			}

			result->allocated = true;
			return (void*)((size_t)result + sizeof(MemoryChunk));
		}

		void MemoryManager::free(void* ptr) {
			MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryManager));

			chunk->allocated = false;
			if (chunk->prev != 0 && !chunk->prev->allocated) {
				chunk->prev->next = chunk->next;
				chunk->prev->size += chunk->size + sizeof(MemoryChunk);
				if (chunk->next != 0) {
					chunk->next->prev = chunk->prev;
				}
				chunk = chunk->prev;
			}
			if (chunk->next != 0 && !chunk->next->allocated) {
				chunk->size += chunk->next->size + sizeof(MemoryChunk);
				chunk->next = chunk->next->next;
				if (chunk->next != 0) {
					chunk->next->prev = chunk;
				}
			}
		}



	}
}


void* operator new(myos::size_t size) {
	if (myos::kernel::MemoryManager::activeMemoryManager == 0)return nullptr;
	return myos::kernel::MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new[](myos::size_t size) {
	if (myos::kernel::MemoryManager::activeMemoryManager == 0)return nullptr;
	return myos::kernel::MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new(myos::size_t size, void* ptr) {
	return ptr;
}

void* operator new[](myos::size_t size, void* ptr) {
	return ptr;
}

void operator delete(void* ptr) {
	if (myos::kernel::MemoryManager::activeMemoryManager != 0) {
		myos::kernel::MemoryManager::activeMemoryManager->free(ptr);
	}
}

void operator delete[](void* ptr) {
	if (myos::kernel::MemoryManager::activeMemoryManager != 0) {
		myos::kernel::MemoryManager::activeMemoryManager->free(ptr);
	}
}
