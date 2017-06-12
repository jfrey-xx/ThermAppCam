

CC := gcc
CXX := g++
RM := rm

CFLAGS=$(shell pkg-config  --cflags libusb-1.0 opencv) -Wall
CXXFLAGS=$(shell pkg-config  --cflags libusb-1.0 opencv) -Wall   -lstdc++
LFLAGS=$(shell pkg-config --libs  libusb-1.0 opencv) -lpthread 


SRC := $(wildcard *.c)
SRCPP := $(wildcard *.cpp)
OBJS := $(SRC:.c=.o)
OBJSCPP := $(SRCPP:.cpp=.o)


.PHONY : all clean
all: prog


prog: $(OBJS) $(OBJSCPP)
	$(CXX)  $(OBJS) $(OBJSCPP) $(LFLAGS)  -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.
	
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I.


clean:
	$(RM) -f *.o prog