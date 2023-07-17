#include <stdio.h>
#include <stdlib.h>
#include <MQTTClient.h>
#include <MQTTClientPersistence.h>
#include "../include/mqtt.h"
#include "../include/cJSON.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../include/linklist.h"

#include "../include/fan.h"
#include "../include/tem_hum.h"
#include "../include/nixie_tube.h"
#include "../include/led.h"



static void delivered(void *context, MQTTClient_deliveryToken dt)
{
	volatile MQTTClient_deliveryToken deliveredtoken;

	 printf("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}
//回调解析加入消息队列
static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{

	//printf("msgarrvd\n");
	char *payloadptr;

	payloadptr = message->payload;
	
	char * local_payload = malloc(message->payloadlen+1);
	memset(local_payload,0,message->payloadlen);
	strncpy(local_payload,message->payload,message->payloadlen);

	#if 1
	cJSON *root = NULL;
	root = cJSON_Parse(local_payload);
	if (!root)
	{
		printf("json format is error.\n");
		return -1;
	}
	// printf("%s\n",cJSON_Print(root));

	struct msg_queue msg_queue;

	int i = 0;
	int isize = 0;

	cJSON *item_array_sensor = NULL;
	// cJSON *item_array_cmdbuf = NULL;
	isize = cJSON_GetArraySize(root);
	//printf("isize:%d\n",isize);
	//获取sensor名字
	item_array_sensor = cJSON_GetArrayItem(root, i);
	msg_queue.name = item_array_sensor->string;

	// item_array_cmdbuf = cJSON_GetArrayItem(root,i);
	char *cmdbuf = NULL;
	cmdbuf = cJSON_GetObjectItem(root, item_array_sensor->string)->valuestring;

	int cmd = 0;
	cmd = cJSON_GetObjectItem(root, item_array_sensor->string)->valueint;

	//获取id
	int id =0;
	if(isize == 2)
	{
		cJSON *item_array_id = NULL;
		item_array_id = cJSON_GetArrayItem(root, i + 1);
		id = cJSON_GetObjectItem(root, item_array_id->string)->valueint;
	}

	msg_queue.cmd = cmd;
	msg_queue.id = id;
	msg_queue.cmdbuf = cmdbuf;
	msg_queue.flag = 0;


	key_t key = ftok("msg_queue", 'k');
	int msgid = msgget(key, 0666 | IPC_CREAT);
	msg_queue.mtype = 1;

	// printf("cmd:%d id:%d cmdbuf:%s flag:%d mytype:%d\n",cmd,id,cmdbuf,msg_queue.flag,msg_queue.mtype );

	msgsnd(msgid, &msg_queue, sizeof(struct msg_queue), 0);
#endif

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	free(local_payload);

	return 1;

}

static void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("cause: %s\n", cause);
}

void mqtt_stop()
{
	return;
}

int mqtt_init(struct mqtt_default_config *cfg, int flag)
{
	MQTTClient client; //定义一个MQTT客户端
	MQTTClient_connectOptions connect_options = MQTTClient_connectOptions_initializer;
	char client_id[32] = {0};
	int rc;

	//初始化MQTT客户端
	if (flag == PUB_CLIENT)
	{
		sprintf(client_id, "pub_%s", cfg->shadow);
	}
	else
	{
		sprintf(client_id, "sub_%s", cfg->shadow);
	}

	if ((rc = MQTTClient_create(&client, cfg->endpoint, client_id,
								MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to create client, return code %d\n", rc);
		return (EXIT_FAILURE);
	}

	connect_options.keepAliveInterval = 20;
	connect_options.cleansession = 1;

	if (cfg->username != NULL && cfg->password != NULL)
	{
		connect_options.username = cfg->username;
		connect_options.password = cfg->password;
	}

	if (flag == SUB_CLIENT)
	{
		if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd,
										  delivered)) != MQTTCLIENT_SUCCESS)
		{
			printf("Failed to set callbacks, return code %d\n", rc);
			return (EXIT_FAILURE);
		}
	}

	//连接MQTT服务器
	if ((rc = MQTTClient_connect(client, &connect_options)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		return (EXIT_FAILURE);
	}

	if (flag == PUB_CLIENT)
	{
		cfg->pub_client = client;
	}
	else
	{
		cfg->sub_client = client;
	}

	return 0;
}

int mqtt_sub(MQTTClient *client, char *topic)
{
	int rc;

	printf("Subscribing to topic: %s\n", topic);

	//订阅主题
	if ((rc = MQTTClient_subscribe(*client, topic, QOS)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}
	return rc;
}

#define TIMEOUT 10000L
int mqtt_pub(MQTTClient *client, char *buf, char *topic)
{
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	char data[1024];
	int rc;

	if (client == NULL)
	{
		printf("mqtt client not init.\n");
		return (EXIT_FAILURE);
	}

	pubmsg.payload = buf;
	pubmsg.payloadlen = strlen(buf);
	pubmsg.qos = QOS;
	pubmsg.retained = 0;

	if ((rc = MQTTClient_publishMessage(*client, topic,
										&pubmsg, &token)) != MQTTCLIENT_SUCCESS)
	{
		// printf("Failed to publish message, return code %d\n", rc);
		return (EXIT_FAILURE);
	}

	// debug("Waiting for up to %d seconds for publication of %s\n"
	//		"on topic %s \n", (int)(TIMEOUT/1000), data, topic);
	rc = MQTTClient_waitForCompletion(*client, token, TIMEOUT);
	// debug("Message with delivery token %d delivered\n", token);

	return 0;
}

void *msg_pthread_sub(void *linklist_sensor_pos)
{
	// printf("msg_pthread_sub\n");
	key_t key = ftok("msg_queue", 'k');
	int msgid;
	struct msg_queue msg_queue;
	while (1)
	{
		msgid = msgget(key, 0666 | IPC_CREAT);
		// printf("msg_queue.name:%s\n",msg_queue.name);
		msgrcv(msgid, &msg_queue, sizeof(struct msg_queue), 1, 0);
		//printf("%s:%d id:%d cmdbuf:%s flag:%d\n",msg_queue.name,msg_queue.cmd,msg_queue.id,msg_queue.cmdbuf,msg_queue.flag);
		search_sensor_control(linklist_sensor_pos, &msg_queue);
	}
}

void *msg_pthread_sub_nixie_tube(void *cmdbuf)
{
	while(1)
	{
		nixie_tube_control(cmdbuf);
	}
}
void *msg_pthread_pub_fan(void *arg)
{
	while (1)
	{
		fan_collect((struct mqtt_default_config *)arg);
		sleep(1);
	}
}

void *msg_pthread_pub_led(void *arg)
{
	// socket_server_init();
	while (1)
	{
		led_collect((struct mqtt_default_config *)arg);
		sleep(1);
	}
	close(acceptfd);
	close(sockfd);
}


void *msg_pthread_pub_tem_hum(void *arg)
{
	while (1)
	{
		tem_hum_collect((struct mqtt_default_config *)arg);
		sleep(1);
	}
}

void socket_server_init(void)
{
	//创建套接字
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket\n");
	}
	//填充结构体
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	serverlen = sizeof(serveraddr);
	//绑定
	if (bind(sockfd, (struct sockaddr *)&serveraddr, serverlen) < 0)
	{
		perror("bind\n");
	}
	//监听
	if (listen(sockfd, 1) < 0)
	{
		perror("listen\n");
	}
	//连接
	acceptfd = accept(sockfd, NULL, NULL);
	if (acceptfd < 0)
	{
		perror("accept\n");
	}
	// printf("accept success\n");
}

char* genRandomString(char string[],int length)
{
    int  i;
    srand((int) time(0 ));
    for (i = 0; i < length-1; i++)
    {
        string[i] = 'a' + rand() % 26;
    }
    string[length - 1] = '\0';
    return string;
}


