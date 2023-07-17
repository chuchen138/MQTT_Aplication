#ifndef LINKLIST_H
#define LINKLIST_H

#include <stdio.h>
#include "list.h"
#include "../include/mqtt.h"

typedef void (*func)(struct msg_queue* msg_queue);

struct linklist_sensor{
	char *name;
	func func_control;
	struct list_head list;
};


void linklist_sensor_init(struct linklist_sensor *linklist_sensor_node, char *sensor_name);
void search_sensor_control(struct linklist_sensor *linklist_sensor_pos,struct msg_queue *msg_queue);

void search_sensor_control_nixie_tube(struct linklist_sensor *linklist_sensor_pos,struct msg_queue *msg_queue);


#endif