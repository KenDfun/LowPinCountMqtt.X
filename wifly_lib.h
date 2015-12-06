#ifndef WIFLY_LIB_H_
#define WIFLY_LIB_H_

void EUSART_Str_Write(uint8_t *txData);
void EUSART_Data_Write(uint8_t *txData,int len);
void EUSART_Read_BufClear(void);


int wifly_open(const unsigned char *host,int port);
int wifly_sendPacketBuffer(unsigned char *buf, int len);
int wifly_close(void);
void wifly_command_mode(void);
int wifly_ready(void);

#define MAX_WIFLYBUF 200


#endif /* WIFLY_LIB_H_ */
