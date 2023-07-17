#include "../include/led.h"
#include "../include/cJSON.h"

#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>

#if 1

void led_collect(struct mqtt_default_config *cfg)
{
    // printf("collect data led\n");
    cJSON *root1 = NULL;
    root1 = cJSON_CreateObject();

    int fd1;
    int fd;

    int readbuf;
    char readbuf1[16] = {"\0"};

    fd = open("/dev/led", O_RDWR);
    if (fd < 0)
    {
        //perror("open");
        fd1 = open("/sys/class/leds/user1/brightness", O_RDWR);
        if (fd1 < 0)
        {
            perror("open leds1  error\n");
        }
        int ret1 = read(fd1, readbuf1, sizeof(readbuf1));
        if (ret1 < 0)
        {
            perror("read led1  error\n");
        }

        if (readbuf1 != NULL)
        {
            if (readbuf1[0] == '0')
            {
                cJSON_AddBoolToObject(root1, "lamp", cJSON_True);
                cJSON_AddNumberToObject(root1, "id", 0);
                char *buf = cJSON_PrintUnformatted(root1);
                // printf("out:%s\n",buf);
                mqtt_pub(&cfg->pub_client, buf, serverbuf_pubtopic);
                strcpy(serverbuf, "1");
                // printf("serverbuf:1\n");
                send(acceptfd, serverbuf, sizeof(serverbuf), 0);
            }

            else
            {
                cJSON_AddBoolToObject(root1, "lamp", cJSON_False);
                cJSON_AddNumberToObject(root1, "id", 0);
                char *buf = cJSON_PrintUnformatted(root1);
                // printf("out:%s\n",buf);
                mqtt_pub(&cfg->pub_client, buf, serverbuf_pubtopic);
                strcpy(serverbuf, "0");
                // printf("serverbuf:0\n");
                send(acceptfd, serverbuf, sizeof(serverbuf), 0);
            }
        }
        close(fd1);
    }
    else
    {
        int ret = read(fd, &readbuf, 4);
        if (ret < 0)
        {
            perror("read");
        }
        if (readbuf == 1)
        {
            cJSON_AddBoolToObject(root1, "lamp", cJSON_True);
            cJSON_AddNumberToObject(root1, "id", 0);
            char *buf = cJSON_PrintUnformatted(root1);
            // printf("out:%s\n",buf);
            mqtt_pub(&cfg->pub_client, buf, serverbuf_pubtopic);
            strcpy(serverbuf, "1");
            // printf("serverbuf:1\n");
            send(acceptfd, serverbuf, sizeof(serverbuf), 0);
        }
        else if (readbuf == 0)
        {
            cJSON_AddBoolToObject(root1, "lamp", cJSON_False);
            cJSON_AddNumberToObject(root1, "id", 0);
            char *buf = cJSON_PrintUnformatted(root1);
            // printf("out:%s\n",buf);
            mqtt_pub(&cfg->pub_client, buf, serverbuf_pubtopic);
            strcpy(serverbuf, "0");
            // printf("serverbuf:0\n");
            send(acceptfd, serverbuf, sizeof(serverbuf), 0);
        }

        close(fd);
    }
}

void led_control(struct msg_queue *msg_queue)
{
    if (msg_queue != NULL)
    {
        int fd, on;
        fd = open("/dev/led", O_RDWR);
        if (fd < 0)
        {
            // printf("control led:%d id:%d\n", cmd, id);
            int fd1[5];
            if (msg_queue->id == 0)
            {
                fd1[0] = open("/sys/class/leds/user1/brightness", O_RDWR);
                fd1[1] = open("/sys/class/leds/user2/brightness", O_RDWR);
                fd1[2] = open("/sys/class/leds/led1/brightness", O_RDWR);
                fd1[3] = open("/sys/class/leds/led2/brightness", O_RDWR);
                fd1[4] = open("/sys/class/leds/led3/brightness", O_RDWR);
                if (fd1[4] < 0)
                {
                    perror("open leds1  error\n");
                }
                if (msg_queue->cmd == 1) //开
                {
                    char *buf = "0";
					for(int i = 0; i <5; ++i)
					{
					int ret = write(fd1[i], buf, sizeof(buf));
                    if (ret < 0)
                    {
                        perror("write to open led1 error\n");
                    }
					}
                }
                else if (msg_queue->cmd == 0)
                {
                    char *buf = "1";
					for(int i = 0; i <5; ++i)
					{
					int ret = write(fd1[i], buf, sizeof(buf));
                    if (ret < 0)
                    {
                        perror("write to close led1 error\n");
                    }
					}
                }
				for(int i = 0; i < 5; ++i)
                close(fd1[i]);
            }
            
        }
        else
        {
            if (msg_queue->cmd == 1)
            {
                on = 1;
                ioctl(fd, LED3_CMD, &on);
                printf("on = 1 \n");
                close(fd);
            }
            else if (msg_queue->cmd == 0)
            {
                on = 0;
                ioctl(fd, LED3_CMD, &on);
                printf("on = 0 \n");
                close(fd);
            }
        }
    }
}

#endif
