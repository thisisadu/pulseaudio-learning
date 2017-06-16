CC=gcc
CFLAGS = -lpulse -lpulse-simple

all:simple-play simple-record simple-rec-play

simple-play:simple-play.c
	$(CC) $(CFLAGS) $^ -o $@ 

simple-record:simple-record.c
	$(CC) $(CFLAGS) $^ -o $@ 

simple-rec-play:simple-rec-play.c
	$(CC) $(CFLAGS) $^ -o $@ 

clean:
	rm *.o simple-play simple-record simple-rec-play
