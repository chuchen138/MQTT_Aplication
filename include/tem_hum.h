#ifndef TEM_HUM__H
#define TEM_HUM__H

#include <stdio.h>
#include "../include/mqtt.h"

#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int temperature ;
int humidity;
int temperature_temp;
char temperature_str[8];

void tem_hum_collect(struct mqtt_default_config *cfg);
int read_sysfs_int(const char *device, const char *filename, int *val);
int read_sysfs_float(const char *device, const char *filename, float *val);



#endif