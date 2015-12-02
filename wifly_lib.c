#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "wifly_lib.h"
#include <stdio.h>
#include <string.h>

#define _XTAL_FREQ 16000000          //  __delay_ms(), __delay_us()用

uint8_t WiflyBuf[MAX_WIFLYBUF];


int wifly_open(char *host,int port)
{
  WiflyWriteStr("open ");
  WiflyWriteStr(host);
  sprintf(WiflyBuf," %d\r",port);
  WiflyWriteStr(WiflyBuf);

  return 1;
}

int wifly_sendPacketBuffer(unsigned char *buf, int len)
{
  EUSART_Data_Write((uint8_t *)buf,len);
  return 1;
}

int wifly_close()
{
  wifly_command_mode();
  WiflyWriteStr("close\r");
}

void wifly_command_mode(void)
{
  static unsigned char cmdChkBuf[4];

  EUSART_Read_BufClear();
  WiflyWriteStr("$$$");
  __delay_ms(250);
  cmdChkBuf[0]=(unsigned char)EUSART_Read();
  cmdChkBuf[1]=(unsigned char)EUSART_Read();
  cmdChkBuf[2]=(unsigned char)EUSART_Read();
  cmdChkBuf[3]='\0';
  if(strncmp(cmdChkBuf,"CMD",3)!=0){
    RC2=1;
    while(1);
  }
}
