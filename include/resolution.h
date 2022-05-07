#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <stdint.h>

struct Resolution {
    uint8_t length;
    uint8_t width;
};
extern struct Resolution terminal_resolution;
void resolution_screen();

#endif