#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "wifly_lib.h"
#include "util_lib.h"
#include "MQTTPacket.h"

#include <stdio.h>
#include <string.h>



#define MAX_WIFLYBUF 200
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

void wifly_command_mode(void)
{
  static unsigned char cmdChkBuf[4];

  EUSART_Read_BufClear();
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
    EUSART_Write(chk);
    if(chk!=rep[i++]){
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
    EUSART_Write(chk);
  }while(chk!='*');
}

int transport_getdata(unsigned char* buf, int len);

int mqtt_pub(void)
{
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 0;
	int mysock = 0;
  unsigned char *buf = (unsigned char *)WiflyBuf;
	int buflen = sizeof(WiflyBuf);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	char* payload = "mypayload";
	int payloadlen = strlen(payload);
	int len = 0;
	int port = 1883;

//	data.clientID.cstring = "me";
	// data.keepAliveInterval = 20;
	// data.cleansession = 1;
	// data.username.cstring = "testuser";
	// data.password.cstring = "testpassword";

	len = MQTTSerialize_connect(buf, buflen, &data);
  dump(WiflyBuf,len);
  /* wait for connack */
	if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
			// printf("Unable to connect, return code %d\n", connack_rc);
      disp_err_forever();
		}
	}
	else{
    disp_err_forever();
  }
  /* loop getting msgs on subscribed topic */
	topicString.cstring = "pubtopic";
	while (!toStop)
	{
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			int payloadlen_in;
			unsigned char* payload_in;
			int rc;
			MQTTString receivedTopic;

			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
			printf("message arrived %.*s\n", payloadlen_in, payload_in);
		}

		printf("publishing reading\n");
		len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
		rc = transport_sendPacketBuffer(mysock, buf, len);
	}

	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(mysock, buf, len);

exit:
	transport_close(mysock);

	return 0;

}

int transport_getdata(unsigned char* buf, int len)
{
  int i;

  for(i=0;i<len;i++){
    buf[i]=(unsigned char)EUSART_Read();
  }

  return i;
}
