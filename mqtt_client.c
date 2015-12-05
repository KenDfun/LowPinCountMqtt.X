#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MQTTSNPacket.h"

extern uint8_t WiflyBuf[];

int mqtt_pub(void)
{
	int buflen = sizeof(WiflyBuf);
	MQTTSN_topicid topic;
	unsigned char* payload = (unsigned char*)"payload";
	int payloadlen = strlen((char*)payload);
	int len = 0;
	unsigned char dup = 0;
	int qos = 1;
	unsigned char retained = 0;
	short packetid = 1;
	char *topicname = "WIFLYIOT";
	char *host = "127.0.0.1";
	int port = 1883;
	MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
	unsigned short topicid;

	options.clientID.cstring = "MQTT-SN";
	len = MQTTSNSerialize_connect(WiflyBuf, buflen, &options);
  dump(WiflyBuf,len);
	// wifly_sendPacketBuffer(WiflyBuf, len);

	/* wait for connack */
/*
	if (MQTTSNPacket_read(buf, buflen, transport_getdata) == MQTTSN_CONNACK)
	{
		int connack_rc = -1;

		if (MQTTSNDeserialize_connack(&connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
			// printf("Unable to connect, return code %d\n", connack_rc);
      disp_err();
			goto exit;
		}
		else
      RC3=0;
			// printf("connected rc %d\n", connack_rc);
	}
	else
		goto exit;

*/

  /* subscribe */
/*
	// printf("Subscribing\n");
	topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
	topic.data.long_.name = topicname;
	topic.data.long_.len = strlen(topic.data.long_.name);
	len = MQTTSNSerialize_subscribe(buf, buflen, 0, 2, packetid, &topic);
	rc = wifly_sendPacketBuffer(host, port, buf, len);

	if (MQTTSNPacket_read(buf, buflen, transport_getdata) == MQTTSN_SUBACK) 	// wait for suback
	{
		unsigned short submsgid;
		int granted_qos;
		unsigned char returncode;

		rc = MQTTSNDeserialize_suback(&granted_qos, &topicid, &submsgid, &returncode, buf, buflen);
		if (granted_qos != 2 || returncode != 0)
		{
			// printf("granted qos != 2, %d return code %d\n", granted_qos, returncode);
      disp_err();
			goto exit;
		}
		else
    {
      RC1=1;
      // printf("suback topic id %d\n", topicid);
    }
	}
	else
		goto exit;

exit:
	wifly_close();
*/
	return 0;

}
