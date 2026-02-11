#ifndef _X_MODEM_H
#define _X_MODEM_H
#include <stdint.h>
#include <stdio.h>

/* FSM State */
typedef enum xmodem_state
{
  state_init = 0,
  state_cmd,
  state_128B,
  state_1024B,
  state_eot,
  state_result
} xmodem_state_t;
typedef struct xmodem xmodem_t;

typedef uint8_t (*xmo_cb_fptr_t) (void *arg, uint8_t *buff, uint16_t sz);

/* X Modem context */
typedef struct xmodem
{
  /* XMODEM states */
  xmodem_state_t cur_state;

  /* xmodem next packet number */
  uint8_t next_packet;

  /* Result */
  uint8_t result;

  /* Callback */
  xmo_cb_fptr_t cb;
  /* Callback argument */
  void *cb_arg;
}xmodem_t;

#define XM_SUCCESS 0
#define XM_F_CRC   2
#define XM_F_PKT   4
#define XM_F_CMP   8

uint8_t  sample_test_xmodem_receive(int *fsm);
void xmodem_init(xmodem_t *xmo, xmo_cb_fptr_t cbptr, void *arg);
xmodem_state_t xmodem_run(xmodem_t *xmo);
#endif
