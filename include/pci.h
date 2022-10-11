#ifndef MYOS_PCI_H
#define MYOS_PCI_H
#include "types.h"
#include "port.h"
#include "driver.h"
#include "interrupts.h"

namespace myos {
	namespace kernel {
		// BaseAddressRegisterType 基址寄存器有两种类型：内存空间；I/O空间
		enum BaseAddressRegisterType
		{
			MemoryMapping = 0,
			InputOutput = 1,
		};

		// 基址寄存器
		class BaseAddressRegister {
		public:
			bool prefetchable;
			uint8_t* address;
			uint32_t size;
			BaseAddressRegisterType type;
		};


		// PeripheralComponentInterconnectDriverDescriptor PCI配置寄存器/配置空间
		class PeripheralComponentInterconnectDriverDescriptor {
		public:
			PeripheralComponentInterconnectDriverDescriptor();
			~PeripheralComponentInterconnectDriverDescriptor();
			
			uint32_t portBase;
			uint32_t interrupt;

			uint8_t  bus;
			uint8_t device;
			uint8_t function;

			uint16_t device_id;
			uint16_t vendor_id;

			uint8_t class_id;
			uint8_t subclass_id;
			uint8_t interface_id;
			uint8_t revision;
		};



		class PeripheralComponentInterconnectController {
		public:
			PeripheralComponentInterconnectController();
			~PeripheralComponentInterconnectController();

			uint32_t Read(uint8_t bus,uint8_t device,uint8_t function,uint8_t registeroffset);
			void write(uint8_t bus, uint8_t device, uint8_t function, uint8_t registeroffset,uint32_t value);
			bool DeviceHasFunctions(uint8_t bus, uint8_t devices);
			// SelectDrivers 遍历获取设备
			void SelectDrivers(DriverManager* manager,InterruptManager* interrupts);
			Driver* GetDriver(PeripheralComponentInterconnectDriverDescriptor dev, InterruptManager* manager);
			PeripheralComponentInterconnectDriverDescriptor GetDeviceDescriptor(uint8_t bus,uint8_t device,uint8_t function);
			// GetBaseAddressRegister 获取基址寄存器
			BaseAddressRegister GetBaseAddressRegister(uint8_t bus, uint8_t device, uint8_t function,uint8_t bar);
		private:
			Port32Bit commandport; // CONFIG_ADDRESS
			Port32Bit dataport;		// CONFIG_DATA
		};
	}
}


#endif // !MYOS_PCI_H
