#ifndef __UART_H__
#define __UART_H__

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#define UART_BUF_RX_SIZE		64
#define UART_BUF_RX_MASK		UART_BUF_RX_SIZE - 1

class UART
{
	private:
	char buff_rx[UART_BUF_RX_SIZE];
	uint8_t cnt_rx_save;
	uint8_t cnt_rx_read;

	void clear_buff_rx();
	bool read_name(const char* str);
	uint16_t read_num();
	void read_comm();

	public:
	UART();

	void print(const char* typeFormat, ...);
	void read();
	
}; //UART

#endif //__UART_H__