#include "driver.h"

namespace myos {
	namespace kernel {
		Driver::Driver(){}
		
		Driver::~Driver() {}
	
		void Driver::Activate(){}

		int Driver::Reset() { return 0; }

		void Driver::Deactivate() {}
	
		DriverManager::DriverManager():numDrivers(0) {}

		void DriverManager::AddDriver(Driver* driver) {
			drivers[numDrivers++] = driver;
		}

		void DriverManager::DriverAll() {
			for (uint8_t i = 0; i < numDrivers; i++) {
				drivers[i]->Activate();
			}
		}
	}
}