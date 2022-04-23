#ifndef WORLD_H

#define WORLD_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#include "colors.h"

/*
 * Explicación de la lógica del mundo.
 * El mundo es un arreglo de dos dimensiones localizado en la memoria dinámica
 * Cada elemento del mundo es una celda, y esta celda puede ser un carácter (por ejemplo un bloque) o un
 * arreglo de entidades, o sea, un indicador de las entidades que están sobre dicha celda.
 * Las celdas que son arreglos de punteros de entidades, son las partes donde podrán andar las entidades.
 */

#define ENTITY_STACK_INSIDE_CELL_LIMIT 10
#define ENTITY_LIMIT 128

struct Position {
    uint16_t x;
    uint16_t y;
};

struct PositionChangeRequest {
    bool is_requesting;
    int8_t delta_x;
    int8_t delta_y;
};

struct Entity {
    struct Position current_position;
    struct Position previous_position;
    struct PositionChangeRequest position_change_request;
    wchar_t character;
    enum Colors color;
    uint8_t stack_index;
};

union Cell {
    wchar_t character;
    struct Entity *entity_stack[ENTITY_STACK_INSIDE_CELL_LIMIT];
};

enum CellTag { CHARACTER, ENTITY_STACK };
struct TaggedCell {
    enum CellTag tag;
    union Cell cell;
};


struct World {
    uint16_t length;
    uint16_t width;
    struct TaggedCell cells[];
};

struct World *world_allocate(const uint16_t length, const uint16_t width);
struct World *init_world(const uint16_t length, const uint16_t width);
struct Entity *init_entity(const struct World *world, wchar_t character);
struct TaggedCell *get_world_map(struct World *world);

uint8_t progresive_position_change(struct Entity *entity, const struct World *world);


#endif