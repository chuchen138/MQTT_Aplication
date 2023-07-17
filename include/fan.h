#ifndef FAN__H
#define FAN__H

#include "../include/mqtt.h"
#include <stdio.h>

int fan_cmd_flag;
void fan_collect(struct mqtt_default_config *cfg);
void fan_control(struct msg_queue* msg_queue);

#endif