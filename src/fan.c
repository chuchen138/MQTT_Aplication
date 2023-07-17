#include "../include/fan.h"
#include "../include/cJSON.h"
#include <fcntl.h>

void fan_collect(struct mqtt_default_config *cfg)
{
    // printf("collect data fan\n");
    int fd;
    fd = open("/sys/class/hwmon/hwmon1/pwm1", O_RDWR);
    if (fd < 0)
    {
        perror("open fan  error\n");
    }
    char readbuf[16] = {"\0"};

    int ret = read(fd, readbuf, sizeof(readbuf));
    if (ret < 0)
    {
        perror("read fan  error\n");
    }

    cJSON *root = NULL;
    root = cJSON_CreateObject();

    if (readbuf != NULL)
    {
        if (readbuf[0] == '0')
        {
            cJSON_AddBoolToObject(root, "fan", cJSON_False);
            cJSON_AddNumberToObject(root, "id", 0);
            char *buf = cJSON_PrintUnformatted(root);
            mqtt_pub(&cfg->pub_client, buf, serverbuf_pubtopic);
        }
        else
        {
            cJSON_AddBoolToObject(root, "fan", cJSON_True);
            cJSON_AddNumberToObject(root, "id", 0);
            char *buf = cJSON_PrintUnformatted(root);
            mqtt_pub(&cfg->pub_client, buf, serverbuf_pubtopic);
        }
    }
    close(fd);
}

void fan_control(struct msg_queue *msg_queue)
{
    if (msg_queue != NULL)
    {
        // printf("control fan:%d\n", msg_queue->cmd);
        int fd;
        fd = open("/sys/class/hwmon/hwmon1/pwm1", O_RDWR);
        if (fd < 0)
        {
            perror("open fan  error\n");
        }
        if (msg_queue->cmd == 1) //开
        {
            char *buf_on = "255";
            int ret = write(fd, buf_on, sizeof(buf_on));
            if (ret < 0)
            {
                perror("write to open fan error\n");
            }
            if (fan_cmd_flag != msg_queue->cmd)
            {
                fan_cmd_flag = msg_queue->cmd;
                memset(serverbuf, 0, sizeof(serverbuf));
                strcpy(serverbuf, "fan:on");
                send(acceptfd, serverbuf, sizeof(serverbuf), 0);
            }
        }
        else if (msg_queue->cmd == 0)
        {
            char *buf_off = "0";
            int ret = write(fd, buf_off, sizeof(buf_off));
            if (ret < 0)
            {
                perror("write to close fan error\n");
            }
            if (fan_cmd_flag != msg_queue->cmd)
            {
                fan_cmd_flag = msg_queue->cmd;
                memset(serverbuf, 0, sizeof(serverbuf));
                strcpy(serverbuf, "fan:off");
                send(acceptfd, serverbuf, sizeof(serverbuf), 0);
            }
        }
        close(fd);
    }
}
