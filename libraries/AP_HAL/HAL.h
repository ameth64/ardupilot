#pragma once

#include "AP_HAL_Namespace.h"

#include "AnalogIn.h"
#include "GPIO.h"
#include "RCInput.h"
#include "RCOutput.h"
#include "SPIDriver.h"
#include "Storage.h"
#include "UARTDriver.h"
#include "system.h"
#include "OpticalFlow.h"

/*! \brief 通用的硬件抽象层,虚基类.

该类定义了一组抽象数据成员及成员函数, 供其它HAL具象类实现
*/
class AP_HAL::HAL {
public:
	/** \brief HAL构造函数
	初始化各数据成员.
	*/
    HAL(AP_HAL::UARTDriver* _uartA, ///< 对应串口console
        AP_HAL::UARTDriver* _uartB, ///< 对应1st GPS
        AP_HAL::UARTDriver* _uartC, ///< 对应telem1
        AP_HAL::UARTDriver* _uartD, ///< 对应telem2
        AP_HAL::UARTDriver* _uartE, ///< 对应2nd GPS
        AP_HAL::I2CDeviceManager* _i2c_mgr,
        AP_HAL::I2CDriver*  _i2c0,
        AP_HAL::I2CDriver*  _i2c1,
        AP_HAL::I2CDriver*  _i2c2,
        AP_HAL::SPIDeviceManager* _spi,
        AP_HAL::AnalogIn*   _analogin,
        AP_HAL::Storage*    _storage,
        AP_HAL::UARTDriver* _console,
        AP_HAL::GPIO*       _gpio,
        AP_HAL::RCInput*    _rcin,
        AP_HAL::RCOutput*   _rcout,
        AP_HAL::Scheduler*  _scheduler,
        AP_HAL::Util*       _util,
        AP_HAL::OpticalFlow *_opticalflow)
        :
        uartA(_uartA),
        uartB(_uartB),
        uartC(_uartC),
        uartD(_uartD),
        uartE(_uartE),
        i2c_mgr(_i2c_mgr),
        i2c(_i2c0),
        i2c1(_i2c1),
        i2c2(_i2c2),
        spi(_spi),
        analogin(_analogin),
        storage(_storage),
        console(_console),
        gpio(_gpio),
        rcin(_rcin),
        rcout(_rcout),
        scheduler(_scheduler),
        util(_util),
        opticalflow(_opticalflow)
    {
        AP_HAL::init();
    }

	/** 回调函数的仿函数对象
	*/
    struct Callbacks {
        virtual void setup() = 0;	///< 该方法在硬件系统启动后在 main 函数中被调用, 完成初始化操作
        virtual void loop() = 0;	///< 该方法由 scheduler 对象定时调用
    };

	/** 回调函数的仿函数对象
	*/
    struct FunCallbacks : public Callbacks {
        FunCallbacks(void (*setup_fun)(void), void (*loop_fun)(void));

        void setup() override { _setup(); }
        void loop() override { _loop(); }

    private:
        void (*_setup)(void);	///< setup 的函数指针, 见 Callbacks 结构
        void (*_loop)(void);	///< loop 的函数指针, 见 Callbacks 结构
    };

    virtual void run(int argc, char * const argv[], Callbacks* callbacks) const = 0;	///< 该方法为命令行启动入口

    AP_HAL::UARTDriver* uartA;
    AP_HAL::UARTDriver* uartB;
    AP_HAL::UARTDriver* uartC;
    AP_HAL::UARTDriver* uartD;
    AP_HAL::UARTDriver* uartE;
    AP_HAL::I2CDeviceManager* i2c_mgr;
    AP_HAL::I2CDriver*  i2c;
    AP_HAL::I2CDriver*  i2c1;
    AP_HAL::I2CDriver*  i2c2;
    AP_HAL::SPIDeviceManager* spi;
    AP_HAL::AnalogIn*   analogin;
    AP_HAL::Storage*    storage;
    AP_HAL::UARTDriver* console;
    AP_HAL::GPIO*       gpio;
    AP_HAL::RCInput*    rcin;
    AP_HAL::RCOutput*   rcout;
    AP_HAL::Scheduler*  scheduler;
    AP_HAL::Util        *util;
    AP_HAL::OpticalFlow *opticalflow;
};
