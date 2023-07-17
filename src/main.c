#include <stdio.h>
#include "../include/mqtt.h"
#include "../include/list.h"
#include "../include/linklist.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#include "../include/fan.h"
#include "../include/led.h"
#include "../include/tem_hum.h"
#include "../include/nixie_tube.h"

int main(int argc, char const *argv[])
{
    char sub_topic[32] = "sub_";
    char pub_topic[32] = "pub_";
    char id_clientid[32] = "id_";
    char randombuf[32] = {0};
    //genRandomString(randombuf, 10);
	strcpy(randombuf,"bznryxtjvf");
    strcat(sub_topic, randombuf);
    strcat(pub_topic, randombuf);
    strcat(id_clientid, randombuf);
    strcpy(serverbuf_subtopic, sub_topic);
    strcpy(serverbuf_pubtopic, pub_topic);
    strcpy(serverbuf_ID, id_clientid);

    //socket_server_init();

    printf("%s %s %s\n", serverbuf_subtopic, serverbuf_pubtopic, serverbuf_ID);
    //send(acceptfd, serverbuf_subtopic, sizeof(serverbuf_subtopic), 0);
    //usleep(300);
    //send(acceptfd, serverbuf_pubtopic, sizeof(serverbuf_pubtopic), 0);
    //usleep(300);
    //send(acceptfd, serverbuf_ID, sizeof(serverbuf_ID), 0);
    usleep(300);

    /* code */
    //初始化传感器节点
    struct linklist_sensor linklist_sensor_fan;

    // //初始化控制节点函数
    linklist_sensor_fan.func_control = fan_control;
    linklist_sensor_fan.func_control(NULL);

    //传感器节点加入链表当中
    linklist_sensor_init(&linklist_sensor_fan, "fan");
 	
	
	struct linklist_sensor linklist_sensor_led;

    //初始化控制节点函数
    linklist_sensor_led.func_control = led_control;
    linklist_sensor_led.func_control(NULL);

    //传感器节点加入链表当中
    linklist_sensor_init(&linklist_sensor_led, "lamp");

    //配置mqtt相关参数
    struct mqtt_default_config cfg;
    cfg.endpoint = "tcp://mqtt.yyzlab.com.cn:1883";
    cfg.username = "admin";
    cfg.password = "123";
    cfg.shadow = serverbuf_ID;//serverbuf_ID为clientid

    //订阅初始化
    mqtt_init(&cfg, 0);
    //订阅主题 serverbuf_subtopic

    mqtt_sub(&cfg.sub_client, serverbuf_subtopic);

    //发布初始化
    mqtt_init(&cfg, 1);

    //采集线程
    pthread_t pthread_pub_tem_hum;
    pthread_create(&pthread_pub_tem_hum, NULL, msg_pthread_pub_tem_hum, &cfg);
    pthread_t pthread_pub_fan;
    pthread_create(&pthread_pub_fan, NULL, msg_pthread_pub_fan, &cfg);

    // //数码管控制线程
    pthread_t pthread_sub_nixie_tube;
    memset(temperature_str, 0, sizeof(temperature_str));
    pthread_create(&pthread_sub_nixie_tube, NULL, msg_pthread_sub_nixie_tube, temperature_str);

    //传感器控制线程
    pthread_t pthread_sub;
    pthread_create(&pthread_sub, NULL, msg_pthread_sub, &linklist_sensor_fan);

	//采集线程
    pthread_t pthread_pub_led;
    pthread_create(&pthread_pub_led,NULL,msg_pthread_pub_led,&cfg);
 
    //传感器控制线程
    //pthread_t pthread_sub;
    //pthread_create(&pthread_sub,NULL,msg_pthread_sub,&linklist_sensor_led);


    while (1);
    return 0;
}
