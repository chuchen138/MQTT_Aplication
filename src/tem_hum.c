#include "../include/tem_hum.h"
#include "../include/cJSON.h"
#include "../include/fan.h"
#if 1

void tem_hum_collect(struct mqtt_default_config *cfg)
{
    int temp_raw = 0;
    int temp_offset = 0;
    float temp_scale = 0;

    int hum_raw = 0;
    int hum_offset = 0;
    float hum_scale = 0;

    read_sysfs_int("iio:device0", "in_temp_raw", &temp_raw);
    read_sysfs_int("iio:device0", "in_temp_offset", &temp_offset);
    read_sysfs_float("iio:device0", "in_temp_scale", &temp_scale);

    temperature = (temp_raw + temp_offset) * temp_scale / 1000;
  //  printf("temperature = %.2f\n", (temp_raw + temp_offset) * temp_scale / 1000);

    read_sysfs_int("iio:device0", "in_humidityrelative_raw", &hum_raw);
    read_sysfs_int("iio:device0", "in_humidityrelative_offset", &hum_offset);
    read_sysfs_float("iio:device0", "in_humidityrelative_scale", &hum_scale);

    humidity = (hum_raw + hum_offset) * hum_scale / 1000;
  //  printf("humidity = %.2f%%\n", (hum_raw + hum_offset) * hum_scale / 1000);

     printf("collect data tem_hum:tem=%d hum=%d\n",temperature,humidity);

    if(temperature != temperature_temp)
    {
        flag = 0;
        flag_temp = 0;
        memset(temperature_str, 0, sizeof(temperature_str));
        sprintf(temperature_str,"%d",temperature);
    }
    temperature_temp = temperature;

    cJSON *root = NULL;
    root = cJSON_CreateObject();

    cJSON_AddNumberToObject(root, "tem", temperature);
    cJSON_AddNumberToObject(root, "hum", humidity);
    cJSON_AddNumberToObject(root, "id", 0);
    char *buf = cJSON_PrintUnformatted(root);
    // printf("out:%s\n",buf);
    mqtt_pub(&cfg->pub_client, buf, serverbuf_pubtopic);

    //本地socket发送
    memset(server_sendbuf,0,sizeof(server_sendbuf));
    sprintf(server_sendbuf,"%d",temperature);
    printf("server_sendbuf:%s\n",server_sendbuf);
    if(send(acceptfd,server_sendbuf,sizeof(server_sendbuf),0)<0)
    {
        perror("server_sendbuf");
    }
    
}

int read_sysfs_float(const char *device, const char *filename, float *val)
{
    int ret = 0;
    FILE *sysfsfp;
    char temp[128];

    memset(temp, '0', 128);

    ret = sprintf(temp, "/sys/bus/iio/devices/%s/%s", device, filename);
    if (ret < 0)
        goto error;

    sysfsfp = fopen(temp, "r");
    if (!sysfsfp)
    {
        ret = -errno;
        goto error;
    }

    errno = 0;
    if (fscanf(sysfsfp, "%f\n", val) != 1)
    {
        ret = errno ? -errno : -ENODATA;
        if (fclose(sysfsfp))
            perror("read_sysfs_float(): Failed to close dir");

        goto error;
    }

    if (fclose(sysfsfp))
        ret = -errno;

error:

    return ret;
}

int read_sysfs_int(const char *device, const char *filename, int *val)
{
    int ret = 0;
    FILE *sysfsfp;
    char temp[128];

    memset(temp, '0', 128);

    ret = sprintf(temp, "/sys/bus/iio/devices/%s/%s", device, filename);
    if (ret < 0)
        goto error;

    sysfsfp = fopen(temp, "r");
    if (!sysfsfp)
    {
        ret = -errno;
        goto error;
    }

    errno = 0;
    if (fscanf(sysfsfp, "%d\n", val) != 1)
    {
        ret = errno ? -errno : -ENODATA;
        if (fclose(sysfsfp))
            perror("read_sysfs_float(): Failed to close dir");

        goto error;
    }

    if (fclose(sysfsfp))
        ret = -errno;

error:

    return ret;
}

#endif