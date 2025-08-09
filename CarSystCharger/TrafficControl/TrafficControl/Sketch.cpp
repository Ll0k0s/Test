#include <Arduino.h>
#include <LocoNet.h>
#include "TrafficControl\Settings.h"
#include "TrafficControl\UART.h"
#include "TrafficControl\Shift_reg.h"
#include "TrafficControl\Timer.h"
#include "TrafficControl\Logic.h"
#include "TrafficControl\LocoPacket.h"
#include "TrafficControl/Wire/src/Wire.h"

UART uart;
Timer timer;
LocoPacket loco;


const uint8_t INA219_ADDR = 0x40;

static void ina219_writeReg(uint8_t reg, uint16_t val) {
    Wire.beginTransmission(INA219_ADDR);
    Wire.write(reg);
    Wire.write((uint8_t)(val >> 8));
    Wire.write((uint8_t)val);
    Wire.endTransmission();
}

static uint16_t ina219_readReg(uint8_t reg) {
    Wire.beginTransmission(INA219_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(INA219_ADDR, (uint8_t)2);
    uint16_t v = ((uint16_t)Wire.read() << 8) | Wire.read();
    return v;
}

static void ina219_init() {
    Wire.begin(0x40);
    // Конфигурация: 32V, 320mV, 12-bit, continuous
    ina219_writeReg(0x00, 0x019F);
    // Калибровка (пример для шунта 0.1 Ом и макс ~3.2А)
    // CAL = 4096 (0x1000) условно; подстройте под ваш шунт.
    ina219_writeReg(0x05, 0x1000);
}

static void ina219_readAndPrint() {
    int16_t shunt_raw = (int16_t)ina219_readReg(0x01);      // 10uV/LSB
    uint16_t bus_raw   = ina219_readReg(0x02);              // 4mV/LSB, бит0..2 status
    int16_t power_raw  = (int16_t)ina219_readReg(0x03);     // зависит от CAL
    int16_t current_raw= (int16_t)ina219_readReg(0x04);     // зависит от CAL

    float shunt_mV = shunt_raw * 0.01f;                     // 10uV -> mV
    float bus_V    = (bus_raw >> 3) * 0.004f;               // 4mV шаг
    // Для выбранной калибровки предположим 1mA/LSB (примерно, пересчёт зависит от CAL/шунта)
    float current_mA = current_raw * 1.0f;
    // Power регистр обычно 20 * current_LSB
    float power_mW = power_raw * 20.0f * 1.0f;              // если 1mA/LSB

    uart.print(PSTR("INA219: V=%.2fV I=%.1fmA P=%.0fmW Vsh=%.2fmV\n"),
               bus_V, current_mA, power_mW, shunt_mV);
}


void setup() {
    Serial.begin(9600);
    LocoNet.init(9);
    uart.print(PSTR("Test Board %d\n"), ADDR_BOARD);
    loco.send_B2_hello();
    ina219_init();
}

void loop() {
    uart.read();
    loco.read();

    if (timer.ena_timer_1Hz) {
		uart.print(PSTR("check\n"));
        logic_timer_1Hz();
        //ina219_readAndPrint();
        timer.ena_timer_1Hz = false;
    }
	_delay_ms(100);
}