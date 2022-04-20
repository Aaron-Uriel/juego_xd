#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include <ncurses.h>

#include "game.h"
#include "world.h"
#include "resolution.h"
#include "ncurses_utils.h"

void render_visible(const World *world, Entity *entities[], WINDOW *gameplay_window, WINDOW *info_window);
bool player_move(uint16_t y, uint16_t x, Entity *player, World world);
void draw_window_borders(WINDOW *window);

void new_game() {
    World *world = init_world(100, 200);
    const wchar_t (*map)[world->width] = (wchar_t(*)[world->width]) world->raw_table;

    WINDOW *gameplay_window = newwin(terminal_resolution.height, terminal_resolution.width * 0.70, 0, 0);
    WINDOW *info_window = newwin(terminal_resolution.height, terminal_resolution.width * 0.30, 0, terminal_resolution.width * 0.70);
    keypad(gameplay_window, TRUE); 
    
    int entities_limit = 128;
    Entity *entities[entities_limit];
    entities[0] = init_entity(world, 0x0D9E);
    Entity * const player = entities[0];
    for (int i = 1; i < entities_limit; i++) {
        entities[i] = init_entity(world, 0x0DA9);
    }

    draw_window_borders(gameplay_window);

    int32_t option;
    do {
        update_world(world, entities, entities_limit);
        render_visible(world, entities, gameplay_window, info_window);

        option = wgetch(gameplay_window); // Incluye un wrefresh(gameplay_window) implícitamente
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

void render_visible(const World *world, Entity *entities[], WINDOW *gameplay_window, WINDOW *info_window) {
    const wchar_t (*map)[world->width] = (wchar_t(*)[world->width]) world->raw_table;
    const Entity * const player = entities[0];

    const int32_t border_thickness = 1;
    Resolution gameplay_resolution, info_resolution;
    getmaxyx(gameplay_window, gameplay_resolution.height, gameplay_resolution.width);
    getmaxyx(info_window, info_resolution.height, info_resolution.width);

    // Cálculos para ver de donde a donde se va a ver en situaciones normales (muy organizado y simple xd)
    Position the_quadrant_we_are_in = {
        .y = player->current_position.y / (gameplay_resolution.height - (border_thickness * 2)),
        .x = player->current_position.x / (gameplay_resolution.width - (border_thickness * 2))
    };

    Position quadrant_start_point = {
        .y = (gameplay_resolution.height - (border_thickness * 2)) * the_quadrant_we_are_in.y,
        .x = (gameplay_resolution.width - (border_thickness * 2)) * the_quadrant_we_are_in.x
    };

    //Fin de los cálculos

    wclear(info_window);
    /* 
     * Para renderizar todo, usamos dos pares de contadores, los primeros son los contadores de la pantalla destinada
     * al gameplay (restandole los lados ocupados por el borde) y los últimos son los relativos al mapa del mundo.
     * Los del mapa se calculan relativos a la posición del punto mas arriba a la izquierda que es posible ver. 
     */
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    uint16_t gameplay_window_row = 1, gameplay_window_column = 1, y = 1, x = 1;
    for (gameplay_window_row = 1, y = quadrant_start_point.y; gameplay_window_row < (gameplay_resolution.height - 1) && (y < world->length); gameplay_window_row++, y++) {
        wmove(gameplay_window, gameplay_window_row, 1);
        for (gameplay_window_column = 1, x = quadrant_start_point.x; (gameplay_window_column < (gameplay_resolution.width - 1)) && (x < world->width); gameplay_window_column++, x++) {
            if (player->current_position.y == y && player->current_position.x == x) {
                wattron(gameplay_window, COLOR_PAIR(1));
            }
            wprintw(gameplay_window, "%lc", map[y][x]);
            if (player->current_position.y == y, player->current_position.x == x) {
                wattroff(gameplay_window, COLOR_PAIR(1));
            }
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
    wrefresh(info_window);
}

