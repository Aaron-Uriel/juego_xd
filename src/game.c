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
#include "colors.h"

void render_visible(const struct World *world, struct Entity *entities[], uint16_t entities_limit, WINDOW *gameplay_window, WINDOW *info_window);
void draw_window_borders(WINDOW *window);

void new_game() {
    struct World *world = init_world(100, 200);
    const wchar_t (*map)[world->width] = (wchar_t(*)[world->width]) world->raw_world;

    WINDOW *gameplay_window = newwin(terminal_resolution.height, terminal_resolution.width * 0.70, 0, 0);
    WINDOW *info_window = newwin(terminal_resolution.height, terminal_resolution.width * 0.30, 0, terminal_resolution.width * 0.70);
    keypad(gameplay_window, TRUE); 
    
    uint16_t entities_limit = 128;
    struct Entity *entities[entities_limit];
    entities[0] = init_entity(world, 0x0D9E);
    struct Entity * const player = entities[0];
    player->color = PLAYER;
    for (int i = 1; i < entities_limit; i++) {
        entities[i] = init_entity(world, 0x0DA9);
    }

    draw_window_borders(gameplay_window);

    int32_t option;
    do {
        render_visible(world, entities, entities_limit, gameplay_window, info_window);

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

void render_visible(const struct World *world, struct Entity *entities[], uint16_t entities_limit, WINDOW *gameplay_window, WINDOW *info_window) {
    const wchar_t (*map)[world->width] = (wchar_t(*)[world->width]) world->raw_world;
    const struct Entity * const player = entities[0];

    const int32_t border_thickness = 1;
    Resolution gameplay_resolution, info_resolution;
    getmaxyx(gameplay_window, gameplay_resolution.height, gameplay_resolution.width);
    getmaxyx(info_window, info_resolution.height, info_resolution.width);

    // Le restamos los bordes a la resolución
    gameplay_resolution.height -= border_thickness * 2;
    gameplay_resolution.width -= border_thickness * 2;

    // Cálculos para ver de donde a donde se va a ver en situaciones normales (muy organizado y simple xd)
    static struct Position the_quadrant_we_are_in = {1000, 1000};
    struct Position possible_new_quadrant = {
        .y = player->current_position.y / gameplay_resolution.height,
        .x = player->current_position.x / gameplay_resolution.width
    };
    bool we_are_in_new_quadrant = (possible_new_quadrant.x != the_quadrant_we_are_in.x || possible_new_quadrant.y != the_quadrant_we_are_in.y);
    if (we_are_in_new_quadrant) {
        the_quadrant_we_are_in = possible_new_quadrant;
    }


    static struct Position quadrant_start_point = {0, 0};
    if (we_are_in_new_quadrant) {
        quadrant_start_point.y = gameplay_resolution.height * the_quadrant_we_are_in.y;
        quadrant_start_point.x = gameplay_resolution.width * the_quadrant_we_are_in.x;
    }

    //Fin de los cálculos

    wclear(info_window);
    uint16_t gameplay_window_row = 1, gameplay_window_column = 1, y = 1, x = 1;
    if (we_are_in_new_quadrant) {
        /* 
        * Para renderizar todo, usamos dos pares de contadores, los primeros son los contadores de la pantalla destinada
        * al gameplay (restandole los lados ocupados por el borde) y los últimos son los relativos al mapa del mundo.
        * Los del mapa se calculan relativos a la posición del punto mas arriba a la izquierda que es posible ver. 
        */

        for (gameplay_window_row = 1, y = quadrant_start_point.y; gameplay_window_row <= (gameplay_resolution.height) && (y < world->length); gameplay_window_row++, y++) {
            wmove(gameplay_window, gameplay_window_row, 1);
            for (gameplay_window_column = 1, x = quadrant_start_point.x; (gameplay_window_column <= (gameplay_resolution.width)) && (x < world->width); gameplay_window_column++, x++) {
                wprintw(gameplay_window, "%lc", map[y][x]);
            }
        }
    }
    bool is_at_screen;
    bool is_at_screen_verticaly, is_at_screen_horizontaly;
    struct Entity *current_entity;
    for (uint16_t i = 0; i < entities_limit; i++) {
        current_entity = entities[i];
        
        is_at_screen_verticaly = (current_entity->current_position.y >= quadrant_start_point.y) && (current_entity->current_position.y < (quadrant_start_point.y + gameplay_resolution.height));
        is_at_screen_horizontaly = (current_entity->current_position.x >= quadrant_start_point.x) && (current_entity->current_position.x < (quadrant_start_point.x + gameplay_resolution.width));
        
        is_at_screen = is_at_screen_horizontaly && is_at_screen_verticaly;
        if (is_at_screen) {
            /* 
             * Ponemos un espacio en donde antes estaba el jugador.
             * Como el jugador no va a traspasar paredes ni nada así, no nos preocupamos por imprimir lo que está en el mapa
             * Solo no se ejecuta cuando el jugador cambia de cuadrante, porque escribe el espacio sobre los bordes en ese caso.
             */
            if (!we_are_in_new_quadrant) {
                gameplay_window_row = current_entity->previous_position.y - quadrant_start_point.y + border_thickness;
                gameplay_window_column = current_entity->previous_position.x - quadrant_start_point.x + border_thickness;
                mvwaddstr(gameplay_window, gameplay_window_row, gameplay_window_column, " ");
            }

            (current_entity->color != NO_COLOR)? wattron(gameplay_window, COLOR_PAIR(current_entity->color)): false;
            gameplay_window_row = current_entity->current_position.y - quadrant_start_point.y + border_thickness;
            gameplay_window_column = current_entity->current_position.x - quadrant_start_point.x + border_thickness;
            mvwprintw(gameplay_window, gameplay_window_row, gameplay_window_column, "%lc", current_entity->character);
            (current_entity->color != NO_COLOR)? wattroff(gameplay_window, COLOR_PAIR(current_entity->color)): false;
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