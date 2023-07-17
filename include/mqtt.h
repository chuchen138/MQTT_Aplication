#ifndef MQTT_H
#define MQTT_H

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <MQTTClient.h>
#include <MQTTClientPersistence.h>
//#include <config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SUB_CLIENT	0
#define PUB_CLIENT	1

#define QOS         0



int flag;
int flag_temp;
int sockfd;
int acceptfd;
struct sockaddr_in serveraddr;
socklen_t serverlen;
char serverbuf[8];
int recvbytes;
int recvbytes1;
char server_sendbuf[32];

char serverbuf_all[32];
char serverbuf_subtopic[32];
char serverbuf_pubtopic[32];
char serverbuf_ID[32];


struct mqtt_default_config {
    char *endpoint;
    char *username;
    char *password;
    char *shadow;
    MQTTClient pub_client;
    MQTTClient sub_client;
};

struct msg_queue
{
    long mtype ;
    char *name;
    int cmd;
    int id;
    char *cmdbuf;
    int flag;
};


void *msg_pthread_sub(void *arg);
void *msg_pthread_sub_nixie_tube(void *arg);

void *msg_pthread_pub_fan(void *arg);
void *msg_pthread_pub_tem_hum(void *arg);
void *msg_pthread_pub_led(void *arg);


extern int mqtt_init(struct mqtt_default_config* cfg, int flag);
extern int mqtt_pub(MQTTClient *client, char *buf, char *topic);
extern int mqtt_sub(MQTTClient *client, char *topic);
void socket_server_init(void);
char* genRandomString(char string[],int length);
#endif // MQTT_H


