#include <stdint.h>


// can be called by C program
// convert buffer to color image
// doflip: 1 to enable flip, that is selected with flipcode
uint8_t* thing(uint8_t img[], int w, int h, int show, int doflip, int flipcode);