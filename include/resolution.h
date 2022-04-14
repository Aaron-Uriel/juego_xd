#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <stdint.h>

typedef struct {
    uint8_t height;
    uint8_t width;
} Resolution;
extern Resolution terminal_resolution;
void resolution_screen();

#endif