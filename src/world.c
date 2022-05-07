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

void init_visible_world(struct VisibleWorld *visible_world, struct Cell world[WORLD_LENGTH][WORLD_WIDTH]) {
    visible_world->world = world;

    visible_world->quadrant.y = UNINITIALIZED_16;
    visible_world->quadrant.x = UNINITIALIZED_16;

    visible_world->is_new_quadrant = true;
}

struct Entity *init_entity(struct Cell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character) {

    struct Cell *tagged_cell;
    struct Position initial_position;
    do {
        initial_position = (struct Position) {
            .x = rand_min_max(0, WORLD_WIDTH - 1),
            .y = rand_min_max(0, WORLD_LENGTH - 1)
        };
        
        tagged_cell = &world[initial_position.y][initial_position.x];
    } while (tagged_cell->tag == CHARACTER);

    // Las posición actual y anterior son la misma al principio
    struct Entity *entity = malloc(sizeof(struct Entity));
    entity->current_position.x = initial_position.x;
    entity->current_position.y = initial_position.y;
    entity->previous_position = entity->current_position;
    entity->character = character;
    entity->color = NO_COLOR;

    bool return_status = add_entity_to_cell_stack(entity, tagged_cell);
    if (return_status == EXIT_FAILURE) {
        fprintf(stderr, "Error crítico ocurrido: La entidad no pudo inicializarse");
        exit(EXIT_FAILURE);
    }

    return entity;
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