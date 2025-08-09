#ifndef LOGIC_H_
#define LOGIC_H_

bool type_magnet_parking(uint16_t addr_B0);		//определение типа магнита по адресу
void change_out(uint16_t addr_B0, bool state);
void reset_outs();
void logic_B0(uint16_t addr_B0, bool state);
void handler_detecor(void);		//обновление входных пинов
void logic_timer_1Hz();
void logic_timer_2Hz();
void logic_timer_10Hz();


void reset_data();

#endif /*LOGIC_H_*/
