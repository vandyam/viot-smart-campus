#include <stdint.h>
#include "tremo_uart.h"
#include "tremo_delay.h"
#include "crc16.h"
#include "console.h"
#include "xmodem.h"
#include <stddef.h>

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A
  
/* All possible testcases here */
typedef xmodem_state_t (*xmodem_fptr_t) (xmodem_t *xmo);

static xmodem_state_t xmodem_start(xmodem_t *xmo);
static xmodem_state_t xmodem_cmd(xmodem_t *xmo);
static xmodem_state_t xmodem_128b(xmodem_t *xmo);
static xmodem_state_t xmodem_1024b(xmodem_t *xmo);
static xmodem_state_t xmodem_eot(xmodem_t *xmo);

const static xmodem_fptr_t handlers[state_result] = {
  xmodem_start,
  xmodem_cmd,
  xmodem_128b,
  xmodem_1024b,
  xmodem_eot
};

/* Init function */
void xmodem_init(xmodem_t *xmo, xmo_cb_fptr_t cbptr, void *arg)
{
  xmo->cur_state = state_init;
  xmo->next_packet = 1;
  xmo->cb = cbptr;
  xmo->cb_arg = arg;

  /* Flush the input */
  console_flush();

  printf("\r\nAttention!!!!!\r\n");
  printf("Please send file using xmodem protocl (Ctrl+a+z+s)\r\n");
  printf("After transfer press key \"Q\" twice to continue\r\n");
}

/* run the statemachine */
xmodem_state_t xmodem_run(xmodem_t *xmo)
{
  if(xmo->cur_state < state_result)
    {
      xmo->cur_state = handlers[xmo->cur_state](xmo);
    }
  return xmo->cur_state;
}

/* Process a command */
static xmodem_state_t  get_state_from_cmd(uint8_t a_byte, xmodem_state_t c_state)
{
  xmodem_state_t next_state = c_state;

  if(a_byte == SOH)
    {
      next_state = state_128B;
    }
  else if(a_byte == STX)
    {
      next_state = state_1024B;
    }
  else if(a_byte == EOT)
    {
      next_state = state_eot;
    }
  else if(a_byte == CAN)
    {
      while(1)
	{
	  printf("CAN\r\n");
	}
    }

  return next_state;
}

/* Compare CRC */
static uint8_t check(uint8_t *buff, uint16_t sz)
{
  uint16_t crc = crc16_ccitt(buff, sz);
  uint16_t tcrc = (buff[sz] << 8) + buff[sz + 1];

  if (crc == tcrc)
    {
      return 1;
    }

  return 0;
}

/* Execute at the start of every transaction */
static xmodem_state_t xmodem_start(xmodem_t *xmo)
{
  uint8_t a_byte;
  xmodem_state_t next_state = state_init;

  a_byte = 0;
  while(a_byte == 0)
    {
      /* Send Checksum */
      uart_send_data(CONFIG_DEBUG_UART, 'C');

      /* Wait for the response */
      if(console_read(&a_byte, 1) == 0)
	{
	  /* Fidn next state from the data */
	  next_state = get_state_from_cmd(a_byte, state_init);
	} 

      delay_ms(1000);
    }
  return next_state;
}

/* Start of a transaction */
static xmodem_state_t xmodem_cmd(xmodem_t *xmo)
{
  uint8_t a_byte;
  xmodem_state_t next_state = state_cmd;

  a_byte = console_a_char();
  next_state = get_state_from_cmd(a_byte, state_cmd);

  return next_state;
}

#define  XMODEM_128B 133
#define PKT_NO 1
#define T_PKT_NO 2
#define PLOAD_IDX 3

/* 128 Byte transfer */
static xmodem_state_t xmodem_128b(xmodem_t *xmo)
{
  uint8_t ret;
  xmodem_state_t next_state = state_128B;
  uint8_t data[XMODEM_128B];

  ret = 1;
  while(ret != 0)
    {
      /* Get SOH + Blk No + ~Blk No + 128Byte data + checksum + EOP */
      ret = console_read(data, 133);
    }

  /* Validation */
  ret = 0;

  /* Packet number and complement */
  if(data[PKT_NO] != (uint8_t)~data[T_PKT_NO])
    {
      ret |= XM_F_CMP;
    }

  /* Packet number & expected packet number */
  if(data[PKT_NO] != xmo->next_packet)
    {
      ret |= XM_F_PKT;
    }

  /* CRC */
  if(check(&data[PLOAD_IDX], 128) == 0)
    {
      ret |= XM_F_CRC;
    }

  if(ret != 0)
    {
      /* Abort the transaction - no retry */
      uart_send_data(CONFIG_DEBUG_UART, CAN);

      /* Expose the result */
      xmo->result = ret;

      return state_result;
    }
    
  /* Send data */
  if(xmo->cb != NULL)
    {
      xmo->cb(xmo->cb_arg, &data[PLOAD_IDX], 128);
    }
  
  /* Send ACK */
  uart_send_data(CONFIG_DEBUG_UART, ACK);
  next_state = state_cmd;
  xmo->next_packet += 1;

  return next_state;
}

/* 1024 Bytes data transfer */
static xmodem_state_t xmodem_1024b(xmodem_t *xmo)
{
  xmodem_state_t next_state = state_1024B;

  while(1)
    {
      printf("TODO: 1024 Byte Data\r\n");
    }
  return next_state;
}

/* End of transmission */
static xmodem_state_t xmodem_eot(xmodem_t *xmo)
{
  xmodem_state_t next_state;

  uart_send_data(CONFIG_DEBUG_UART, ACK);

  next_state = state_result;

  /* Say success */
  xmo->result = XM_SUCCESS;

  return next_state;
}

/* Receieve the xmodem packet */
uint8_t sample_test_xmodem_receive(int *fsm)
{
  xmodem_t xmo;

  xmodem_init(&xmo, NULL, NULL);

  while(xmo.cur_state != state_result)
    {
      /* Run the FSM */
       xmodem_run(&xmo);
    }

  console_flush();
  console_a_char();

  while(1)
    {
      printf("----- EOT %d\r\n", xmo.result);
    }
#if 0  
  /* Step1: Receieve one char */
  a_byte = uart_receive_data(CONFIG_DEBUG_UART);

  /* Step2: Send ACK */
  uart_send_data(CONFIG_DEBUG_UART, a_byte);

  /* Step:3 Receieve the one packet */

  /* Step4: Send ACK */

  /* Repeat Step 1 */

  /* Sender send EOT */

  /* Receiver Send ACK */
#endif
  return 0;
}
