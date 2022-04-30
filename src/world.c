#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "world.h"

int32_t rand_min_max(int32_t min, int32_t max);

void init_world(struct TaggedCell world[WORLD_LENGTH][WORLD_WIDTH]) {
    struct TaggedCell *tagged_cell;
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

struct Entity *init_entity(struct TaggedCell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character) {

    struct TaggedCell *tagged_cell;
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
    entity->current_position = (struct Position) {
        initial_position.x,
        initial_position.y
    };
    entity->previous_position = (struct Position) {
        initial_position.x,
        initial_position.y
    };
    entity->character = character;
    entity->color = NO_COLOR;

    bool return_status = add_entity_to_stack(entity, tagged_cell->cell.entity_stack);
    if (return_status == EXIT_FAILURE) {
        fprintf(stderr, "Error crítico ocurrido: La entidad no pudo inicializarse");
        exit(EXIT_FAILURE);
    }

    return entity;
}

bool add_entity_to_stack(struct Entity * const entity, struct Entity *stack[STACK_LIMIT]) {
    uint8_t position = UNINITIALIZED_8;
    for (uint8_t i = 0; i < STACK_LIMIT; i++) {
        if (stack[i] == NULL) {
            position = i;
        }
    }
    if (position == UNINITIALIZED_8) { return EXIT_FAILURE; }
    stack[position] = entity;

    return EXIT_SUCCESS;
}

bool remove_entity_from_stack(struct Entity * const entity, struct Entity *stack[STACK_LIMIT]) {
    const bool its_not_the_same_entity = stack[entity->stack_index] != entity;
    if (its_not_the_same_entity) {
        return EXIT_FAILURE;
    }

    stack[entity->stack_index] = NULL;
    return EXIT_SUCCESS;
}