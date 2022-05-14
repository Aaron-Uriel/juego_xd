#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "world.h"

int32_t rand_min_max(int32_t min, int32_t max);

void init_world(struct Cell world[WORLD_LENGTH][WORLD_WIDTH]) {
    struct Cell *tagged_cell;
    uint16_t row, column, i;
    for (row = 0; row < WORLD_LENGTH; row++) {
        for (column = 0; column < WORLD_WIDTH; column++) {
            tagged_cell = &world[row][column];

            tagged_cell->tag = ENTITY_STACK;
            for (i = 0; i < STACK_LIMIT; i++) {
                tagged_cell->cell.entity_stack[i] = NULL;
            }
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
    } while (cell->tag == CHARACTER);

    // Las posición actual y anterior son la misma al principio
    new_entity->current_position = initial_position;
    new_entity->previous_position = initial_position;
    
    bool return_status = add_entity_to_cell_stack(new_entity, cell);
    if (return_status == EXIT_FAILURE) {
        fprintf(stderr, "Error crítico ocurrido: la entidad no pudo inicializarse");
        exit(EXIT_FAILURE);
    }

    new_entity->character = character;
    new_entity->color = NO_COLOR;
}

void init_player(struct Entity * const new_player, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character, struct Resolution gameplay_resolution) {
    const struct Position selected_quadrant = {
        .y = (WORLD_LENGTH - 1) / gameplay_resolution.length,
        .x = ((WORLD_LENGTH - 1) / 2) / gameplay_resolution.width
    };

    const struct Position start_point = {
        .y = selected_quadrant.y * gameplay_resolution.length + 4,
        .x = selected_quadrant.x * gameplay_resolution.width + 8
    };

    struct Position end_point = {
        .y = start_point.y + gameplay_resolution.length - 4,
        .x = start_point.x + gameplay_resolution.width - 8
    };

    if (end_point.y >= WORLD_LENGTH) { end_point.y = WORLD_LENGTH - 1; }
    if (end_point.x >= WORLD_WIDTH) { end_point.x = WORLD_WIDTH - 1; }

    struct Cell *cell;
    struct Position initial_position;
    do {
        initial_position.y = rand_min_max(start_point.y, end_point.y);
        initial_position.x = rand_min_max(start_point.x, end_point.x);

        cell = &world[initial_position.y][initial_position.x];
    } while(cell->tag == CHARACTER);

    // Las posición actual y anterior son la misma al principio
    new_player->current_position = initial_position;
    new_player->previous_position = initial_position;
    
    bool return_status = add_entity_to_cell_stack(new_player, cell);
    if (return_status == EXIT_FAILURE) {
        fprintf(stderr, "Error crítico ocurrido: la entidad no pudo inicializarse");
        exit(EXIT_FAILURE);
    }
    new_player->character = character;
    new_player->color = PLAYER;
}

bool add_entity_to_cell_stack(struct Entity * const entity, struct Cell *cell) {
    if (cell->tag != ENTITY_STACK) {
        return EXIT_FAILURE;
    }

    for (uint8_t i = 0; i < STACK_LIMIT; i++) {
        if (cell->cell.entity_stack[i] == NULL) {
            entity->stack_index = i;
            cell->cell.entity_stack[i] = entity;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

bool remove_entity_from_cell_stack(struct Entity * const entity, struct Cell *cell) {
    const bool its_not_the_same_entity = cell->cell.entity_stack[entity->stack_index] != entity;
    const bool its_not_an_entity_stack = cell->tag != ENTITY_STACK;
    if (its_not_the_same_entity || its_not_an_entity_stack) {
        return EXIT_FAILURE;
    }

    cell->cell.entity_stack[entity->stack_index] = NULL;
    return EXIT_SUCCESS;
}