#include "fs.h"
int main()
{
	my_fs_init();
	
	device_init(); // initialize the devices
	//fs_init();  // initialize the file system
	//SD_TEST();
  	sys_init();	// initialize the OS
	_panic("?");
	return 0;
}


