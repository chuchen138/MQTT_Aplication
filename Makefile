OBJS = main.o linklist.o mqtt.o cJSON.o fan.o led.o tem_hum.o nixie_tube.o 
#CC = /home/linux/buildroot_157/arm-fsmp1x-linux-gnueabihf_sdk-buildroot/bin/arm-fsmp1x-linux-gnueabihf-gcc
CC = /home/linux/Downloads/voice_light/arm-fsmp1x-linux-gnueabihf_sdk-buildroot/bin/arm-fsmp1x-linux-gnueabihf-gcc
#CC = gcc
CFLAGS = -c 
TARGET = fs_smartgreenhouse_demo
LDLIBS = -lpaho-mqtt3cs -lm -lpthread 

$(TARGET):$(OBJS) 
	$(CC) $^ -o $@ $(LDLIBS)
	mv *.o  $(TARGET)  bin/

%.o:src/%.c
	$(CC) $^ $(CFLAGS) -o $@ 

clean:
#	$(RM) *.o $(TARGET) -r
	$(RM) bin/* -r



