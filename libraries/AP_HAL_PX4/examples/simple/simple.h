#include <AP_HAL/AP_HAL.h>
#include <AP_HAL/HAL.h>
#include <AP_Common/AP_Common.h>
#include <AP_HAL/system.h>
#include <GCS_MAVLink/GCS.h>
#include <GCS_MAVLink/GCS_MAVLink.h>    // MAVLink GCS definitions
#include <AP_SerialManager/AP_SerialManager.h>   // Serial manager library

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

/** \brief PX4测试类, 完成简单的串口初始化并打印信息
*/
class SimpleTest : public AP_HAL::HAL::Callbacks 
{
public:
    friend class GCS_MAVLINK;

    SimpleTest(void){};
	
	/// HAL::Callbacks implementation.
    void setup() override;
    void loop() override;
	
private:
	/// 串口管理器对象
    AP_SerialManager serial_manager;
	/// 定时器变量
	uint32_t last_ms, delta_ms;
	const uint32_t millis_span = 500;
};
