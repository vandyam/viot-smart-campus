#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdio.h>
void console_process_rx_data(uint8_t data);
void console_flush();
uint8_t console_read(uint8_t *buff, uint8_t sz);
uint8_t console_a_char();
#endif
