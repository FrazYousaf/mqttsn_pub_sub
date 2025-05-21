#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTTSNPacket.h"
#include "transport.h"

#define KEEP_ALIVE 60

int main(int argc, char** argv)
{
    int rc = 0;
    int mysock;
    unsigned char buf[200];
    int buflen = sizeof(buf);
    MQTTSN_topicid topic;
    unsigned char* payload = (argc > 4) ? (unsigned char*)argv[4] : (unsigned char*)"Hello my name is fraz yousaf";
    int payloadlen = strlen((char*)payload);
    char *host = "127.0.0.1";
    int port = 10000;
    char *topicname = "tt";
    MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
    options.duration = KEEP_ALIVE;

    mysock = transport_open();
    if(mysock < 0)
        return mysock;

    if (argc > 1) host = argv[1];
    if (argc > 2) port = atoi(argv[2]);
    if (argc > 3) 
        topicname = argv[3];

    printf("Sending to hostname %s port %d\n", host, port);

    /* Connect */
    options.clientID.cstring = "myclientid";
    int len = MQTTSNSerialize_connect(buf, buflen, &options);
    rc = transport_sendPacketBuffer(host, port, buf, len);

    /* Wait for CONNACK */
    if (MQTTSNPacket_read(buf, buflen, transport_getdata) == MQTTSN_CONNACK)
    {
        int connack_rc = -1;
        if (MQTTSNDeserialize_connack(&connack_rc, buf, buflen) != 1 || connack_rc != 0)
        {
            printf("Unable to connect, return code %d\n", connack_rc);
            goto exit;
        }
        else 
            printf("connected rc %d\n", connack_rc);
    }
    else
        goto exit;

    /* Publish */
    topic.type = MQTTSN_TOPIC_TYPE_SHORT;
    memcpy(topic.data.short_name, topicname, 2);
    len = MQTTSNSerialize_publish(buf, buflen, 0, 0, 0, 0, topic, payload, payloadlen);
    rc = transport_sendPacketBuffer(host, port, buf, len);
    printf("rc %d from send packet for publish length %d\n", rc, len);

    /* Graceful disconnect */
    len = MQTTSNSerialize_disconnect(buf, buflen, 0);
    transport_sendPacketBuffer(host, port, buf, len);

exit:
    transport_close();
    return 0;
}