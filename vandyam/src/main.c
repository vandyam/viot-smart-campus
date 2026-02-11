#ifndef HW_RAO8
#define HW_RAO8 1
#endif

#include <stdio.h>
#include <stdint.h>
#include "tremo_uart.h"
#include "platform.h"
#include "testcase.h"
#include "xmodem.h"
#include "console.h"

#define BL_VERSION "P1_0_0"
#define BL_BOARD_NAME "VANDYAM_BOARD"


/* All possible testcases here */
typedef uint8_t (*test_fptr_t) (int *fsm);

/* List of test cases */
typedef struct testcases
{
  test_fptr_t fnptr;
  uint8_t help[48];
} testcases_t;
  
/* List of all sample testcases */
static const testcases_t tests[MAX_TESTCASES] = {
  {
    sample_gpio_test,
    "GPIO test\r\n"
  },
  {
    sample_adc_test,
    "ADC test\r\n"
  },
  {
    sample_uart_test,
    "UART test\r\n"
  },
  {
    sample_send_data,
    "Send data test\r\n"
  },
  {
    sample_test_reboot,
    "Reboot\r\n"
  },  
};

static void print_banner()
{
  printf("**************************************\r\n");
  printf("Welcome to Vandyam test image \r\n");
  printf("Board Name: %s\r\n", BL_BOARD_NAME);
  printf("sample Test Version:%s\r\n", BL_VERSION);
  printf("**************************************\r\n");
}

/* Print test menu */
static void print_main_menu()
{
  uint8_t counter;

  printf("\r\n*****************\r\n");
  printf("   Main Menu\r\n");
  printf("*****************\r\n");

  /* Print test cases */
  for(counter = 0; counter < MAX_TESTCASES; counter++)
    {
      printf("%d. %s\r\n", counter + 1, tests[counter].help);
    }

  printf("Please select the testcase number: ");
}


int main(void)
{
  uint8_t input = 0;
  int fsm = 0;

  /* Initialize the platform */
  board_init();

  /* Print the banner */
  print_banner();

  while(1)
    {
      /* print the main menu */
      print_main_menu();

      /* Get user input */
      console_flush();

      input = console_a_char();


      /* Jump to next line */
      printf("\r\n");

      /* validate the input */
      if(input >= FACTORY_TEST_START && input <= FACTORY_TEST_END)
	{
	  /* Get the index */
	  input = input - FACTORY_TEST_START;

	  /* Run the function */
	  tests[input].fnptr(&fsm);
	}
    }
}
