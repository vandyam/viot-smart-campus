#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "tremo_gpio.h"
#include "tremo_uart.h"
#include "tremo_adc.h"
#include "tremo_delay.h"
#include "tremo_system.h"
#include "tremo_rcc.h"
#include <ctype.h>
#include "platform.h"
#include "testcase.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BREAK_OUT_BOARD
#define BREAK_OUT_BOARD 1
#endif


#if DEBUG
static void dump_from_uart()
{
  uint8_t buff[100];
  uint8_t idx = 0;

  while(1)
    {
      buff[idx] = uart_receive_data(UART2);
      idx++;
      if(buff[idx - 1] == '\n')
	{
	  buff[idx] = '\0';
	  printf("Data from GPS: %s", buff);
	  idx = 0;
	}
    }
}
#endif

/* Sixth testcase implmentation */
/* temperature sensor testcase */
uint8_t sample_gpio_test(int *fsm)
{
  uint8_t i = 10;
  printf("Sample test: GPIO test\r\n");

  gpio_init(GPIOA, GPIO_PIN_15, GPIO_MODE_OUTPUT_PP_LOW);
  gpio_init(GPIOA, GPIO_PIN_8, GPIO_MODE_INPUT_PULL_DOWN);
  gpio_set_iomux(GPIOA, GPIO_PIN_8, 0);
  while(i--)
  {
    if(gpio_read(GPIOA, GPIO_PIN_8) == GPIO_LEVEL_LOW)
      {
        printf("GPIO pin is LOW\r\n");
        gpio_write(GPIOA, GPIO_PIN_15, GPIO_LEVEL_LOW);
      }
    else
      {
        printf("GPIO pin is HIGH\r\n");
        gpio_write(GPIOA, GPIO_PIN_15, GPIO_LEVEL_HIGH);
      }
      delay_ms(1000);
  }
  return 0;
}

uint8_t sample_adc_test(int *fsm)
{
  uint16_t adc_data_1[100] = {0};
  uint32_t sum_adc = 0;
  uint16_t avg_adc;
  float gain_value;
  float dco_value;
  uint8_t i;
  printf("Sample test: ADC test\r\n");
  rcc_set_adc_clk_source(RCC_ADC_CLK_SOURCE_RCO48M);
  rcc_enable_peripheral_clk(RCC_PERIPHERAL_ADC, true);
  adc_get_calibration_value(false, &gain_value, &dco_value);
  gpio_init(GPIOA, GPIO_PIN_5, GPIO_MODE_ANALOG);
  adc_init();
  adc_config_clock_division(20);
  adc_config_sample_sequence(0, 3);
  adc_config_conv_mode(ADC_CONV_MODE_CONTINUE);
  adc_enable(true);
  adc_start(true);
  for (i = 0; i < 100; i++)
    {
        while(!adc_get_interrupt_status(ADC_ISR_EOC));
        adc_data_1[i] = adc_get_data();
    }

  adc_start(false);
  adc_enable(false);

  for (i = 0; i < 100; i++)
  {
      sum_adc += adc_data_1[i];
  }
  avg_adc = sum_adc / 100;
  printf("Average ADC value: %d\r\n", avg_adc);
  return 0;
}

uint8_t sample_uart_test(int *fsm)
{
    uart_config_t uart_config;
    printf("Sample test: UART test\r\n");

    // Enable clocks
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_UART2, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOA, true);

    // UART2 pins MUST be GPIOA8/9
    gpio_set_iomux(GPIOA, GPIO_PIN_8, 1); // TX
    gpio_set_iomux(GPIOA, GPIO_PIN_9, 1); // RX

    // UART config
    uart_config_init(&uart_config);
    uart_config.baudrate = 9600;
    uart_config.data_width = UART_DATA_WIDTH_8;
    uart_config.parity = UART_PARITY_NO;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_control = UART_FLOW_CONTROL_DISABLED;
    uart_config.mode = UART_MODE_TXRX;
    uart_config.fifo_mode = 1;

    if (uart_init(UART2, &uart_config) != 0)
    {
        printf("UART2 init failed\r\n");
        return -1;
    }

    uart_cmd(UART2, true);

    // Clear FIFO BEFORE triggering sensor
    while (uart_get_flag_status(UART2, UART_FLAG_RX_FIFO_EMPTY) == RESET)
        uart_receive_data(UART2);

    // Trigger sensor Mode-5
    uart_send_data(UART2, 0x01);
    delay_ms(100);   // SR04M needs >50ms

    char buffer[128];
    int idx = 0;

    // Read sensor response
    while (uart_get_flag_status(UART2, UART_FLAG_RX_FIFO_EMPTY) == RESET)
    {
        char c = uart_receive_data(UART2);
        if (idx < sizeof(buffer) - 1)
            buffer[idx++] = c;
    }

    buffer[idx] = '\0';
    printf("Received RAW: %s\r\n", buffer);

    // Parse distance
    int value = 0;
    int found = 0;
    const char *p = buffer;

    while (*p)
    {
        if (isdigit((unsigned char)*p))
        {
            found = 1;
            value = value * 10 + (*p - '0');
        }
        else if (found)
        {
            break;
        }
        p++;
    }

    int16_t distance_mm = value;
    printf("Parsed Distance = %d mm\r\n", distance_mm);

    return 0;
}


uint8_t sample_send_data(int *fsm)
{
  printf("Sample test: Send data test\r\n");
  return 0;
}

/* Reboot the system */
uint8_t sample_test_reboot(int *fsm)
{
  printf("Rebooting.......\r\n\r\n");
  system_reset();

  return 0;
}

