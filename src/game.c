#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include <ncurses.h>
#include <unistd.h>

#include "game.h"
#include "world.h"
#include "resolution.h"
#include "ncurses_utils.h"
#include "colors.h"

enum GameElements {
    GAMEPLAY,
    INFORMATION,
    GAME_ELEMENTS_LIMIT
};

void render_visible(const struct World *world, struct Entity *entities[], WINDOW *window_array[], const struct Resolution resolution_array[]);
void draw_window_borders(WINDOW *window);

void new_game() {
    struct World *world = init_world(100, 200);

    WINDOW *gameplay_window = newwin(terminal_resolution.height, terminal_resolution.width * 0.70, 0, 0);
    WINDOW *info_window = newwin(terminal_resolution.height, terminal_resolution.width * 0.30, 0, terminal_resolution.width * 0.70);
    keypad(gameplay_window, TRUE); 
    nodelay(gameplay_window, TRUE);

    struct Resolution gameplay_resolution, info_resolution;
    getmaxyx(gameplay_window, gameplay_resolution.height, gameplay_resolution.width);
    getmaxyx(info_window, info_resolution.height, info_resolution.width);

    WINDOW *window_array[] = {gameplay_window, info_window};
    struct Resolution resolution_array[] = {gameplay_resolution, info_resolution};
    
    struct Entity *entities[ENTITY_LIMIT];
    entities[0] = init_entity(world, 0x0D9E);
    struct Entity * const player = entities[0];
    player->color = PLAYER;
    for (int i = 1; i < ENTITY_LIMIT; i++) {
        entities[i] = init_entity(world, 0x0DA9);
    }

    draw_window_borders(gameplay_window);

    int32_t option;
    do {
        render_visible(world, entities, window_array, resolution_array);

        option = wgetch(gameplay_window); // Incluye un wrefresh(gameplay_window) implícitamente
        usleep(10000); // Prevenimos el uso excesivo de CPU
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
        player->position_change_request = (struct PositionChangeRequest) {
            .is_requesting = true,
            .delta_x = delta_x,
            .delta_y = delta_y
        };
    } while(1);
}

void render_visible(const struct World *world, struct Entity *entities[], WINDOW *window_array[], const struct Resolution resolution_array[]) {
    const struct TaggedCell (* const cell_map)[world->width] = (struct TaggedCell(*)[world->width]) world->cells;
    const struct Entity * const player = entities[0];

    WINDOW * const gameplay_window = window_array[GAMEPLAY];
    struct Resolution gameplay_resolution = resolution_array[GAMEPLAY];
    WINDOW * const info_window = window_array[INFORMATION];
    struct Resolution info_resolution = resolution_array[INFORMATION];

    // El área dónde podemos dibujar es la del gameplay menos los dos bordes
    const int32_t border_thickness = 1;
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

    //wclear(info_window);
    uint16_t gameplay_window_row = 1, gameplay_window_column = 1, y = 1, x = 1;
    if (we_are_in_new_quadrant) {
        /* 
        * Para renderizar todo, usamos dos pares de contadores, los primeros son los contadores de la pantalla destinada
        * al gameplay (restandole los lados ocupados por el borde) y los últimos son los relativos al mapa del mundo.
        * Los del mapa se calculan relativos a la posición del punto mas arriba a la izquierda que es posible ver. 
        */

        const struct TaggedCell *cell;
        for (gameplay_window_row = 1, y = quadrant_start_point.y; gameplay_window_row <= (gameplay_resolution.height) && (y < world->length); gameplay_window_row++, y++) {
            wmove(gameplay_window, gameplay_window_row, 1);
            for (gameplay_window_column = 1, x = quadrant_start_point.x; (gameplay_window_column <= (gameplay_resolution.width)) && (x < world->width); gameplay_window_column++, x++) {
                cell = &cell_map[y][x];
                wprintw(gameplay_window, "%lc", (cell->tag == CHARACTER)? cell->cell.character: ' ');
            }
        }
    }
    bool is_at_screen, is_requesting_position_change;
    bool is_at_screen_verticaly, is_at_screen_horizontaly;
    struct Entity *current_entity;
    for (uint16_t i = 0; i < ENTITY_LIMIT; i++) {
        current_entity = entities[i];
        
        is_requesting_position_change = current_entity->position_change_request.is_requesting;
        if (is_requesting_position_change) {
            progresive_position_change(current_entity, world);
        }

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

    wmove(info_window, 0, 0);
    wprintw(info_window, "P(%d, %d)\n"
                         "Gameplay alto %d\n"
                         "Gameplay ancho %d\n"
                         "Índice de posición del jugador %d",
                         player->current_position.y, 
                         player->current_position.x,
                         gameplay_resolution.height, 
                         gameplay_resolution.width,
                         player->stack_index

    );
    wrefresh(info_window);
}