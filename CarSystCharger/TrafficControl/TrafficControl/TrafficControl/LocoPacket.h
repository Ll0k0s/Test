#ifndef __LOCOPACKET_H__
#define __LOCOPACKET_H__

#include <LocoNet.h>

#define LOCO_START_B2_HELLO_BOARDS		0
#define LOCO_FINISH_B2_HELLO			199


class LocoPacket {
private:
	lnMsg	*lnrx;
	lnMsg	lntx;
	
	uint16_t start_B0_board = 1500;		//1500+
	uint16_t start_B2_board = 200;		// 200+
	uint16_t start_B2_Read = 2000;		// 200+

	void read_B2(uint16_t addr_B2, bool state);
	void read_B0(uint16_t addr_B2, bool state, bool ifClosed);
	void read_UserPack(uint8_t* data);

public:
	LocoPacket();
	
	lnMsg* read();	//чтение LocoNet2 команд

	void send_B2(uint16_t addr_B2, bool state);
	void send_B2_sensor(uint16_t addr_B2, bool state);
	void send_B2_hello();

	void send_B0(uint16_t addr_B0, bool state);
	void send_UserPack(uint8_t* data, uint8_t dataLen);
};

#endif //__LOCOPACKET_H__
