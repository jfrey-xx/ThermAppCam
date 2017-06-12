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

int thing(uint8_t img[], int w, int h) { 

    // convert input to Mat
    Mat imgcv(h, w, CV_8UC1, img);

    // Holds the colormap version of the image:
    Mat imgmap;
    

    // Apply the colormap:
    applyColorMap(imgcv, imgmap, COLORMAP_HOT);
                
    // show the image
    imshow("mainWin", imgmap);
  
    // wait for a key, without it window would not show
    waitKey(1);
        
    return 0;
}

