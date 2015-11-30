#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MQTTSNPacket.h"


int mqtt_pub(void)
{
	unsigned char buf[200];
	int buflen = sizeof(buf);
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

	wifly_open(host,port);
	// printf("Sending to hostname %s port %d\n", host, port);

	options.clientID.cstring = "MQTT-SN";
	len = MQTTSNSerialize_connect(buf, buflen, &options);
	wifly_sendPacketBuffer(host, port, buf, len);

	/* wait for connack */
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



	// printf("Publishing\n");
	/* publish with short name */
	topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
	topic.data.id = topicid;
	++packetid;
	len = MQTTSNSerialize_publish(buf, buflen, dup, qos, retained, packetid,
			topic, payload, payloadlen);
	rc = transport_sendPacketBuffer(host, port, buf, len);

	/* wait for puback */
	if (MQTTSNPacket_read(buf, buflen, transport_getdata) == MQTTSN_PUBACK)
	{
		unsigned short packet_id, topic_id;
		unsigned char returncode;

		if (MQTTSNDeserialize_puback(&topic_id, &packet_id, &returncode, buf, buflen) != 1 || returncode != MQTTSN_RC_ACCEPTED)
			printf("Unable to publish, return code %d\n", returncode);
		else
			printf("puback received, msgid %d topic id %d\n", packet_id, topic_id);
	}
	else
		goto exit;

	printf("Receive publish\n");
	if (MQTTSNPacket_read(buf, buflen, transport_getdata) == MQTTSN_PUBLISH)
	{
		unsigned short packet_id;
		int qos, payloadlen;
		unsigned char* payload;
		unsigned char dup, retained;
		MQTTSN_topicid pubtopic;

		if (MQTTSNDeserialize_publish(&dup, &qos, &retained, &packet_id, &pubtopic,
				&payload, &payloadlen, buf, buflen) != 1)
			printf("Error deserializing publish\n");
		else
			printf("publish received, id %d qos %d\n", packet_id, qos);

		if (qos == 1)
		{
			len = MQTTSNSerialize_puback(buf, buflen, pubtopic.data.id, packet_id, MQTTSN_RC_ACCEPTED);
			rc = transport_sendPacketBuffer(host, port, buf, len);
			if (rc == 0)
				printf("puback sent\n");
		}
	}
	else
		goto exit;

	len = MQTTSNSerialize_disconnect(buf, buflen, 0);
	rc = transport_sendPacketBuffer(host, port, buf, len);

exit:
	wifly_close();

	return 0;
}

void disp_err(voie)
{
  RC2=1;
}
