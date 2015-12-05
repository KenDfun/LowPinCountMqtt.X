
#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "util_lib.h"

#include <stdio.h>
#include <string.h>

void disp_err(void)
{
  RC2=1;
}

void disp_err_forever(void)
{
  while(1){
    __delay_ms(250);
    RC2 ^= 1;
  }
}

void dump(unsigned char *buf,int len)
{
  char dispbuf[6];
  int i;

  EUSART_Str_Write((uint8_t *)"\r00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\r");
  EUSART_Str_Write((uint8_t *)"-----------------------------------------------\r");
  for(i=0;i<len;i++){
    sprintf(dispbuf,"%02x ",buf[i]);
    EUSART_Str_Write(dispbuf);
    if((i+1)%16 == 0){
      EUSART_Write('\r');
    }
  }
  EUSART_Write('\r');

}
