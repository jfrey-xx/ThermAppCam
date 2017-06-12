// processing image, here colormap

#include <iostream>
extern "C" {
#include "process.h"
}

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace std;
using namespace cv;

int thing(uint8_t img[], int w, int h) { 

    CvMat imgcv =  cvMat(h, w, CV_8UC1, img);

    //CvMat *imgmap = cvCreateMat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1);
            
    // cvApplyColorMap(imgcv, imgmap, COLORMAP_JET);
            
    //CsvColorMap()

                
    // show the image
     cvShowImage("mainWin",  &imgcv );
  
    // wait for a key
    cvWaitKey(1);
        
    return 0;
}

