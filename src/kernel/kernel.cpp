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
#include "net/udp.h"
#include "net/tcp.h"
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

class PrintUDPHandler :public net::UserDatagramProtocolHandler {
public:
	void HandleUserDatagramProtocolMessage(net::UserDatagramProtocolSocket* socket, uint8_t* data, uint32_t size)override {
		char* foo = "";
		printf("[%s-%d] ",__FUNCTION__,__LINE__);
		for (int i = 0; i < size; i++) {
			foo[0] = data[i];
			printf("%c", foo[0]);
		}
	}
};

class PrintTCPHandler :public net::TransmissionControlProtocolHandler {
public:
	bool HandleTransmissionControlProtocolMessage(net::TransmissionControlProtocolSocket* socket, uint8_t* data, uint32_t size)override {
		char foo[1] = {""};
		printf("[%s-%d] size=%x\n", __FUNCTION__, __LINE__,size);
		for (int i = 0; i < size; i++) {
			foo[0] = data[i];
			printf("%c",foo[0]);
		}
		foo[1] = ' ';
		printf("\n");
		return true;
	}
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

	printf("\nheap: %x\n", heap);

	void* allocated = memoryManager.malloc(1024);
	printf("\nallocated: %x\n", allocated);


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


	uint8_t ip1 = 10, ip2 = 0, ip3 = 2, ip4 = 15;
	uint32_t ip_be = ((uint32_t)ip4 << 24)
		| ((uint32_t)ip3 << 16)
		| ((uint32_t)ip2 << 8)
		| (uint32_t)ip1;

	uint8_t gip1 = 10, gip2 = 0, gip3 = 2, gip4 = 2;
	uint32_t gip_be = ((uint32_t)gip4 << 24)
		| ((uint32_t)gip3 << 16)
		| ((uint32_t)gip2 << 8)
		| (uint32_t)gip1;

	uint8_t subnet1 = 255, subnet2 = 255, subnet3 = 255, subnet4 = 0;
	uint32_t subnet_be = ((uint32_t)subnet4 << 24) |
		((uint32_t)subnet3 << 16) |
		((uint32_t)subnet2 << 8) |
		(uint32_t)subnet1;

	uint32_t tmp_ip_be = ip_be;
	uint32_t tmp_gip_be = gip_be;
	uint32_t tmp_subnet_be = subnet_be;
	net::AmdAm78c973* eth0 = (net::AmdAm78c973*)(driverManager.drivers[2]);

	eth0->SetIPAddress(ip_be);


	net::EtherFrameProvider etherframe(eth0);
	net::AddressResolutionProtocol arp(&etherframe);

	// IPV4 设置以太网（etherframe），网关地址（tmp_gip_be），子网掩码（tmp_subnet_be）
	net::InternetProtocolProvider ipv4(&etherframe, &arp, tmp_gip_be, tmp_subnet_be);
	net::InternetControlMessageProtocol icmp(&ipv4);
	net::UserDatagramProtocolProvider udp(&ipv4);
	net::TransmissionControlProtocolProvider tcp(&ipv4);

	interrupt.Activate();

	arp.BroadcastMACAddress(tmp_gip_be);
	/*icmp.RequestEchoReply(tmp_gip_be);*/

	/*PrintUDPHandler udphandler;
	net::UserDatagramProtocolSocket* socket = udp.Listen(1234);
	udp.Bind(socket, &udphandler);*/
	//tcp.Connect(tmp_gip_be, 1234);
	PrintTCPHandler tcphandler;
	net::TransmissionControlProtocolSocket* tcpsocket = tcp.Connect(tmp_gip_be, 1234);
	tcp.Bind(tcpsocket, &tcphandler);
	tcpsocket->Send((uint8_t*)"hello world!", 12);

	while (1) {
#ifdef GRAPHICMODE
		desktop.Draw(&vga);
#endif // GRAPHICMODE
	}
}