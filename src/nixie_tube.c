#include "../include/nixie_tube.h"
#include "../include/cJSON.h"

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#if 1

static void pabort(const char *s)
{
    perror(s);
    abort();
}

void nixie_tube_collect(struct mqtt_default_config *cfg)
{
    if (nixie_tube != NULL)
    {
        cJSON *root = NULL;
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "nixie_tube", nixie_tube);
        cJSON_AddNumberToObject(root, "id", 0);
        char *buf = cJSON_PrintUnformatted(root);
        // printf("out:%s\n",buf);
        mqtt_pub(&cfg->pub_client, buf, "pub_topic");
    }
}

void nixie_tube_control(char *cmdbuf)
{

        static const char *device = "/dev/spidev0.0";
        static uint8_t mode;
        static uint8_t bits = 8;
        static uint32_t speed = 400000;
        static uint16_t delay = 0;

        int ret = 0;
        int fd;

        fd = open(device, O_RDWR); //打开设备文件
        if (fd < 0)
            pabort("can't open device");

        /*
         * spi mode //设置spi设备模式
         */
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode); //写模式
        if (ret == -1)
            pabort("can't set spi mode");

        /*
         * bits per word    //设置每个字含多少位
         */
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits); //写每个字含多少位
        if (ret == -1)
            pabort("can't set bits per word");

        /*
         * max speed hz     //设置速率
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed); //写速率
        if (ret == -1)
            pabort("can't set max speed hz");

        //打印模式,每字多少位和速率信息
        // printf("spi mode: %d\n", mode);
        // printf("bits per word: %d\n", bits);
        // printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);

        // transfer(fd);   //传输测试
        flag = 1;
        unsigned char num[20] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
        unsigned char buf[2];
        while (flag != flag_temp)
        {
            int pos = 0;
            int n = 0;
            int i = 0;

            // printf("cmdbuf%s\n", cmdbuf);
            if (cmdbuf != NULL)
            {
                char cmdbuf_temp[8];
                strcpy(cmdbuf_temp, cmdbuf);
                strcpy(nixie_tube, cmdbuf);
                // printf("%d\n",(strlen(cmdbuf_temp)) );

                for (i = 0; i < strlen(cmdbuf_temp);)
                {
                    if (n == 1)
                    {
                        pos = 1 << (i - 1);
                        buf[0] = pos;
                        buf[1] = num[cmdbuf_temp[i] - '0'];
                        if (cmdbuf_temp[i] == 'a' || cmdbuf_temp[i] == 'b' || cmdbuf_temp[i] == 'c' || cmdbuf_temp[i] == 'd' || cmdbuf_temp[i] == 'e' || cmdbuf_temp[i] == 'f')
                        {
                            buf[1] = num[cmdbuf_temp[i] - 87];
                        }
                        if (write(fd, buf, 2) < 0)
                        {
                            perror("write\n");
                        }
                        usleep(3000);
                    }
                    else
                    {
                        pos = 1 << i;
                        buf[0] = pos;
                        buf[1] = num[cmdbuf_temp[i] - '0'];
                        if (cmdbuf_temp[i] == 'a' || cmdbuf_temp[i] == 'b' || cmdbuf_temp[i] == 'c' || cmdbuf_temp[i] == 'd' || cmdbuf_temp[i] == 'e' || cmdbuf_temp[i] == 'f')
                        {
                            buf[1] = num[cmdbuf_temp[i] - 87];
                        }
                        if (write(fd, buf, 2) < 0)
                        {
                            perror("write\n");
                        }
                        usleep(3000);
                    }
                    if (i == strlen(cmdbuf_temp) - 1 && n == 0)
                    {
                        break;
                    }
                    if (cmdbuf_temp[i + 1] == '.')
                    {
                        pos = 1 << i;
                        buf[0] = pos;
                        buf[1] = num[cmdbuf_temp[i] - '0'] | 0x80;
                        if (cmdbuf_temp[i] == 'a' || cmdbuf_temp[i] == 'b' || cmdbuf_temp[i] == 'c' || cmdbuf_temp[i] == 'd' || cmdbuf_temp[i] == 'e' || cmdbuf_temp[i] == 'f')
                        {
                            buf[1] = num[cmdbuf_temp[i] - 87] | 0x80;
                        }
                        if (write(fd, buf, 2) < 0)
                        {
                            perror("write\n");
                        }
                        usleep(3000);
                        n = 1;
                        i++;
                    }
                    i++;
                }
            }
            // usleep(3000);
        }
}

#endif