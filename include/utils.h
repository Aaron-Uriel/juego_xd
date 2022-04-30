#ifndef UTILS_H
#define UTILS_H

#include <ncurses.h>

void draw_window_borders(WINDOW *window);
int msleep(uint64_t msec);
uint32_t rand_min_max(uint32_t, uint32_t);

struct Point {
    uint16_t y;
    uint16_t x;
};

#endif