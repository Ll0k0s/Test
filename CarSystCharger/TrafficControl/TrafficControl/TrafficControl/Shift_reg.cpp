#include "Shift_reg.h"
#include "Settings.h"
#include "Timer.h"
#include "UART.h"
#include "LocoPacket.h"

extern UART uart;
extern LocoPacket loco;

// default constructor
Shift_reg::Shift_reg()
{
	this->input = 0;
	for (uint8_t i = 0; i < SIZE_BUF_SHIFT; i++) {
		this->buff[SIZE_BUF_SHIFT] = 0;
	}
	this->buf_cnt_r = 0;
	this->buf_cnt_w = 0;
	this->output = 0;
	for (uint8_t i = 0; i < 48; i++) {
		this->in[i] = false;
	}
	for (uint8_t i = 0; i < 32; i++) {
		this->out[i] = false;
	}
	for (uint8_t i = 0; i < 32; i++) {
		this->out_board[i] = false;
	}
	for (uint8_t i = 0; i < 32; i++) {
		this->out_cnt[i] = TIME_AUTO_OFF_OUTS;
	}
	//input
	pinMode(pin_input_165, INPUT);
	pinMode(pin_lock_165, OUTPUT);
	//output
	pinMode(pin_output_595, OUTPUT);
	pinMode(pin_lock_595, OUTPUT);
	//input + output
	pinMode(pin_clk, OUTPUT);

	//input
	digitalWrite(pin_lock_165, LOW);
	//output
	digitalWrite(pin_output_595, LOW);
	digitalWrite(pin_lock_595, LOW);
	//input + output
	digitalWrite(pin_clk, LOW);

	send_out();
}


//���������� ������� �����
void Shift_reg::read_in(void) {
	digitalWrite(pin_input_165, HIGH);
	digitalWrite(pin_lock_165, LOW);
	digitalWrite(pin_lock_165, HIGH);
	digitalWrite(pin_input_165, LOW);
	uint64_t tmp = 0;	//�������� ������� ����� (��������, ���� �� ����� ������� ��� ������)
	bool state;			//��������� �������� ����
	for (uint8_t b = 0; b < 48; b++) {	//������� ������� �����
		state = digitalRead(pin_input_165);
		tmp = (tmp << 1) + state;		//������ ������ � tmp
		digitalWrite(pin_clk, HIGH);
		digitalWrite(pin_clk, LOW);
	}
	this->input = tmp;	//���������� �������� ������� �����
}

//���������� �������� �����
void Shift_reg::send_out(void) {
	digitalWrite(pin_lock_595, LOW);
	uint32_t out = this->output;			//�������� ��������� �������� ����� ��� �� ���������
	for (uint8_t i = 0; i < 32; i++) {		//������� �������� �����
		if (this->out_board[i] == true) {	//���� �������� ��� - true
			digitalWrite(pin_output_595, HIGH);
		} else {
			digitalWrite(pin_output_595, LOW);
		}
		digitalWrite(pin_clk, HIGH);
		digitalWrite(pin_clk, LOW);
	}
	digitalWrite(pin_lock_595, HIGH);
}

// ************************** ��������� ************************** //

//����������� ��������� �����
void Shift_reg::detect_in(void) {
	this->read_in();
	uint64_t change = this->buff[this->buf_cnt_w] ^ this->input;	//������ ������� �������� ��� ��������
	if (change > 0) {												//���� �� ����� ��������� ���������
		this->buf_cnt_w++; this->buf_cnt_w &= SIZE_BUF_SHIFT;		//���������� �������� �� ��������� �������
		this->buff[this->buf_cnt_w] = this->input;					//���������� ������ � ������
	}

	if (this->buf_cnt_r != this->buf_cnt_w) {					//���� ��������� ������ � �����
		this->buf_cnt_r++; this->buf_cnt_r &= SIZE_BUF_SHIFT;	//������� � ���������� �������� ������
		for (uint8_t i = 0; i < 48; i++) {						//������� �����
			uint8_t pin = 47 - i;								//����� ����
			if (this->in[pin] ^ (~(this->buff[this->buf_cnt_r] >> i) & 1)) {			//���� ��������� ���������
				this->input = this->buff[this->buf_cnt_r];							//������ ������ � "input"
				this->in[pin] = (~(this->buff[this->buf_cnt_r] >> i) & 1) ? 1 : 0;	//��������� ������ "input"
				
				#if ADDR_BOARD == 3
				if (pin == 176 % 48) {		//���������� addr: 176 -> 179
					pin = 179 % 48;
				}
				#endif
				#if ADDR_BOARD == 2
				if (pin == 126 % 48) {		//���������� addr: 126 -> 123
					pin = 123 % 48;
				}
				#endif
				uint16_t addr = 48 * ADDR_BOARD + pin;

				if (this->in[pin] == true) {
					#if ENA_MODE_DEBUG
					uart.print(PSTR("Sensor %d is on\n"), addr);
					#endif
					loco.send_B2_sensor(addr, true);
				} else {
					#if ENA_MODE_DEBUG
					uart.print(PSTR("Sensor %d is off\n"), addr);
					#endif
				}
			}
		}
	}
}
