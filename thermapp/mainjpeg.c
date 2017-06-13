#include "thermapp.h"


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"


#define FRAME_WIDTH  384
#define FRAME_HEIGHT 288
// will be used to avoid active waiting
#define FPS 10

unsigned int framedelay = 1000000/FPS;

int main(int argc, char *argv[]) {
    ThermApp *therm = thermapp_initUSB();
    if (therm == NULL) {
        fputs("init error\n", stderr);
        return -1;
    }

    /// Debug -> check for thermapp
    if (thermapp_USB_checkForDevice(therm, VENDOR, PRODUCT) == -1){
       fputs("USB_checkForDevice error\n", stderr);
       return -1;
    } else {
        puts("thermapp_FrameRequest_thread\n");
        //Run thread usb therm
        thermapp_FrameRequest_thread(therm);
    }

    

    short frame[PIXELS_DATA_SIZE];
    uint8_t img[165888];
    double pre_offset_cal = 0;
    double gain_cal = 1;
    double offset_cal = 0;
    int flipv = 0;
    if (argc >= 2) {
	flipv = *argv[1];
    }
	// get cal
	printf("Calibrating... cover the lens!\n");
    long meancal = 0;
    short frame1[PIXELS_DATA_SIZE];
    int d_frame1[PIXELS_DATA_SIZE];
    int image_cal[PIXELS_DATA_SIZE];
    int deadpixel_map[PIXELS_DATA_SIZE] = { 0 };
    while(!thermapp_GetImage(therm, frame1));
    while(!thermapp_GetImage(therm, frame1));

    for(int i = 0; i < PIXELS_DATA_SIZE; i++){
        d_frame1[i] = frame1[i];
    }

    for(int i = 0; i < 50; i++){
	printf("Captured calibration frame %d/50. Keep lens covered.\n", i+1);
        while(!thermapp_GetImage(therm, frame1));

        for(int j = 0; j < PIXELS_DATA_SIZE; j++){
            d_frame1[j] += frame1[j];
        }
    }

    for(int i = 0; i < PIXELS_DATA_SIZE; i++){
        image_cal[i] = d_frame1[i] / 50;
	meancal+=image_cal[i];
    }
    meancal = meancal / PIXELS_DATA_SIZE;
	// record the dead pixels
    for(int i = 0; i < PIXELS_DATA_SIZE; i++){
        if ((image_cal[i] > meancal + 250) || (image_cal[i] < meancal - 250)) {
		printf("Dead pixel ID: %d (%d vs %li)\n", i, image_cal[i], meancal);
		deadpixel_map[i] = 1;
	}
    }

	// end of get cal
	printf("Calibration finished\n");
        
    while (1) {
      if (thermapp_GetImage(therm, frame)) {
          
            int i;
	int frameMax = ((frame[0] + pre_offset_cal - image_cal[0]) * gain_cal) + offset_cal;
	int frameMin = ((frame[0] + pre_offset_cal - image_cal[0]) * gain_cal) + offset_cal;
        for (i = 0; i < PIXELS_DATA_SIZE; i++) { // get the min and max values
		// only bother if the pixel isn't dead
		if (!deadpixel_map[i]) {
			int x = ((frame[i] + pre_offset_cal - image_cal[i]) * gain_cal) + offset_cal;
			if (x > frameMax) {
				frameMax = x;
			}
			if (x < frameMin) {
				frameMin = x;
			}
		}
	}
	// second time through, this time actually scaling data
	for (i = 0; i < PIXELS_DATA_SIZE; i++) {
          	int x = ((frame[i] + pre_offset_cal - image_cal[i]) * gain_cal) + offset_cal;
		if (deadpixel_map[i]) {
			x = ((frame[i-1] + pre_offset_cal - image_cal[i-1]) * gain_cal) + offset_cal;
		}
		x = (((double)x - frameMin)/(frameMax - frameMin))*255;
		if (flipv) {
			img[PIXELS_DATA_SIZE - ((i/384)+1)*384 + i%384] = x; 
		} else {
			img[PIXELS_DATA_SIZE - 1 - (PIXELS_DATA_SIZE - ((i/384)+1)*384 + i%384)] = x;
		}
        }
	for (i = PIXELS_DATA_SIZE; i < 165888; i++) {
		img[i] = 128;
	}
	
	

        // convert image to color and export, do not show, flip H and V
        exportjpeg("therm.jpg", img, FRAME_WIDTH, FRAME_HEIGHT, 0, 1, -1);
        
        // free pointer ??
        //free(imgr); // crashes...
      }
      else {
        // wait for next frame to be ready, we don't want to exaust the cpu
        usleep(framedelay);
      }
    }

    thermapp_Close(therm);
    return 0;
}
