LDFLAGS += -L$(STAGING_DIR)/usr/lib/pulseaudio -lpulse-simple -lpulse -lpulsecommon-10.0
helloworld:helloworld.o  
	$(CC) $(LDFLAGS) helloworld.o -o helloworld  

helloworld.o:helloworld.c  
	$(CC) $(CFLAGS) -c helloworld.c  

clean:
	rm *.o helloworld

