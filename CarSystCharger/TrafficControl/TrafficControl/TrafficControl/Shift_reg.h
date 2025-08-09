#ifndef __SHIFT_REG_H__
#define __SHIFT_REG_H__

#include <avr/io.h>

#define SIZE_BUF_SHIFT 15	//размер буфера для чтения датчика (n^2-1)

class Shift_reg
{
private:
	uint8_t pin_input_165 = 18;		//	input
	uint8_t pin_lock_165 = 17;		//	input
	uint8_t pin_output_595 = 15;	//	output
	uint8_t pin_lock_595 = 4;		//	output
	uint8_t pin_clk = 16;			//	input + output

	uint64_t input;						//входные данные
	uint64_t buff[SIZE_BUF_SHIFT + 1];	//буфер с измененными значениями датчиков
	uint8_t buf_cnt_r;					//счетчик записи для буфера
	uint8_t buf_cnt_w;					//счетчик чтения для буфера
	uint32_t output;					//выходные данные
public:
	bool in[48];			//значения входных пинов
	bool out[32];			//входные состояние пина к которому будет стремиться
	bool out_board[32];		//значения выходных пинов
	uint16_t out_cnt[32];	//счетчик времени во включенном состоянии выходного пина

	Shift_reg();
	void read_in(void);
	void send_out(void);
	void detect_in(void);
	void handler_detecor(void);
};

#endif //__SHIFT_REG_H__