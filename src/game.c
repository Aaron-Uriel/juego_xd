#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <time.h>

#include <ncurses.h>

#include "game.h"
#include "world.h"
#include "resolution.h"
#include "utils.h"
#include "colors.h"

enum GameElements {
    GAMEPLAY,
    INFORMATION,
    GAME_ELEMENTS_LIMIT
};


void update_visible_world(struct VisibleWorld * const, struct Entity * const[ENTITY_LIMIT], struct Resolution);
void render_visible(struct VisibleWorld * const, WINDOW *window_array[], const struct Resolution resolution_array[]);
void draw_window_borders(WINDOW *window);
void handle_all_position_change_requests(struct VisibleWorld * const);

void new_game() {
     /*
     * ¿Cómo funciona el mundo?
     * El mundo es un arreglo bidimensional de casillas etiquetadas (o sea una tabla)
     * La etiqueta en la casilla nos permite saber si esta guarda un caracter (como un bloque o algo que asemeje una puerta)
     * o si guarda un arreglo de entidades (el cual nos va a decir cuales y cuantas son las entidades sobre tal casilla).
     */
    struct Cell world[WORLD_LENGTH][WORLD_WIDTH];
    world_init(world);

    WINDOW * const border_gameplay_window = newwin(terminal_resolution.length, terminal_resolution.width * 0.70, 0, 0);
    WINDOW * const border_info_window = newwin(terminal_resolution.length, terminal_resolution.width * 0.30, 0, terminal_resolution.width * 0.70);

    WINDOW * const gameplay_window = newwin(terminal_resolution.length - 2, (terminal_resolution.width * 0.70) - 2, 1, 1);
    WINDOW * const info_window = newwin(terminal_resolution.length - 2, (terminal_resolution.width * 0.30) - 2, 1, (terminal_resolution.width * 0.70) + 1);
    keypad(gameplay_window, TRUE); 
    nodelay(gameplay_window, TRUE);

    struct Resolution gameplay_resolution, info_resolution;
    getmaxyx(gameplay_window, gameplay_resolution.length, gameplay_resolution.width);
    getmaxyx(info_window, info_resolution.length, info_resolution.width);

    WINDOW *window_array[] = {gameplay_window, info_window};
    struct Resolution resolution_array[] = {gameplay_resolution, info_resolution};
    
    draw_window_borders(border_gameplay_window);
    draw_window_borders(border_info_window);
    wrefresh(border_gameplay_window);
    wrefresh(border_info_window);

    struct VisibleWorld *visible_world = &(struct VisibleWorld) { 0 };
    visible_world_init(visible_world, world);
    
    struct Entity *entities[ENTITY_LIMIT];
    for (int i = 0; i < ENTITY_LIMIT; i++) {
        entities[i] = malloc(sizeof (entities[i]));
        entity_init(entities[i], world, 0x0D9E);
    }
    struct Entity * const player = entities[0];
    entity_player_init(player, world, 0x0DAC, gameplay_resolution);    

    int32_t option;
    do {
        visible_world_update(visible_world, entities, gameplay_resolution);
        handle_all_position_change_requests(visible_world);
        render_visible(visible_world, window_array, resolution_array);

        option = wgetch(gameplay_window); // Incluye un wrefresh(gameplay_window) implícitamente
        msleep(10); // Prevenimos el uso excesivo de CPU

        // Cuando el jugador no presiona ninguna tecla hacemos los movimientos de los enemigos.
        if (option == ERR) {
            continue;
        }

        // Casos de teclas que no implican movimiento (o sea, los casos genéricos)
        switch (option) {
            case 'q':
                endwin();
                exit(0);
                break;
        }

        /*
         * Si el jugador presiona las teclas de movimiento (wasd o las flechas),
         * creamos una petición para cambiar su posición con struct PositionChangeRequest.
         */
        struct PositionChangeRequest position_request;
        switch (option) {
            case KEY_UP:   case KEY_DOWN:  case 'w': case 's': position_request.axis = AXIS_Y; break;
            case KEY_LEFT: case KEY_RIGHT: case 'a': case 'd': position_request.axis = AXIS_X; break;
        }
        switch (option) {
            case KEY_UP:   case KEY_LEFT:  case 'w': case 'a': position_request.delta = DELTA_NEGATIVE; break;
            case KEY_DOWN: case KEY_RIGHT: case 's': case 'd': position_request.delta = DELTA_POSITIVE; break;
        }
        struct EntityRequest request = {
            .requesting_entity = player,
            .kind = ENTITY_REQUEST_KIND_POSITION,
            .content = { position_request }
        };
        entity_request_add(request, visible_world->requests_stack);
    } while(1);
}

