#ifndef WIFLY_LIB_H_
#define WIFLY_LIB_H_

int wifly_open(char *host,int port);
int wifly_sendPacketBuffer(unsigned char *buf, int len);
int wifly_close(void);


#endif /* WIFLY_LIB_H_ */
