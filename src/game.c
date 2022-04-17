#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <ncurses.h>

#include "game.h"
#include "world.h"
#include "resolution.h"

void render_visible(const World *world, Entity *player);
bool player_move(uint16_t y, uint16_t x, Entity *player, World world);
void draw_window_borders(WINDOW *window);

void new_game() {
    World *world = init_world(100, 200);
    const char (*map)[world->width] = (char(*)[world->width]) world->raw_table;

    char entities_limit = 64;
    Entity *player = init_entity(world, '*');
    Entity *entities[entities_limit];
    entities[0] = player;
    int i;
    for (i = 1; i < entities_limit; i++) {
        entities[i] = init_entity(world, '.');
    }


    int32_t option;
    do {
        update_world(world, entities, entities_limit);
        render_visible(world, player);

        option = getch();
        int8_t delta_x = 0, delta_y = 0;
        switch (option) {
            case KEY_UP: case 'w':
                delta_y = -1;
                break;
            case KEY_LEFT: case 'a':
                delta_x = -1;
                break;
            case KEY_DOWN: case 's':
                delta_y = 1;
                break;
             case KEY_RIGHT: case 'd':
                delta_x = 1;
                break;
            case 'q': case 'Q':
                endwin();
                exit(0);
        }
        request_change_of_position(delta_x, delta_y, player, world);
    } while(1);
}

void render_visible(const World *world, Entity *player) {
    const char (*map)[world->width] = (char(*)[world->width]) world->raw_table;
    
    const int32_t border_thickness = 1;
    const Resolution gameplay_resolution = {
        .height = terminal_resolution.height,
        .width = terminal_resolution.width * 0.70
    };
    const Resolution info_resolution = {
        .height = terminal_resolution.height,
        .width = terminal_resolution.width - gameplay_resolution.width
    };

    WINDOW *gameplay_window = newwin(gameplay_resolution.height, gameplay_resolution.width, 0, 0);
    WINDOW *info_window = newwin(info_resolution.height, info_resolution.width, 0, gameplay_resolution.width);

    // Cálculos para ver de donde a donde se va a ver en situaciones normales (muy organizado y simple xd)
    Position the_quadrant_we_are_in = {
        .y = player->current_position.y / (gameplay_resolution.height - (border_thickness * 2)),
        .x = player->current_position.x / (gameplay_resolution.width - (border_thickness * 2))
    };

    Position quadrant_start_point = {
        .y = (gameplay_resolution.height - (border_thickness * 2)) * the_quadrant_we_are_in.y,
        .x = (gameplay_resolution.width - (border_thickness * 2)) * the_quadrant_we_are_in.x
    };

    Position quadrant_end_point = {
        .y = quadrant_start_point.y + gameplay_resolution.height,
        .x = quadrant_start_point.x + gameplay_resolution.width
    };

    if (quadrant_end_point.y > world->length) { quadrant_end_point.y = world->length; }
    if (quadrant_end_point.x > world->width)  { quadrant_end_point.x = world->width; }

    //Fin de los cálculos

    draw_window_borders(gameplay_window);
    wclear(info_window);

    uint16_t gameplay_window_row = 1, gameplay_window_column = 1, y, x;
    for (gameplay_window_row = 1; gameplay_window_row < (gameplay_resolution.height - 1) && (gameplay_window_row < quadrant_end_point.y); gameplay_window_row++) {
        wmove(gameplay_window, gameplay_window_row, 1);
        for (gameplay_window_column = 1; (gameplay_window_column < (gameplay_resolution.width - 1)) && (gameplay_window_column < quadrant_end_point.x); gameplay_window_column++) {
            y = quadrant_start_point.y + gameplay_window_row - 1;
            x = quadrant_start_point.x + gameplay_window_column - 1;
            wprintw(gameplay_window, "%c", map[y][x]);
        }
    }


    wprintw(info_window, "P(%d, %d)\n"
                         "Gameplay alto %d\n"
                         "Gameplay ancho %d\n",
                         player->current_position.y, 
                         player->current_position.x, 
                         gameplay_resolution.height, 
                         gameplay_resolution.width

    );
    wrefresh(gameplay_window);
    wrefresh(info_window);
}

void draw_window_borders(WINDOW *window) {
    uint32_t height, width;
    getmaxyx(window, height, width);

    // Bordes
    mvwprintw(window, 0, 0, u8"┏");
    mvwprintw(window, height - 1, 0, u8"┗");
    mvwprintw(window, 0, width - 1, u8"┓");
    mvwprintw(window, height - 1, width - 1, u8"┛");

    // Lados
    uint32_t i;
    for (i = 1; i < (height - 1); i++) {
        mvwprintw(window, i, 0, u8"┃");
        mvwprintw(window, i, width - 1, u8"┃");
    }
    for (i = 1; i < (width - 1); i++) {
        mvwprintw(window, 0, i, u8"━");
        mvwprintw(window, height - 1, i, u8"━");
    }
}