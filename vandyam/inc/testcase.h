#ifndef TESTCASE_H
#define TESTCASE_H

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"

/* Maximum testcases */
#define MAX_TESTCASES 5

#define FACTORY_TEST_START 1
#define FACTORY_TEST_END   (FACTORY_TEST_START + MAX_TESTCASES)

// Define the CustomTimer structure for measuring latency
typedef struct timestamp
{
  TimerTime_t start_time; // Declare a global variable to store the start time
  TimerTime_t stop_time; // Declare a global variable to store the stop time
} timestamp_t;

// Define the CustomTimer structure for measuring latency
typedef struct {
    TimerTime_t start_time;
    bool active;
} CustomTimer_t;

uint8_t sample_test_reboot(int *fsm);

uint8_t sample_test_set_baud_uart115k(int *fsm);

uint8_t sample_gpio_test(int *fsm);

uint8_t sample_uart_test(int *fsm);

uint8_t sample_send_data(int *fsm);

uint8_t sample_adc_test(int *fsm);

#endif /* TESTCASE_H */
