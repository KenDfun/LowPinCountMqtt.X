#include "mcc_generated_files/mcc.h"
#include <stdio.h>
#include <string.h>


int wifly_open(char *host,int port)
{
  EUSART_str_Write((uint8_t *)"open ");
  EUSART_str_Write((uint8_t *)host);
  sprintf(buf," %d\r",port);

  return 1;
}


rc = wifly_sendPacketBuffer(host, port, buf, len);
wifly_close();
