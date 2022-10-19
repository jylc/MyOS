#ifndef MYOS_MULTITASKING_H
#include "types.h"
#include "gdt.h"
namespace myos {
	namespace kernel {
		struct CPUState {
			uint32_t eax, ebx, ecx, edx, esi, edi, ebp;

			uint32_t err, eip, cs, eflags, esp, ss;
		}__attribute__((packed));

		class Task {
			friend class TaskManager;
		public:
			Task(GlobalDescriptorTable* gdt, void entrypoint());
			~Task();

		private:
			uint8_t stack[4096];//ջ
			CPUState* cpustate;
		};

		class TaskManager {
		public:
			TaskManager();
			~TaskManager();
			bool AddTask(Task* task);
			CPUState* Schedule(CPUState* cpustate);

		private:
			Task* tasks[256];
			int numTasks;
			int currentTask;
		};
	}
}
#endif // !MYOS_MULTITASKING_H
