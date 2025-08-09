#include <Arduino.h>
#include <LocoNet.h>
#include "TrafficControl\Settings.h"
#include "TrafficControl\UART.h"
#include "TrafficControl\Shift_reg.h"
#include "TrafficControl\Timer.h"
#include "TrafficControl\Logic.h"
#include "TrafficControl\LocoPacket.h"
#include "TrafficControl/Adafruit_INA219.h"

UART uart;
Timer timer;
LocoPacket loco;

Adafruit_INA219 ina219;  // По умолчанию используется адрес 0x40

void setup() {
    Serial.begin(9600);
    LocoNet.init(9);
    uart.print(PSTR("Test Board %d\n"), ADDR_BOARD);
    loco.send_B2_hello();

    uart.print(PSTR("Initializing INA219...\n"));
    if (!ina219.begin()) {
		//uart.print(PSTR("Failed to find INA219 chip"));
		//while (1) { delay(1000); }
	} else {
        //uart.print(PSTR("INA219 chip found\n"));
	}
	
	//ina219.setCalibration_32V_2A();  // Настройка по умолчанию (32 В, до 2 А)
}

void loop() {
    uart.read();
    loco.read();

    if (timer.ena_timer_1Hz) {
		uart.print(PSTR("check\n"));
        logic_timer_1Hz();
//
    //float shuntvoltage_mV = ina219.getShuntVoltage_mV();
    //float busvoltage_V    = ina219.getBusVoltage_V();
    //float loadvoltage_V   = busvoltage_V + (shuntvoltage_mV / 1000.0f);
    //float current_mA      = ina219.getCurrent_mA();
    //float power_mW        = ina219.getPower_mW();
//
    //// Вывод результатов
    //uart.print(PSTR("Bus Voltage: %d V\n"), busvoltage_V);
    //uart.print(PSTR("Shunt Voltage: %d mV\n"), shuntvoltage_mV);
    //uart.print(PSTR("Load Voltage:  %d V\n"), loadvoltage_V);
    //uart.print(PSTR("Current:       %d mA\n"), current_mA);
    //uart.print(PSTR("Power:         %d mW\n"), power_mW);
    //uart.print(PSTR("\n"));
//
        timer.ena_timer_1Hz = false;
    }
}