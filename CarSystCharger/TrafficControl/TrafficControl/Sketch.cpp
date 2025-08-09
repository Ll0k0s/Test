#include <Arduino.h>
#include <LocoNet.h>
#include "TrafficControl\Settings.h"
#include "TrafficControl\UART.h"
#include "TrafficControl\Shift_reg.h"
#include "TrafficControl\Timer.h"
#include "TrafficControl\Logic.h"
#include "TrafficControl\LocoPacket.h"
//#include <Wire.h>

UART uart;
Timer timer;
LocoPacket loco;

const float VREF = 5; // Опорное напряжение (измените на 3.3, если используется 3.3 В)

void setup() {
    //	_delay_ms(3000);
    Serial.begin(9600);
    LocoNet.init(9);
    uart.print(PSTR("Test Board %d\n"), ADDR_BOARD);
    loco.send_B2_hello();

}

void loop() {
    uart.read();
    loco.read();

    if (timer.ena_timer_1Hz) {


        logic_timer_1Hz();
        timer.ena_timer_1Hz = false;
    }
	_delay_ms(100);
}