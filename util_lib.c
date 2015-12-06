
#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "util_lib.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void json_decode(unsigned char *buf,int payloadlen_in,uint8_t *arry)
{
  if(buf[1]=='0'){
    arry[0]=0;
  }else if(buf[1]=='1'){
    arry[0]=1;
  }else{
    arry[0]=1;
  }

  if(buf[3]=='0'){
    arry[1]=0;
  }else if(buf[3]=='1'){
    arry[1]=1;
  }else{
    arry[1]=1;
  }

  if(buf[5]=='0'){
    arry[2]=0;
  }else if(buf[5]=='1'){
    arry[2]=1;
  }else{
    arry[2]=1;
  }
}

static uint8_t LedDisp[3];
void led_init(void)
{
  LedDisp[0]=0;
  LedDisp[1]=0;
  LedDisp[2]=0;
  led_show();
}

void led_update(uint8_t *arry)
{
  LedDisp[0] = arry[0];
  LedDisp[1] = arry[1];
  LedDisp[2] = arry[2];
  led_show();
}

void led_show(void)
{
  RC1 = LedDisp[0];
  RC2 = LedDisp[1];
  RC3 = LedDisp[2];
}
