#ifndef UTILS_H
#define UTILS_H

#include <ncurses.h>

void draw_window_borders(WINDOW *window);
int msleep(uint64_t msec);

#endif