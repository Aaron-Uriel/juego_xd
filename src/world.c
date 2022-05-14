#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "world.h"

int32_t rand_min_max(int32_t min, int32_t max);

bool cell_is_free(struct Cell *cell) {
    if (cell->kind == KIND_CHARACTER) {
        return false;
    }
    if (cell->kind == KIND_ENTITY_HOLDER && cell->content.entity_holder != NULL) {
        return false;
    }
    return true;
}

void init_world(struct Cell world[WORLD_LENGTH][WORLD_WIDTH]) {
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

void init_visible_world(struct VisibleWorld * const visible_world, struct Cell world[WORLD_LENGTH][WORLD_WIDTH]) {
    visible_world->world = world;

    visible_world->quadrant.y = UNINITIALIZED_16;
    visible_world->quadrant.x = UNINITIALIZED_16;

    visible_world->is_new_quadrant = true;
}

void init_entity(struct Entity *new_entity, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character) {

    struct Cell *cell;
    struct Position initial_position;
    do {
        initial_position = (struct Position) {
            .x = rand_min_max(0, WORLD_WIDTH - 1),
            .y = rand_min_max(0, WORLD_LENGTH - 1)
        };
        
        cell = &world[initial_position.y][initial_position.x];
    } while (!cell_is_free(cell));

    // Las posición actual y anterior son la misma al principio
    new_entity->current_position = initial_position;
    new_entity->previous_position = initial_position;
    
    cell->content.entity_holder = new_entity;

    new_entity->character = character;
    new_entity->color = NO_COLOR;
}

void init_player(struct Entity * const new_player, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character, struct Resolution gameplay_resolution) {
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

bool try_to_update_entity_position(struct Entity * const entity, struct PositionChangeRequest position_request, struct VisibleWorld * const visible_world) {
    struct Cell * const previous_cell = &visible_world->world[entity->current_position.y][entity->current_position.x];
    struct Cell *next_cell;

    entity->previous_position = entity->current_position;

    struct Position new_position = entity->current_position;
    switch (position_request.axis) {
        case AXIS_Y: 
            new_position.y += (position_request.delta == DELTA_POSITIVE)? 1: -1; break;
        case AXIS_X:
            new_position.x += (position_request.delta == DELTA_POSITIVE)? 1: -1;
    }

    bool is_out_of_bounds = (new_position.x >= WORLD_WIDTH) || (new_position.y >= WORLD_LENGTH);
    if (is_out_of_bounds) {
        return EXIT_FAILURE;
    }

    next_cell = &visible_world->world[new_position.y][new_position.x];

    if (!cell_is_free(next_cell)) {
        return EXIT_FAILURE;
    }

    entity->current_position = new_position;

    // Borramos la información de la entidad en la celda anterior y la ponemos en la nueva celda
    previous_cell->content.entity_holder = NULL;
    next_cell->content.entity_holder = entity;

    return EXIT_SUCCESS;
}