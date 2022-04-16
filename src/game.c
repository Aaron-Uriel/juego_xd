#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <ncurses.h>

#include "game.h"
#include "world.h"
#include "resolution.h"

void render_visible(const World *world, Entity *player);
bool player_move(uint16_t y, uint16_t x, Entity *player, World world);

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

    WINDOW *gameplay_window = newwin(terminal_resolution.height - 2, terminal_resolution.width, 0, 0);
    Resolution gameplay_resolution;
    getmaxyx(gameplay_window, gameplay_resolution.height, gameplay_resolution.width);
    // Organizamos todas las subventanas dentro de la ventana principal, como si fuesen cajas
    box(gameplay_window, 0, 0);

    // Cálculos para ver de donde a donde se va a ver en situaciones normales (muy organizado y simple xd)
    Position the_quadrant_we_are_in = {
        .y = player->current_position.y / gameplay_resolution.height,
        .x = player->current_position.x / gameplay_resolution.width
    };

    Position quadrant_start_point = {
        .y = gameplay_resolution.height * the_quadrant_we_are_in.y,
        .x = gameplay_resolution.width * the_quadrant_we_are_in.x
    };

    Position quadrant_end_point = {
        .y = quadrant_start_point.y + gameplay_resolution.height,
        .x = quadrant_start_point.x + gameplay_resolution.width
    };

    if (quadrant_end_point.y > world->length) { quadrant_end_point.y = world->length; }
    if (quadrant_end_point.x > world->width)  { quadrant_end_point.x = world->width; }

    //Fin de los cálculos

    wclear(gameplay_window);
    clear();

    uint16_t terminal_row = 0, terminal_column = 0, map_row, map_column;
    for (map_row = quadrant_start_point.y; map_row < quadrant_end_point.y; terminal_row++, map_row++) {
        wmove(gameplay_window, terminal_row, 0); // Movernos a tal fila y dejarnos al inicio
        for (map_column = quadrant_start_point.x, terminal_column = 0; map_column < quadrant_end_point.x; terminal_column++, map_column++) {
            wprintw(gameplay_window, "%c", map[map_row][map_column]);
        }
    }
    printw("P(%d, %d)", 
            player->current_position.y, 
            player->current_position.x
    );
    refresh();
    wrefresh(gameplay_window);
}