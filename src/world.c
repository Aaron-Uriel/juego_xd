#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "world.h"

void world_init(struct Cell world[WORLD_LENGTH][WORLD_WIDTH]) {
    struct Cell *cell;
    uint16_t row, column;
    for (row = 0; row < WORLD_LENGTH; row++) {
        for (column = 0; column < WORLD_WIDTH; column++) {
            cell = &world[row][column];

            cell->kind = KIND_ENTITY_HOLDER;
            cell->content.entity_holder = NULL;
        }
    }
}

void visible_world_init(struct VisibleWorld * const visible_world, struct Cell world[WORLD_LENGTH][WORLD_WIDTH]) {
    visible_world->world = world;

    visible_world->quadrant.y = UNINITIALIZED_16BIT;
    visible_world->quadrant.x = UNINITIALIZED_16BIT;

    visible_world->is_new_quadrant = true;
}

static inline bool cell_is_free(const struct Cell * const cell) {
    if (cell->kind == KIND_CHARACTER) {
        return false;
    }
    if (cell->kind == KIND_ENTITY_HOLDER && cell->content.entity_holder != NULL) {
        return false;
    }
    return true;
}

void visible_world_update(struct VisibleWorld * const visible_world, struct Entity * const entities[STACK_LIMIT], const struct Resolution resolution) {
    const struct Entity * const player = entities[0];

    struct Position possible_new_quadrant;
    possible_new_quadrant.y = player->current_position.y / resolution.length;
    possible_new_quadrant.x = player->current_position.x / resolution.width;

    if (visible_world->is_new_quadrant == true) { visible_world->is_new_quadrant = false; }

    bool we_are_in_new_quadrant = (possible_new_quadrant.x != visible_world->quadrant.x || possible_new_quadrant.y != visible_world->quadrant.y);
    if (we_are_in_new_quadrant) {
        visible_world->is_new_quadrant = true;

        visible_world->quadrant = possible_new_quadrant;

        visible_world->start_point.y = resolution.length * visible_world->quadrant.y;
        visible_world->start_point.x = resolution.width * visible_world->quadrant.x;

        visible_world->end_point.y = visible_world->start_point.y + resolution.length;
        visible_world->end_point.x = visible_world->start_point.x + resolution.width;

        struct Entity *current_entity;
        bool is_at_screen, is_at_screen_vertically, is_at_screen_horizontally;
        for (int i = 0, stack_index = 0; i < ENTITY_LIMIT; i++) {
            current_entity = entities[i];
            
            is_at_screen_vertically = (current_entity->current_position.y >= visible_world->start_point.y) && (current_entity->current_position.y < visible_world->end_point.y);
            is_at_screen_horizontally = (current_entity->current_position.x >= visible_world->start_point.x) && (current_entity->current_position.x < visible_world->end_point.x);
            is_at_screen = is_at_screen_horizontally && is_at_screen_vertically;
    
            if (is_at_screen) {
                visible_world->visible_entities[stack_index++] = current_entity;
                if (i == STACK_LIMIT) {
                    break;
                }
            }
        }
    
        for (int i = 0; i < STACK_LIMIT; i++) {
            visible_world->requests_stack[i].requesting_entity = NULL;
        }
    }
}

void entity_init(struct Entity * const new_entity, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], const wchar_t character) {
    struct Cell *cell;

    struct Position initial_position;
    do {
        initial_position = (struct Position) {
            .x = rand_min_max(0, WORLD_WIDTH - 1),
            .y = rand_min_max(0, WORLD_LENGTH - 1)
        };
        
        cell = &world[initial_position.y][initial_position.x];
    } while (cell_is_free(cell) == false);

    // Las posición actual y anterior son la misma al principio
    new_entity->current_position = initial_position;
    new_entity->previous_position = initial_position;
    
    cell->content.entity_holder = new_entity;

    new_entity->character = character;
    new_entity->color = NO_COLOR;
}

void entity_player_init(struct Entity * const new_player, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], const wchar_t character, const struct Resolution gameplay_resolution) {
    const struct Position selected_quadrant = {
        .y = (WORLD_LENGTH - 1) / gameplay_resolution.length,
        .x = ((WORLD_LENGTH - 1) / 2) / gameplay_resolution.width
    };

    // Queremos que donde se genere el jugador sea mas o menos por el centro del cuadrante
    const uint8_t vertical_margin = gameplay_resolution.length * 0.20;
    const uint8_t horizontal_margin = gameplay_resolution.width * 0.40;

    const struct Position start_point = {
        .y = selected_quadrant.y * gameplay_resolution.length + vertical_margin,
        .x = selected_quadrant.x * gameplay_resolution.width + vertical_margin
    };

    struct Position end_point = {
        .y = start_point.y + gameplay_resolution.length - vertical_margin,
        .x = start_point.x + gameplay_resolution.width - horizontal_margin
    };

    if (end_point.y >= WORLD_LENGTH) { end_point.y = WORLD_LENGTH - 1; }
    if (end_point.x >= WORLD_WIDTH) { end_point.x = WORLD_WIDTH - 1; }

    struct Cell *cell;
    struct Position initial_position;
    do {
        initial_position.y = rand_min_max(start_point.y, end_point.y);
        initial_position.x = rand_min_max(start_point.x, end_point.x);

        cell = &world[initial_position.y][initial_position.x];
    } while(!cell_is_free(cell));

    // Las posición actual y anterior son la misma al principio
    new_player->current_position = initial_position;
    new_player->previous_position = initial_position;

    cell->content.entity_holder = new_player;

    new_player->character = character;
    new_player->color = PLAYER;
}


bool entity_request_add(const struct EntityRequest request, struct EntityRequest requests_stack[STACK_LIMIT]) {
    uint8_t free_stack_position;
    for (free_stack_position = 0; free_stack_position < STACK_LIMIT; free_stack_position++) {
        if (requests_stack[free_stack_position].requesting_entity == NULL) {
            break;
        }
    }
    if (free_stack_position == STACK_LIMIT) { return 1; }

    requests_stack[free_stack_position] = request;

    return 0;
}

bool entity_try_to_update_position(struct Entity * const entity, const struct PositionChangeRequest position_request, const struct VisibleWorld * const visible_world) {
    struct Cell * const previous_cell = &visible_world->world[entity->current_position.y][entity->current_position.x];
    struct Cell *next_cell;

    entity->previous_position = entity->current_position;

    struct Position new_position = entity->current_position;
    switch (position_request.axis) {
        case AXIS_Y:
            if (position_request.delta == DELTA_POSITIVE) {
                new_position.y++;
                entity->facing = FACING_SOUTH;
            } else {
                new_position.y--;
                entity->facing = FACING_NORTH;
            }
            break;
        case AXIS_X:
            if (position_request.delta == DELTA_POSITIVE) {
                new_position.x++;
                entity->facing = FACING_EAST;
            } else {
                new_position.x--;
                entity->facing = FACING_WEST;
            }
    }

    bool is_out_of_bounds = (new_position.x >= WORLD_WIDTH) || (new_position.y >= WORLD_LENGTH);
    if (is_out_of_bounds) {
        return EXIT_FAILURE;
    }

    next_cell = &visible_world->world[new_position.y][new_position.x];

    if (cell_is_free(next_cell) == false) {
        return EXIT_FAILURE;
    }

    entity->current_position = new_position;

    // Borramos la información de la entidad en la celda anterior y la ponemos en la nueva celda
    previous_cell->content.entity_holder = NULL;
    next_cell->content.entity_holder = entity;

    return EXIT_SUCCESS;
}