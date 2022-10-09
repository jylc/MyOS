#include "kernel.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"
#include "types.h"
#include "driver.h"
#include "print.h"
using namespace myos;
using namespace myos::tools;
using namespace myos::kernel;

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

		printf("pos:(%d,%d),move:(%d,%d)\n", x, y,nx,ny);
		VideoMemory[y * 80 + x] = ((VideoMemory[y * 80 + x] & 0xf000) >> 4) |
			((VideoMemory[y * 80 + x] & 0x0f00) << 4) |
			((VideoMemory[y * 80 + x] & 0x00ff));
	};
};


extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/) {
	printf("Hello World!\n");
	printf("As we can!\n");
	GlobalDescriptorTable gdt;
	InterruptManager interrupt(0x20, &gdt);
	PrintKeyboardEventHandler keyboardHandler;
	KeyboardDriver keyboardDriver(&interrupt, &keyboardHandler);

	MouseToConsole mouseHandler;
	MouseDriver mouseDriver(&interrupt, &mouseHandler);
	DriverManager driverManager;
	driverManager.AddDriver(&keyboardDriver);
	driverManager.AddDriver(&mouseDriver);
	driverManager.DriverAll();

	interrupt.Activate();
	while (1);
}