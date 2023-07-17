#ifndef LED__H
#define LED__H

#include <stdio.h>
#include "../include/mqtt.h"

#define LED_TYPE 'H'

#define LED2_CMD	_IOW(LED_TYPE, 0, int)
#define LED3_CMD	_IOW(LED_TYPE, 1, int)
 

void led_collect(struct mqtt_default_config *cfg);
void led_control(struct msg_queue* msg_queue);


#endif
