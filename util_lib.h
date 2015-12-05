#ifndef UTIL_LIB_H_
#define UTIL_LIB_H_

#include <xc.h>

#define _XTAL_FREQ 16000000          //  __delay_ms(), __delay_us()用


void disp_err(void);
void disp_err_forever(void);
void dump(unsigned char *buf,int len);

void EUSART_Str_Write(uint8_t *txData);



#endif /* UTIL_LIB_H_ */
