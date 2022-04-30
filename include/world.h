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

enum Constants {
    WORLD_LENGTH = 100,
    WORLD_WIDTH = 200,
    STACK_LIMIT = 10,
    ENTITY_LIMIT = 128,
    UNINITIALIZED_8 = UINT8_MAX,
    UNINITIALIZED_32 = UINT32_MAX
};

struct Position {
    uint16_t x;
    uint16_t y;
};

struct Entity {
    struct Position current_position;
    struct Position previous_position;
    wchar_t character;
    enum Colors color;
    uint8_t stack_index;
};

enum Axis {Y_AXIS, X_AXIS};
enum PositionDelta {NEGATIVE, POSITIVE};
struct PositionChangeRequest {
    struct Entity *requesting_entity;
    enum Axis axis;
    enum PositionDelta delta;
};

union Cell {
    wchar_t character;
    struct Entity *entity_stack[STACK_LIMIT];
};

enum CellTag { CHARACTER, ENTITY_STACK };
struct TaggedCell {
    enum CellTag tag;
    union Cell cell;
};

void init_world(struct TaggedCell world[WORLD_LENGTH][WORLD_WIDTH]);
struct Entity *init_entity(struct TaggedCell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character);

bool add_entity_to_cell_stack(struct Entity * const, struct TaggedCell *cell);
bool remove_entity_from_cell_stack(struct Entity * const, struct TaggedCell *cell);


#endif