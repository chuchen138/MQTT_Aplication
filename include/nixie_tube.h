#ifndef NIXIE_TUBE__H
#define NIXIE_TUBE__H

#include <stdio.h>
#include "../include/mqtt.h"

char nixie_tube[8];
void nixie_tube_collect(struct mqtt_default_config *cfg);

void nixie_tube_control(char *cmdbuf);

static void pabort(const char *s) ;


#endif