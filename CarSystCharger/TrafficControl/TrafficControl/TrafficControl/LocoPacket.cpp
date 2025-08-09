#include "Settings.h"
#include "Timer.h"
#include "LocoPacket.h"
#include "Logic.h"
#include "UART.h"

#if ENA_MODE_W_EEPROM == 0

extern UART uart;

LocoPacket::LocoPacket()
{
}

lnMsg* LocoPacket::read()
{
	while(LocoNet.available() > 0) {
		this->lnrx = LocoNet.receive();
		if (this->lnrx != NULL) {
	
			uint16_t Address = (this->lnrx->srq.sw1 | ( ( this->lnrx->srq.sw2 & 0x0F ) << 7 ));
			if( this->lnrx->sr.command != OPC_INPUT_REP ) {
				Address++;
			}
			//uart.print(PSTR("read LocoPack: addr %d\n"), Address);
			//for (uint8_t i = 0; i < 16; i++) {
				//uart.print(PSTR("%d "), this->lnrx->data[i]);
			//}
			//uart.print(PSTR("\n"));
			switch (this->lnrx->data[0]) {
				case 0xB2: {
					Address <<= 1 ;
					Address += ( this->lnrx->ir.in2 & OPC_INPUT_REP_SW ) ? 2 : 1 ;
					this->read_B2(Address, (bool)(this->lnrx->ir.in2 & OPC_INPUT_REP_HI));
					break;
				}
				case 0xB0: {
					this->read_B0(Address, (bool)(this->lnrx->srq.sw2 & OPC_SW_REQ_OUT), (bool)(this->lnrx->srq.sw2 & OPC_SW_REQ_DIR));
					break;
				}
				case 0xBB: {
					if (this->lnrx->data[1] == 0x03) {
						lnMsg Ln2TxPacket;
						Ln2TxPacket.data[0] = 0xE7;
						Ln2TxPacket.data[1] = 0x0E;
						Ln2TxPacket.data[2] = 0x03;
						Ln2TxPacket.data[3] = 0x00;
						Ln2TxPacket.data[4] = 0x00;
						Ln2TxPacket.data[5] = 0x00;
						Ln2TxPacket.data[6] = 0x00;
						Ln2TxPacket.data[7] = 0x47;
						Ln2TxPacket.data[8] = 0x00;
						Ln2TxPacket.data[9] = 0x00;
						Ln2TxPacket.data[10] = 0x00;
						Ln2TxPacket.data[11] = 0x00;
						Ln2TxPacket.data[12] = 0x00;
						LocoNet.send(&Ln2TxPacket);
					}
				}
			}
		} else {
			for (uint8_t i = 0; i < 16; i++) {
				this->lnrx->data[i] = 0x00;
			}
		}
	}

}

#pragma region B2
void LocoPacket::read_B2(uint16_t addr_B2, bool state)
{
	addr_B2--;
	if ( (addr_B2 == LOCO_START_B2_HELLO_BOARDS + ADDR_BOARD) || (addr_B2 == LOCO_FINISH_B2_HELLO) ) {
		if (state == false) {
			send_B2_hello();
		}
	}
	if (this->start_B2_Read <= addr_B2) {
		addr_B2 -= this->start_B2_Read;
		#if SHOW_LOCONET
		uart.print(PSTR("[Rx][READ_B2][%d]\n"), addr_B2);
		#endif

	} else if (this->start_B2_board <= addr_B2) {
		addr_B2 -= this->start_B2_board;
		#if SHOW_LOCONET
		uart.print(PSTR("[Rx][%dB2_%d]\n"), state, addr_B2);
		#endif
	}
}

//отправка по LocoNet2 команды B2
void LocoPacket::send_B2(uint16_t addr_B2, bool state)
{
	#if SHOW_LOCONET
	uart.print(PSTR("[Tx][%dB2_%d]\n"), state, addr_B2);
	#endif
	
	uint8_t board = (addr_B2) / 16;
	uint8_t input = (addr_B2) % 16;
	uint16_t result, input_2;
	lnMsg	Ln2TxPacket;

	if (input == 0 || input == 1) input_2 = 0;
	else if (input == 2 || input == 3) input_2 = 1;
	else if (input == 4 || input == 5) input_2 = 2;
	else if (input == 6 || input == 7) input_2 = 3;
	else if (input == 8 || input == 9) input_2 = 4;
	else if (input == 10 || input == 11) input_2 = 5;
	else if (input == 12 || input == 13) input_2 = 6;
	else if (input == 14 || input == 15) input_2 = 7;
	else input_2 = 0;

	result = board;                       //    [0,0,0,0,0,0,0,0][B7,B6,B5,B4,B3,B2,B1,B0]
	result <<= 3;                         //    [0,0,0,0,0,B7,B6,B5][B4,B3,B2,B1,B0,0,0,0]
	result += input_2;                    //	[0,0,0,0,0,B7,B6,B5][B4,B3,B2,B1,B0,I2,I1,I0] == [0,0,0,0,0,A10,A9,A8][A7,A6,A5,A4,A3,A2,A1,A0]

	Ln2TxPacket.data[0] = 0xB2;
	Ln2TxPacket.data[1] = (uint8_t)(result & 0x7F);  //	0b01111111			- select A6....A0
	Ln2TxPacket.data[2] = (uint8_t)(result >> 7);    //	0b11110000000 >> 7	- select A10...A7

	if (state)
	Ln2TxPacket.data[2] |= 0x10; // Set L

	if (input == 1 || input == 3 || input == 5 || input == 7 || input == 9 || input == 11 || input == 13 || input == 15)
	Ln2TxPacket.data[2] |= 0x20; // Set I

	Ln2TxPacket.data[2] |= 0x40; //  Set X = 1

	LocoNet.send(&Ln2TxPacket);
}

void LocoPacket::send_B2_hello()
{
	send_B2(LOCO_START_B2_HELLO_BOARDS + ADDR_BOARD, true);
}

void LocoPacket::send_B2_sensor(uint16_t addr_B2, bool state)
{
	send_B2(addr_B2 + start_B2_board, state);
}


#pragma endregion B2

#pragma region B0

void LocoPacket::read_B0(uint16_t addr_B0, bool state, bool ifClosed)
{
	#if SHOW_LOCONET
	uart.print(PSTR("[Rx][%dB0_%d]\n"), state, addr_B0);
	#endif
	if (this->start_B0_board <= addr_B0) {
		addr_B0 -= this->start_B0_board;
		#if SHOW_LOCONET
		uart.print(PSTR("[Rx][%dB0_%d][%d]\n"), state, addr_B0, ifClosed);
		#endif
	}
}

//отправка по LocoNet2 команды B0
void LocoPacket::send_B0(uint16_t addr_B0, bool state)
{
	#if SHOW_LOCONET
	uart.print(PSTR("[Tx][%dB0_%d]\n"), state, addr_B0);
	#endif

	addr_B0 += this->start_B0_board - 1;
	lnMsg SendPacket;

	SendPacket.data[0] = 0xB0;
	SendPacket.data[1] = (uint8_t)(addr_B0 & 0x7F);  //	0b01111111			- select A6....A0
	SendPacket.data[2] = (uint8_t)(addr_B0 >> 7);    //	0b11110000000 >> 7	- select A10...A7

	if (state) {
		SendPacket.data[2] |= 0x10; // Set ON
	}
	SendPacket.data[2] |= 0x20; // Set DIR
	LocoNet.send(&SendPacket);
}

#pragma endregion B0

#endif