/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/**
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

/** 
rewrite the simple code with reference of ArduPlane.cpp, use the HAL::Callbacks class instead.
MobiuS Z.X, 2016.03.10
*/
#include "simple.h"
/// 重载的 setup 方法, 完成串口与定时器变量初始化, 并打印提示信息
void SimpleTest::setup()
{
	/// initialise serial port
	serial_manager.init_console();
	/// initialise serial ports
	serial_manager.init();
	hal.console->printf("Hello in setup() from custom build of ameth@qq.com! \n");
	delta_ms = last_ms = AP_HAL::millis();
}

/// 重载的 loop 方法, 计算距上次输出的时间间隔是否达到500ms, 并打印提示信息
void SimpleTest::loop()
{
	delta_ms = AP_HAL::millis() - last_ms;
	if(abs(delta_ms - millis_span)<2){
		last_ms = AP_HAL::millis();
		hal.console->printf("--- loop in custom build of ameth@qq.com, %d ms elapsed since last call. \n", delta_ms);
		return;
	}
}

SimpleTest simple;

AP_HAL_MAIN_CALLBACKS(&simple);
