#include "Settings.h"
#include "UART.h"
#include "Logic.h"
#include "LocoPacket.h"

extern LocoPacket loco;

UART::UART()
{
	this->cnt_rx_read = 0;
	this->cnt_rx_save = 0;
}

void UART::print(const char* typeFormat, ...) {
	va_list args;
	va_start(args, typeFormat);
	size_t bufferSize = vsnprintf_P(NULL, 0, typeFormat, args) + 1;
	char* buffer = (char*)malloc(bufferSize);
	if (buffer) {
		vsnprintf_P(buffer, bufferSize, typeFormat, args);
		Serial.print(buffer);
		free(buffer);
	}
	va_end(args);
}


void UART::clear_buff_rx()
{
	for (uint8_t i = 0; i < UART_BUF_RX_SIZE; i++) {
		this->buff_rx[i] = 0;
	}
	this->cnt_rx_save = 0;
	this->cnt_rx_read = 0;
}

bool UART::read_name(const char* str)
{
	uint8_t cnt_rx_read = this->cnt_rx_read;
	if (this->cnt_rx_save >= strlen(str)) {
		for (uint8_t i = 0; i < strlen(str); i++, this->cnt_rx_read++) {
			if (this->buff_rx[this->cnt_rx_read] != str[i]) {
				this->cnt_rx_read = cnt_rx_read;
				return 0;
			}
		}
		if (this->buff_rx[this->cnt_rx_read] == 32) {
			this->cnt_rx_read++;
		}
		return 1;
	}
	this->cnt_rx_read = cnt_rx_read;
	return 0;
}

uint16_t UART::read_num()
{
	char num[16] = {0};
	uint16_t n = 0xFFFF;

	for (uint8_t i = 0; i < strlen(this->buff_rx); i++, this->cnt_rx_read++) {
		if ((this->buff_rx[this->cnt_rx_read] == 0)
		|| (this->buff_rx[this->cnt_rx_read] == 32)) {
			i = strlen(this->buff_rx);
			} else {
			num[i] = this->buff_rx[this->cnt_rx_read];
		}
	}
	if (strlen(num) != 0) {
		return atoi(num);
		} else {
		return 0xFFFF;
	}
}

void UART::read(void)
{
	if (Serial.available() > 0) {
		uint8_t data = Serial.read();
		if ((data != 13) && (data != 10)) {
			this->buff_rx[this->cnt_rx_save] = data;
			this->cnt_rx_save++;
			if (this->cnt_rx_save == UART_BUF_RX_MASK) {
				print(PSTR("comm: buffer overflow\n"));
				this->clear_buff_rx();
			}
			this->cnt_rx_save &= UART_BUF_RX_MASK;
			} else if (data == 10) {
			print(PSTR("comm: %s\n"), this->buff_rx);
			read_comm();
		}
	}
}

void UART::read_comm()
{
	if (read_name("b2 on")) {		//command: [b2 on ][addr_B2]
		uint16_t addr_B2 = read_num();

	} else if (read_name("b2 off")) {		//command: [b2 off ][addr_B2]
		uint16_t addr_B2 = read_num();

	} else {
		print(PSTR("comm: undefined command\n"));
	}
	this->clear_buff_rx();
}
   