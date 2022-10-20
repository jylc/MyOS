#include "pci.h"
#include "print.h"
#include "net/amd_am79c973.h"
#include "memorymanagement.h"
namespace myos {
	namespace kernel {
		PeripheralComponentInterconnectDriverDescriptor::PeripheralComponentInterconnectDriverDescriptor() {}
		PeripheralComponentInterconnectDriverDescriptor::~PeripheralComponentInterconnectDriverDescriptor() {}


		PeripheralComponentInterconnectController::PeripheralComponentInterconnectController() :
			commandport(0xcf8),// PCI_CONFIG_ADDRESS
			dataport(0xcfc) //PCI_CONFIG_DATA        
		{

		}

		PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController() {}

		uint32_t PeripheralComponentInterconnectController::Read(uint8_t bus, uint8_t device, uint8_t function, uint8_t registeroffset) {
			// id CONFIG_ADDRESS 32-bit format 
			uint32_t id = (1 << 31) |
				((bus & 0xff) << 16) |
				((device & 0x1f) << 11) |
				((function & 0x07) << 8) |
				(registeroffset & 0xfc);
			commandport.write(id);
			uint32_t data = dataport.read();
			return data >> (8 * (registeroffset % 4));
		}

		void PeripheralComponentInterconnectController::write(uint8_t bus, uint8_t device, uint8_t function, uint8_t registeroffset, uint32_t value) {
			uint32_t id = (1 << 31) |
				((bus & 0xff) << 16) |
				((device & 0x1f) << 11) |
				((function & 0x07) << 8) |
				(registeroffset & 0xfc);
			commandport.write(id);
			dataport.write(value);
		}

		bool PeripheralComponentInterconnectController::DeviceHasFunctions(uint8_t bus, uint8_t devices) {
			return Read(bus, devices, 0, 0x0e) & (1 << 7);
		}

		void PeripheralComponentInterconnectController::SelectDrivers(DriverManager* driverManager, InterruptManager* interruptManager) {
			// 256个总线
			for (uint16_t bus = 0; bus < 256; bus++) {
				// 每个总线有32个设备
				for (uint8_t device = 0; device < 32; device++) {
					// 一般有8个方法
					int numFunctions = DeviceHasFunctions((uint8_t)bus, device) ? 8 : 1;
					for (uint8_t function = 0; function < numFunctions; function++) {
						// bus device function 获取配置空间
						PeripheralComponentInterconnectDriverDescriptor dev = GetDeviceDescriptor((uint8_t)bus, device, function);
						if (dev.vendor_id == 0 || dev.vendor_id == 0xffff) continue;

						/*tools::printf("PCI BUS:");
						tools::printf("%lX", bus & 0xff);

						tools::printf(", DEVICE :");
						tools::printf("%lX", device);

						tools::printf(", FUNCTION :");
						tools::printf("%lX", function);

						tools::printf(" = VENDOR :");
						tools::printf("%X", dev.vendor_id);

						tools::printf(", DEVICE :");
						tools::printf("%X", dev.device_id);
						tools::printf("\n");*/

						for (uint8_t barNum = 0; barNum < 6; barNum++) {
							// 从配置空间获取基址寄存器
							BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);
							if (bar.address && (bar.type == InputOutput)) {// I/O设备
								dev.portBase=(uint32_t)bar.address;// portBase是4字节的地址，指向寄存器
							}
						}

						Driver* driver = GetDriver(dev, interruptManager);
						if (driver != nullptr) {
							driverManager->AddDriver(driver);
						}
					}
				}
			} 
		}
		Driver* PeripheralComponentInterconnectController::GetDriver(PeripheralComponentInterconnectDriverDescriptor dev, InterruptManager* manager){
			Driver* driver = nullptr;
			switch (dev.vendor_id)
			{
			case 0x1022:// AMD
				switch (dev.device_id)
				{
				case 0x2000:// am79c973
					driver = (net::AmdAm78c973*)MemoryManager::activeMemoryManager->malloc(sizeof(net::AmdAm78c973));
					if (driver != nullptr) {
						// 指定在driver所在的位置分配内存
						new (driver)net::AmdAm78c973(&dev, manager);
						tools::printf("AMD am79c973 instantiation successful\n");
					}
					else {
						tools::printf("AMD am79c973 instantiation failed\n");
					}
					tools::printf("AMD AM79c973!\n");
					return driver;
					break;
				}
				break;
			case 0x8086:// INTEL
				tools::printf("INTEL!\n");
				break;
			case 0x1af4:
				tools::printf("VirtIO\n");
				break;
			default:
				//tools::printf("UNKNOWN\n");
				break;
			}
			
			switch (dev.class_id)
			{
			case 0x03:
				switch (dev.subclass_id) {
				case 0x00:// VGA
					tools::printf("VGA\n");
					break;
				}
				break;
			default:
				break;
			}
			return nullptr;
		}



		BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister(uint8_t bus, uint8_t device, uint8_t function, uint8_t bar) {
			BaseAddressRegister result;

			// 读取PCI配置空间中Header Type部分
			uint32_t headertype = Read(bus, device, function, 0x0e) & 0x7e;
			int maxbars = 6 - 4 * headertype;
			if (bar >= maxbars)return result;

			// 读取PCI配置空间中基址寄存器部分
			uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);
			result.type = (bar_value & 1) ? InputOutput : MemoryMapping;

			if (result.type == MemoryMapping) {
				switch ((bar_value>>1)&0x3)
				{
				case 0:// 32
				case 1:// 20
				case 2:// 64
					break;
				}
			}
			else {
				result.address = (uint8_t*)(bar_value & ~3);
				result.prefetchable = false;
			}
			return result;
		}

		PeripheralComponentInterconnectDriverDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint8_t bus, uint8_t device, uint8_t function) {
			PeripheralComponentInterconnectDriverDescriptor result;

			result.bus = bus;
			result.device = device;
			result.function = function;

			result.vendor_id = Read(bus, device, function, 0);
			result.device_id = Read(bus, device, function, 0x02);

			result.class_id = Read(bus, device, function, 0x0b);
			result.subclass_id = Read(bus, device, function, 0x0a);
			result.interface_id = Read(bus, device, function, 0x09);
			result.revision = Read(bus, device, function, 0x08);
		
			result.interrupt = Read(bus, device, function, 0x3c);
			return result;
		}
	}
}