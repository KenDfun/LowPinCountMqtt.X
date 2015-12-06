#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "wifly_lib.h"
#include "util_lib.h"
#include "MQTTPacket.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern uint8_t WiflyBuf[MAX_WIFLYBUF];

int transport_getdata(unsigned char* buf, int len);

int mqtt_connect(void)
{
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 0;
  unsigned char *buf = (unsigned char *)WiflyBuf;
	int buflen = sizeof(WiflyBuf);
	int len = 0;

//	data.clientID.cstring = "me";
	// data.keepAliveInterval = 20;
	// data.cleansession = 1;
	// data.username.cstring = "testuser";
	// data.password.cstring = "testpassword";

	len = MQTTSerialize_connect(buf, buflen, &data);
	EUSART_Str_Write((uint8_t *)"\rconnect");
  // dump(buf,len);

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

  return 1;
}

int mqtt_subscribe(void)
{
	unsigned char *buf = (unsigned char *)WiflyBuf;
	int buflen = sizeof(WiflyBuf);
	MQTTString topicString = MQTTString_initializer;
	int msgid = 1;
	int req_qos = 0;
	int len = 0;
	int rc = 0;

  /* subscribe */
	topicString.cstring = (char *)"design-fun.com/led";
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

	EUSART_Str_Write((uint8_t *)"\rsubscribe");
	// dump(buf,len);
	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 0)
		{
			disp_err_forever();
		}
	}
	else{
		disp_err_forever();
	}

	return 1;
}

int mqtt_get_msg(void)
{
	unsigned char dispbuf[40];
	unsigned char *buf = (unsigned char *)WiflyBuf;
	int buflen = sizeof(WiflyBuf);
	int rc = 0;

  EUSART_Str_Write((uint8_t *)"\rwaitmsg\n");

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
		int i;
		uint8_t arry[3];

		rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
				&payload_in, &payloadlen_in, buf, buflen);
		// sprintf(dispbuf,"msg: %.*s\r", payloadlen_in, payload_in);
		// EUSART_Str_Write((uint8_t *)dispbuf);
		json_decode(payload_in,payloadlen_in,arry);
		for(i=0;i<3;i++){
			sprintf(dispbuf,"led[%d]:%d\r", i, arry[i]);
			EUSART_Str_Write((uint8_t *)dispbuf);
		}
    led_update(arry);
	}

	// EUSART_Str_Write((uint8_t *)"disconnecting\r");
	// len = MQTTSerialize_disconnect(buf, buflen);
	// rc = transport_sendPacketBuffer(mysock, buf, len);

	return 1;
}

int transport_getdata(unsigned char* buf, int len)
{
  int i;

  for(i=0;i<len;i++){
    buf[i]=(unsigned char)EUSART_Read();
  }

  return i;
}
