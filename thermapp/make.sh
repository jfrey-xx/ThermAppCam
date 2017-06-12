#!/bin/bash
gcc main.c thermapp.c `pkg-config --libs --cflags libusb-1.0 opencv` -lpthread -o thermapp -Wall
