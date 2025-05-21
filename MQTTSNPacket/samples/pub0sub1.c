#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "MQTTSNPacket.h"
#include "transport.h"

#define KEEP_ALIVE_INTERVAL 60  // seconds

int main(int argc, char** argv)
{
    int rc = 0;
    int mysock;
    unsigned char buf[200];
    int buflen = sizeof(buf);
    MQTTSN_topicid topic;
    int len = 0;
    unsigned char dup = 0;
    int qos = 1;
    unsigned char retained = 0;
    short packetid = 1;
    char *topicname = "tt";
    char clientID[50] = "pub0sub1";  // Default client ID
    char *host = "127.0.0.1";
    int port = 10000;
    MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
    options.duration = KEEP_ALIVE_INTERVAL;

    // Handle command line args
    if (argc > 1) host = argv[1];
    if (argc > 2) port = atoi(argv[2]);
    if (argc > 3) topicname = argv[3];
    if (argc > 4) snprintf(clientID, sizeof(clientID), "%s", argv[4]);

    printf("Sending to hostname %s port %d\n", host, port);
    printf("Client ID: %s\n", clientID);

    mysock = transport_open();
    if (mysock < 0) return mysock;

    // Connect
    options.clientID.cstring = clientID;
    len = MQTTSNSerialize_connect(buf, buflen, &options);
    rc = transport_sendPacketBuffer(host, port, buf, len);

    if (MQTTSNPacket_read(buf, buflen, transport_getdata) == MQTTSN_CONNACK)
    {
        int connack_rc = -1;
        if (MQTTSNDeserialize_connack(&connack_rc, buf, buflen) != 1 || connack_rc != 0)
        {
            printf("Unable to connect, return code %d\n", connack_rc);
            goto exit;
        }
        else
        {
            printf("Connected (rc=%d)\n", connack_rc);
        }
    }
    else
    {
        printf("Failed to receive CONNACK\n");
        goto exit;
    }

    // Subscribe
    topic.type = MQTTSN_TOPIC_TYPE_SHORT;
    memcpy(topic.data.short_name, topicname, 2);  // Use short topic name

    printf("Subscribing to topic: %s\n", topicname);
    len = MQTTSNSerialize_subscribe(buf, buflen, 0, qos, packetid, &topic);
    rc = transport_sendPacketBuffer(host, port, buf, len);

    // Handle SUBACK
    unsigned short topicid;
    if (MQTTSNPacket_read(buf, buflen, transport_getdata) == MQTTSN_SUBACK)
    {
        unsigned short submsgid;
        int granted_qos;
        unsigned char returncode;
        rc = MQTTSNDeserialize_suback(&granted_qos, &topicid, &submsgid, &returncode, buf, buflen);
        if (granted_qos != qos || returncode != 0)
        {
            printf("SUBACK error: qos=%d return code=%d\n", granted_qos, returncode);
            goto exit;
        }
        else
        {
            printf("Subscribed successfully. Topic ID: %d\n", topicid);
        }
    }
    else
    {
        printf("Did not receive SUBACK\n");
        goto exit;
    }

    // Setup for PINGREQ
    MQTTSNString ping_clientid = MQTTSNString_initializer;
    ping_clientid.cstring = clientID;

    printf("Waiting for messages...\n");
    struct timeval last_packet, now;
    gettimeofday(&last_packet, NULL);

    while (1)
    {
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(mysock, &readset);

        gettimeofday(&now, NULL);
        long elapsed = now.tv_sec - last_packet.tv_sec;
        long remaining = KEEP_ALIVE_INTERVAL - elapsed;

        struct timeval timeout = {
            .tv_sec = (remaining > 0) ? remaining : 0,
            .tv_usec = 0
        };

        int select_rc = select(mysock + 1, &readset, NULL, NULL, &timeout);
        if (select_rc > 0)
        {
            int msgType = MQTTSNPacket_read(buf, buflen, transport_getdata);
            if (msgType == MQTTSN_PUBLISH)
            {
                unsigned short packet_id;
                int msg_qos, payloadlen;
                unsigned char* payload;
                unsigned char msg_dup, msg_retained;
                MQTTSN_topicid pubtopic;

                if (MQTTSNDeserialize_publish(&msg_dup, &msg_qos, &msg_retained, &packet_id, &pubtopic,
                        &payload, &payloadlen, buf, buflen) == 1)
                {
                    if (pubtopic.type == MQTTSN_TOPIC_TYPE_SHORT)
                        printf("Topic (short): %c%c\n", pubtopic.data.short_name[0], pubtopic.data.short_name[1]);

                    printf("Message received: %.*s\n", payloadlen, payload);
                    gettimeofday(&last_packet, NULL);
                }
            }
            else if (msgType == MQTTSN_DISCONNECT)
            {
                printf("Received DISCONNECT from gateway/broker.\n");
                break;
            }
        }
        else if (select_rc == 0)
        {
            // Timeout: send PINGREQ
            printf("Keep-alive timeout, sending PINGREQ\n");
            len = MQTTSNSerialize_pingreq(buf, buflen, ping_clientid);
            transport_sendPacketBuffer(host, port, buf, len);
            gettimeofday(&last_packet, NULL);
        }
        else
        {
            perror("select error");
            break;
        }
    }

exit:
    transport_close();
    printf("Client exiting.\n");
    return 0;
}
