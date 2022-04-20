#include <ncurses.h>

#include "colors.h"

void set_color_pairs() {
    init_pair(PLAYER, COLOR_GREEN, COLOR_BLACK);
    init_pair(ENEMY, COLOR_RED, COLOR_BLACK);
}