#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CONSOLE_FIFO_MAX 140

typedef struct console
{
  /* Input data stored here */
  uint8_t rx_data[CONSOLE_FIFO_MAX];
  /* Index */
  volatile uint8_t idx;
}console_t;

static  console_t console;

/* Call from IRQ handler */
void console_process_rx_data(uint8_t data)
{
  if(console.idx < CONSOLE_FIFO_MAX)
    {
      console.rx_data[console.idx] = data;
      console.idx++;
    }
}

/* Flush all data */
void console_flush()
{
  console.idx = 0;
}

/* Read from the FIFO */
uint8_t console_read(uint8_t *buff, uint8_t sz)
{
  if(console.idx >= sz)
    {
      memcpy(buff, console.rx_data, sz);
      console.idx = 0;
      return 0;
    }
  return 1;
}

/* Read input from console until user press enter key */
uint8_t console_a_char()
{
    uint8_t input = 0;
    uint8_t flag = 0;
    uint8_t ch = 0;

    while (!flag)
    {
      while (console.idx == 0);

      ch = console.rx_data[0];

      if (ch == '\r' || ch == '\n')
      {
        flag = 1;
      }
      else if(ch >= '0' && ch <= '9')
      {
        input = (input * 10) + (ch - '0');
      }

      if(console.idx > 1)
        {
          memmove(console.rx_data, console.rx_data + 1, console.idx - 1);
          console.idx--;
        }
      else
        {
          console.idx = 0;
        }
    }
    return input;
}