

CC := gcc
CXX := g++
RM := rm

CFLAGS=$(shell pkg-config  --cflags libusb-1.0 opencv) -Wall
CXXFLAGS=$(shell pkg-config  --cflags libusb-1.0 opencv) -Wall   -lstdc++
LFLAGS=$(shell pkg-config --libs  libusb-1.0 opencv) -lpthread 


.PHONY : all clean
all: tov4l tojpeg


tojpeg: mainjpeg.o thermapp.o process.o
	$(CXX) $^  $(LFLAGS)  -o $@ 
	
tov4l: main.o thermapp.o process.o
	$(CXX)  $^ $(LFLAGS)  -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.
	
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I.


clean:
	$(RM) -f *.o prog