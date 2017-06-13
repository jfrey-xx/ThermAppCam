// processing image, here colormap

#include <iostream>
extern "C" {
#include "process.h"
}

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>

using namespace std;
using namespace cv;

void thing(uint8_t img[], uint8_t dest[], int w, int h, int show, int doflip, int flipcode) { 

    // convert input to Mat
    Mat imgcv(h, w, CV_8UC1, img);

    // Holds the colormap version of the image:
    Mat imgmap;
    

    // Apply the colormap:
    applyColorMap(imgcv, imgmap, COLORMAP_HOT);
    
    Mat imgflip;
    if (doflip != 0) {
        flip(imgmap, imgflip, flipcode);
    }
    else {
        imgflip = imgmap;
    }
    
    if (show == 1) {
        // show the image
        imshow("mainWin", imgflip);
    
        // wait for a key, without it window would not show
        waitKey(1);
    }
    
    // copy buffer to destination
    int len = w * h * 3; // 3 channels image
    copy(imgflip.data, imgflip.data+len, dest);
}


void exportjpeg(const char *filename, uint8_t img[], int w, int h) {
    // convert input to Mat
    Mat imgcv(h, w, CV_8UC3, img);
    imwrite(filename, imgcv);
}