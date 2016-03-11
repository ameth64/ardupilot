/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/*
  simple hello world sketch
  Andrew Tridgell September 2011
*/

#include <AP_HAL/AP_HAL.h>

/*
const AP_HAL::HAL& hal = AP_HAL::get_HAL();

void setup() {
	hal.console->println("hello world from setup method of PX4 firmware-based board!");
#if HAVE_OS_POSIX_IO
	::printf("Hello from UART printf() call!");
#endif
	
}

void loop()
{
	hal.scheduler->delay(50);
	hal.console->println("*---delay for 1000ms...");
}

AP_HAL_MAIN();
*/

/* --2016.03.10
rewrite the simple code with reference of ArduPlane.cpp, use the HAL::Callbacks class instead.
MobiuS Z.X
*/
#include "simple.h"

void SimpleTest::init()
{
	// initialise serial port
	serial_manager.init_console();
	// initialise serial ports
	serial_manager.init();
}

void SimpleTest::setup()
{
	init();
	hal.scheduler->delay(500);
	hal.console->printf("Hello in setup() from custom build of ameth@qq.com! \n");
}

void SimpleTest::loop()
{
	hal.console->printf("--- loop in custom build of ameth@qq.com! \n");
	hal.scheduler->delay(1000);
}

SimpleTest simple;

AP_HAL_MAIN_CALLBACKS(&simple);
