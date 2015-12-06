#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "wifly_lib.h"
#include "util_lib.h"
#include "MQTTPacket.h"

#include <stdio.h>
#include <string.h>



#define WiflyWriteStr(a) EUSART_Str_Write((uint8_t *)a)
uint8_t WiflyBuf[MAX_WIFLYBUF];

int wifly_wait_response(const unsigned char *rep);
int wifly_wait_star(void);


int wifly_open(const unsigned char *host,int port)
{
  WiflyWriteStr("open ");
  WiflyWriteStr(host);
  sprintf(WiflyBuf," %d\r",port);
  WiflyWriteStr(WiflyBuf);
  wifly_wait_star();
  if(!wifly_wait_response("OPEN*")){
    disp_err_forever();
  }

  return 1;
}

int wifly_sendPacketBuffer(unsigned char *buf, int len)
{
  EUSART_Data_Write((uint8_t *)buf,len);
  return 1;
}

int wifly_close(void)
{
  wifly_command_mode();
  WiflyWriteStr("close\r");
  wifly_wait_star();
  if(!wifly_wait_response("CLOS*")){
    disp_err_forever();
  }

  return 1;
}

int wifly_ready(void)
{
  wifly_wait_star();
  if(!wifly_wait_response("READY*")){
    disp_err_forever();
  }

}


void wifly_command_mode(void)
{
  static unsigned char cmdChkBuf[4];

  // WiflyWriteStr("test\r\n\r");

  EUSART_Read_BufClear();
  __delay_ms(250);
  WiflyWriteStr("$$$");
  __delay_ms(250);
  if(!wifly_wait_response("CMD")){
    disp_err_forever();
  }
}


int wifly_wait_response(const unsigned char *rep)
{
  int i=0;
  unsigned char chk;

  while(rep[i]!='\0'){
    chk = (unsigned char)EUSART_Read();
    // RC3 ^= 1;
    // EUSART_Write(chk);
    if(chk!=rep[i++]){
      disp_err_forever();
      sprintf(WiflyBuf,"ERR 0x%x\r",chk);
      WiflyWriteStr(WiflyBuf);
      return 0;
    }
  }

  return 1;
}

int wifly_wait_star(void)
{
  unsigned char chk;

  do{
    chk = (unsigned char)EUSART_Read();
    // EUSART_Write(chk);
  }while(chk!='*');
}
