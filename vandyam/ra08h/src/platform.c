#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "timer.h"
#include "radio.h"
#include "tremo_uart.h"
#include "tremo_gpio.h"
#include "tremo_rcc.h"
#include "tremo_pwr.h"
#include "tremo_delay.h"
#include "tremo_rtc.h"
#include "rtc-board.h"
#include "platform.h"


extern int app_start(void);

#ifndef CONFIG_DEBUG_UART_IRQ
#define CONFIG_DEBUG_UART_IRQ UART0_IRQn
#endif

void uart_log_init(void)
{
    uart_config_t uart_config;

    // uart0
    gpio_set_iomux(GPIOB, GPIO_PIN_0, 1);
    gpio_set_iomux(GPIOB, GPIO_PIN_1, 1);

    /* uart config struct init */
    uart_config_init(&uart_config);

    uart_config.baudrate = UART_BAUDRATE_115200;
    uart_init(CONFIG_DEBUG_UART, &uart_config);
    uart_cmd(CONFIG_DEBUG_UART, ENABLE);

    /* Enable RX interrupt */
    uart_config_interrupt(CONFIG_DEBUG_UART, UART_INTERRUPT_RX_DONE, ENABLE);
    uart_config_interrupt(CONFIG_DEBUG_UART, UART_INTERRUPT_RX_TIMEOUT, ENABLE);
    NVIC_EnableIRQ(CONFIG_DEBUG_UART_IRQ);
}

void board_init()
{
    rtc_calendar_t time;

    rcc_enable_oscillator(RCC_OSC_XO32K, true);

    rcc_enable_peripheral_clk(RCC_PERIPHERAL_UART0, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_UART2, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOA, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOB, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOD, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_PWR, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_RTC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_SAC, true);
    rcc_enable_peripheral_clk(RCC_PERIPHERAL_LORA, true);

    delay_ms(100);
    pwr_xo32k_lpm_cmd(true);

    uart_log_init();

    /* Preserve the RTC time */
    rtc_calendar_cmd(ENABLE);
    rtc_get_calendar(&time);

    RtcInit();

    /* Restore the RTC */
    rtc_set_calendar(&time);
    rtc_check_syn();

}

#ifdef USE_FULL_ASSERT
void assert_failed(void *file, uint32_t line)
{
    (void)file;
    (void)line;

    while (1)
    {
    }
}
#endif
