#include "kernel.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"
#include "types.h"
#include "driver.h"
#include "print.h"
#include "pci.h"
#include "vga.h"
#include "gui/desktop.h"
#include "gui/window.h"
#include "memorymanagement.h"
#include "net/amd_am79c973.h"
#include "net/etherframe.h"
using namespace myos;
using namespace myos::tools;
using namespace myos::kernel;
using namespace myos::gui;

typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors() {
	for (constructor* i = &start_ctors; i != &end_ctors; i++)
		(*i)();
}

class PrintKeyboardEventHandler :public KeybordEventHandler {
public:
	void OnKeyDown(const char* c) {
		printf(c);
	}
	void OnKeyUp(const char*) {}
};

class MouseToConsole :public MouseEventHandler {
private:
	int8_t x, y;
public:
	MouseToConsole() :x(40), y(12) {}
	void OnActivate() {
		uint16_t* VideoMemory = (uint16_t*)0xb8000;
		VideoMemory[y * 80 + x] = ((VideoMemory[y * 80 + x] & 0xf000) >> 4) |
			((VideoMemory[y * 80 + x] & 0x0f00) << 4) |
			(VideoMemory[y * 80 + x] & 0x00ff);
	};
	void OnMouseDown(uint8_t) {};
	void OnMouseUp(uint8_t) {};
	void OnMouseMove(int8_t nx, int8_t ny) {
		uint16_t* VideoMemory = (uint16_t*)0xb8000;
		VideoMemory[y * 80 + x] = ((VideoMemory[y * 80 + x] & 0xf000) >> 4) |
			((VideoMemory[y * 80 + x] & 0x0f00) << 4) |
			((VideoMemory[y * 80 + x] & 0x00ff));

		x += nx;
		if (x < 0)x = 0;
		else if (x >= 80)x = 79;

		y += ny;
		if (y < 0)y = 0;
		else if (y >= 25)y = 24;

		// printf("pos:(%d,%d),move:(%d,%d)\n", x, y,nx,ny);
		VideoMemory[y * 80 + x] = ((VideoMemory[y * 80 + x] & 0xf000) >> 4) |
			((VideoMemory[y * 80 + x] & 0x0f00) << 4) |
			((VideoMemory[y * 80 + x] & 0x00ff));
	};
};

void taskA() {
	while (true) {
		printf("A");
	}
}

void taskB() {
	while (true) {
		printf("B");
	}
}



extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/) {
	printf("Hello World!\n");
	printf("As we can!\n");
	GlobalDescriptorTable gdt;
	// 一块空闲的内存地址
	size_t heap = 10 * 1024 * 1024;
	uint32_t* memupper = (uint32_t*)((size_t)multiboot_structure + 8);
	MemoryManager memoryManager(heap, (*memupper) * 1024 - heap - 10 * 1024);
	TaskManager taskManager;
	Task task1(&gdt, taskA);
	Task task2(&gdt, taskB);
	//taskManager.AddTask(&task1);
	//taskManager.AddTask(&task2);


	InterruptManager interrupt(0x20, &gdt, &taskManager);
#ifdef GRAPHICMODE
	Desktop desktop(320, 200, 0x00, 0x00, 0xa8);
#endif
	DriverManager driverManager;


#ifdef GRAPHICMODE
	KeyboardDriver keyboardDriver(&interrupt, &desktop);
#else
	PrintKeyboardEventHandler keyboardHandler;
	KeyboardDriver keyboardDriver(&interrupt, &keyboardHandler);
#endif
	driverManager.AddDriver(&keyboardDriver);

#ifdef GRAPHICMODE
	MouseDriver mouseDriver(&interrupt, &desktop);
#else
	MouseToConsole mouseHandler;
	MouseDriver mouseDriver(&interrupt, &mouseHandler);
#endif
	driverManager.AddDriver(&mouseDriver);

	PeripheralComponentInterconnectController PCIController;
	PCIController.SelectDrivers(&driverManager, &interrupt);
	VideoGraphicsArray vga;
	driverManager.DriverAll();

#ifdef GRAPHICMODE
	vga.SetMode(320, 200, 8);
	Window w1(&desktop, 10, 10, 20, 20, 0x00, 0xa8, 0x00);
	desktop.AddChild(&w1);
	Window w2(&desktop, 40, 15, 30, 30, 0xff, 0xff, 0xff);
	desktop.AddChild(&w2);
#endif // GRAPHICMODE

	net::AmdAm78c973* eth0 = (net::AmdAm78c973*)(driverManager.drivers[2]);
	net::EtherFrameProvider etherframe(eth0);
	etherframe.Send(0xffffffffffff, 0x608, (uint8_t*)"hello", 6);
	interrupt.Activate();
	while (1) {
#ifdef GRAPHICMODE
		desktop.Draw(&vga);
#endif // GRAPHICMODE
	}
}