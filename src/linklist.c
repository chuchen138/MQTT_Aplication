#include <stdio.h>
#include "../include/list.h"
#include "../include/linklist.h"
#include "string.h"


LIST_HEAD(linklist_sensor_all);

void linklist_sensor_init(struct linklist_sensor *linklist_sensor_name, char *sensor_name)
{
    linklist_sensor_name->name = sensor_name;
    list_add_tail(&linklist_sensor_name->list, &linklist_sensor_all);
}

void search_sensor_control(struct linklist_sensor *linklist_sensor_pos,struct msg_queue* msg_queue)
{
    //printf("%s\n",name);
    // printf("%s\n",linklist_sensor_pos->name); 
    //printf("search_sensor_control\n");
    //printf("%d\n",cmd);
    //printf("cmdbuf:%s\n",cmdbuf);

    list_for_each_entry(linklist_sensor_pos,&linklist_sensor_all,list)
    {
        //printf("%s\n",linklist_sensor_pos->name); 
        if(strcmp(msg_queue->name,linklist_sensor_pos->name) == 0)
        {
            //printf("linklist_sensor_pos->name_no:%s\n",linklist_sensor_pos->name);
            // if(strcmp(linklist_sensor_pos->name,"nixie_tube")!=0)
            // {
                //printf("%s\n",linklist_sensor_pos->name); 
                //printf("linklist_sensor_pos->name:%s\n",linklist_sensor_pos->name);
                linklist_sensor_pos->func_control(msg_queue);
            // }
        }
    }
}


void search_sensor_control_nixie_tube(struct linklist_sensor *linklist_sensor_pos,struct msg_queue *msg_queue)
{
    {
    //printf("%s\n",name);
    // printf("%s\n",linklist_sensor_pos->name); 
    //printf("search_sensor_control\n");
    //printf("%d\n",cmd);
    //printf("cmdbuf:%s\n",cmdbuf);
    list_for_each_entry(linklist_sensor_pos,&linklist_sensor_all,list)
    {
        //printf("%s\n",linklist_sensor_pos->name); 
        if(strcmp(linklist_sensor_pos->name,"nixie_tube") == 0)
        {
            //printf("%s\n",linklist_sensor_pos->name); 
            //printf("linklist_sensor_pos->name_nixie_tube:%s\n",linklist_sensor_pos->name);
            linklist_sensor_pos->func_control(msg_queue);
        }
    }
}
}

