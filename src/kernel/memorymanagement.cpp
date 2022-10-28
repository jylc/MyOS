#include "memorymanagement.h"

namespace myos {
	namespace kernel {
		MemoryManager* MemoryManager::activeMemoryManager = nullptr;
		
		// 该构造函数创建了一个初始的内存块，之后的内存块从该内存块中切割出去
		MemoryManager::MemoryManager(size_t start, size_t size) {
			activeMemoryManager = this;

			// 输入的大小小于MemoryChunk的大小，说明内存已经不足了
			if (size < sizeof(MemoryChunk)) {
				first = nullptr;
			}
			else
			{
				// start是接下来所有内存分配的起始地址
				first = (MemoryChunk*)start;
				first->allocated = false;
				first->prev = nullptr;
				first->next = nullptr;
				// 该内存块保存的大小不包含头部大小（sizeof(MemoryChunk)），size是包括了头部大小的整体尺寸
				first->size = size - sizeof(MemoryChunk);
			}
		}

		MemoryManager::~MemoryManager() {
			if (activeMemoryManager == this) {
				activeMemoryManager = nullptr;
			}
		}

		// malloc 分配内存
		void* MemoryManager::malloc(size_t size) {
			MemoryChunk* result = nullptr;
			for (MemoryChunk* chunk = first; chunk != nullptr && result == nullptr; chunk = chunk->next) {
				if (chunk->size > size && !chunk->allocated) {
					result = chunk;
				}
			}

			if (result == nullptr) return nullptr;

			if (result->size >= size + sizeof(MemoryChunk) + 1) {
				MemoryChunk* temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
				temp->allocated = false;
				temp->size = result->size - size - sizeof(MemoryChunk);
				temp->prev = result;
				temp->next = result->next;
				if (temp->next != nullptr) {
					temp->next->prev = temp;
				}
				result->next = temp;
				result->size = size;
			}

			result->allocated = true;
			return (void*)((size_t)result + sizeof(MemoryChunk));
		}

		void MemoryManager::free(void* ptr) {
			MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryManager));

			chunk->allocated = false;
			if (chunk->prev != nullptr && !chunk->prev->allocated) {
				chunk->prev->next = chunk->next;
				chunk->prev->size += chunk->size + sizeof(MemoryChunk);
				if (chunk->next != 0) {
					chunk->next->prev = chunk->prev;
				}
				chunk = chunk->prev;
			}
			if (chunk->next != nullptr && !chunk->next->allocated) {
				chunk->size += chunk->next->size + sizeof(MemoryChunk);
				chunk->next = chunk->next->next;
				if (chunk->next != nullptr) {
					chunk->next->prev = chunk;
				}
			}
		}

	}
}


void* operator new(myos::size_t size) {
	if (myos::kernel::MemoryManager::activeMemoryManager == nullptr)return nullptr;
	return myos::kernel::MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new[](myos::size_t size) {
	if (myos::kernel::MemoryManager::activeMemoryManager == nullptr)return nullptr;
	return myos::kernel::MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new(myos::size_t size, void* ptr) {
	return ptr;
}

void* operator new[](myos::size_t size, void* ptr) {
	return ptr;
}

void operator delete(void* ptr) {
	if (myos::kernel::MemoryManager::activeMemoryManager != nullptr) {
		myos::kernel::MemoryManager::activeMemoryManager->free(ptr);
	}
}

void operator delete[](void* ptr) {
	if (myos::kernel::MemoryManager::activeMemoryManager != nullptr) {
		myos::kernel::MemoryManager::activeMemoryManager->free(ptr);
	}
}
