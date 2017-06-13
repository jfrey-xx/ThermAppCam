#include "thermapp.h"

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <highgui.h> 

#include "process.h"



#define VIDEO_DEVICE "/dev/video2" //FIXME: read from command line
#define FRAME_WIDTH  384
#define FRAME_HEIGHT 288

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

    struct v4l2_capability vid_caps;
    struct v4l2_format vid_format;

    const char *video_device = VIDEO_DEVICE;
    int fdwr = open(video_device, O_RDWR);
    assert(fdwr >= 0);

    int ret_code = ioctl(fdwr, VIDIOC_QUERYCAP, &vid_caps);
    assert(ret_code != -1);

    memset(&vid_format, 0, sizeof(vid_format));

    ret_code = ioctl(fdwr, VIDIOC_G_FMT, &vid_format);

    vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    vid_format.fmt.pix.width = FRAME_WIDTH;
    vid_format.fmt.pix.height = FRAME_HEIGHT;
    vid_format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
    vid_format.fmt.pix.sizeimage =   FRAME_WIDTH  * FRAME_HEIGHT * 3;
    vid_format.fmt.pix.field = V4L2_FIELD_NONE;
    vid_format.fmt.pix.bytesperline = FRAME_WIDTH*3;
    vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

    ret_code = ioctl(fdwr, VIDIOC_S_FMT, &vid_format);


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

        // retrieve image colorized
        uint8_t *imgr = thing(img, FRAME_WIDTH, FRAME_HEIGHT, 1, 1, -1);
        // compute size and write to loopback
        int len = FRAME_HEIGHT * FRAME_WIDTH * 3; //sizeof(&imgr)/sizeof(imgr[0]);
        write(fdwr, imgr, len);
        // free pointer ??
        // free(imgr); // crashes...
      }
    }

    close(fdwr);
    thermapp_Close(therm);
    return 0;
}
