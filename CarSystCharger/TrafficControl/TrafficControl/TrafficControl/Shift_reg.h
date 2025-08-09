#ifndef __SHIFT_REG_H__
#define __SHIFT_REG_H__

#include <avr/io.h>

#define SIZE_BUF_SHIFT 15	//������ ������ ��� ������ ������� (n^2-1)

class Shift_reg
{
private:
	uint8_t pin_input_165 = 18;		//	input
	uint8_t pin_lock_165 = 17;		//	input
	uint8_t pin_output_595 = 15;	//	output
	uint8_t pin_lock_595 = 4;		//	output
	uint8_t pin_clk = 16;			//	input + output

	uint64_t input;						//������� ������
	uint64_t buff[SIZE_BUF_SHIFT + 1];	//����� � ����������� ���������� ��������
	uint8_t buf_cnt_r;					//������� ������ ��� ������
	uint8_t buf_cnt_w;					//������� ������ ��� ������
	uint32_t output;					//�������� ������
public:
	bool in[48];			//�������� ������� �����
	bool out[32];			//������� ��������� ���� � �������� ����� ����������
	bool out_board[32];		//�������� �������� �����
	uint16_t out_cnt[32];	//������� ������� �� ���������� ��������� ��������� ����

	Shift_reg();
	void read_in(void);
	void send_out(void);
	void detect_in(void);
	void handler_detecor(void);
};

#endif //__SHIFT_REG_H__