void handle_all_position_change_requests(struct VisibleWorld *visible_world) {
    struct EntityRequest *current_entity_request;
    struct Entity *entity;
    bool is_free_space;
    for (uint8_t i = 0; i < STACK_LIMIT; i++) {
        current_entity_request = &visible_world->requests_stack[i];
        entity = current_entity_request->requesting_entity;

        is_free_space = (current_entity_request->requesting_entity == NULL);
        if (is_free_space) {
            continue;
        }

        switch (current_entity_request->kind) {
            case ENTITY_REQUEST_KIND_POSITION: 
                entity_try_to_update_position(entity, current_entity_request->content.position_change_request, visible_world);
                break;
            case ENTITY_REQUEST_KIND_ATTACK:
                break;
        }

        current_entity_request->requesting_entity = NULL;
    }
}

void render_visible(struct VisibleWorld *visible_world, WINDOW *window_array[], const struct Resolution resolution_array[]) {
    const struct Entity * const player = visible_world->visible_entities[0];

    WINDOW * const gameplay_window = window_array[GAMEPLAY];
    struct Resolution gameplay_resolution = resolution_array[GAMEPLAY];
    WINDOW * const info_window = window_array[INFORMATION];
    struct Resolution info_resolution = resolution_array[INFORMATION];

    uint16_t gameplay_window_row, gameplay_window_column, y, x;
    if (visible_world->is_new_quadrant) {
        /* 
        * Para renderizar todo, usamos dos pares de contadores, los primeros son los contadores de la pantalla destinada
        * al gameplay (restandole los lados ocupados por el borde) y los últimos son los relativos al mapa del mundo.
        * Los del mapa se calculan relativos a la posición del punto mas arriba a la izquierda que es posible ver. 
        */
        const struct Cell *cell;
        for (gameplay_window_row = 0, y = visible_world->start_point.y; gameplay_window_row <= (gameplay_resolution.length) && (y < WORLD_LENGTH); gameplay_window_row++, y++) {
            wmove(gameplay_window, gameplay_window_row, 0);
            for (gameplay_window_column = 0, x = visible_world->start_point.x; (gameplay_window_column <= (gameplay_resolution.width)) && (x < WORLD_WIDTH); gameplay_window_column++, x++) {
                cell = &visible_world->world[y][x];
                wprintw(gameplay_window, "%lc", (cell->kind == KIND_CHARACTER)? cell->content.character: ' ');
            }
        }
    }

    struct Entity *current_entity;
    for (uint16_t i = 0; i < STACK_LIMIT; i++) {
        current_entity = visible_world->visible_entities[i];

        if (current_entity == NULL) {
            break;
        }
    
        /* 
         * Ponemos un espacio en donde antes estaba el jugador.
         * Como el jugador no va a traspasar paredes ni nada así, no nos preocupamos por imprimir lo que está en el mapa
         * Solo no se ejecuta cuando el jugador cambia de cuadrante, porque escribe el espacio sobre los bordes en ese caso.
         */
        if (!visible_world->is_new_quadrant) {
            gameplay_window_row = current_entity->previous_position.y - visible_world->start_point.y;
            gameplay_window_column = current_entity->previous_position.x - visible_world->start_point.x;
            mvwaddstr(gameplay_window, gameplay_window_row, gameplay_window_column, " ");
        }

        (current_entity->color != NO_COLOR)? wattron(gameplay_window, COLOR_PAIR(current_entity->color)): false;
        gameplay_window_row = current_entity->current_position.y - visible_world->start_point.y;
        gameplay_window_column = current_entity->current_position.x - visible_world->start_point.x;
        mvwprintw(gameplay_window, gameplay_window_row, gameplay_window_column, "%lc", current_entity->character);
        (current_entity->color != NO_COLOR)? wattroff(gameplay_window, COLOR_PAIR(current_entity->color)): false;
    }


    wmove(info_window, 0, 0);
    wprintw(info_window, "P(%d, %d)\n"
                         "Gameplay alto %d\n"
                         "Gameplay ancho %d\n",
                         player->current_position.y, 
                         player->current_position.x,
                         gameplay_resolution.length, 
                         gameplay_resolution.width

    );
    wrefresh(info_window);
}