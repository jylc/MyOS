#ifndef MYOS_DRIVER_H
#define MYOS_DRIVER_H
#include "types.h"
namespace myos {
	namespace kernel {
		class Driver {
		public:
			Driver();
			~Driver();

			virtual void Activate();
			virtual int Reset();
			virtual void Deactivate();
		};


		class DriverManager {
		public:
			DriverManager();
			void AddDriver(Driver*);
			void DriverAll();
			Driver* drivers[256];
			int numDrivers;
		};
	}
}

#endif // !MYOS_DRIVER_H
