

CC := gcc
RM := rm

CFLAGS=$(shell pkg-config  --cflags libusb-1.0 opencv) -Wall
LFLAGS=$(shell pkg-config --libs  libusb-1.0 opencv) -lpthread 


SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)


.PHONY : all clean
all: prog


prog: $(OBJS)
	$(CC)  $(OBJS) $(LFLAGS)  -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.


clean:
	$(RM) -f *.o prog