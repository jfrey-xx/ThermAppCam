#include <stdint.h>


// can be called by C program
// convert buffer to color image
// doflip: 1 to enable flip, that is selected with flipcode
void thing(uint8_t img[], uint8_t dest[], int w, int h, int show, int doflip, int flipcode);

// FIXME: kind of duplicate of "thing" to speedup
void exportjpeg(const char* filename, uint8_t img[], int w, int h, int show, int doflip, int flipcode);