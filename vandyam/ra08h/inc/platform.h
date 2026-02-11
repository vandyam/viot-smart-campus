#ifndef _PLATFORM_H_
#include <stdint.h>

#define _PLATFORM_H_
void uart_log_init(void);
void board_init();
void platform_show_proress();
void uart_gps_init_baud(uint32_t baud);
void platform_gps_fon_off();
#endif
