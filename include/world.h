#ifndef WORLD_H

#define WORLD_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#include "colors.h"
#include "resolution.h"

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
    STACK_LIMIT = 20,
    ENTITY_LIMIT = 128,
    UNINITIALIZED_8 = UINT8_MAX,
    UNINITIALIZED_16 = UINT16_MAX,
    UNINITIALIZED_32 = UINT32_MAX
};

struct Position {
    uint16_t x;
    uint16_t y;
};

struct Entity {
    // Cosas técnicas
    struct Position current_position;
    struct Position previous_position;
    uint8_t stack_index;

    // Atributos
    wchar_t character;
    enum Colors color;
};

enum Axis {Y_AXIS, X_AXIS};
enum PositionDelta {NEGATIVE, POSITIVE};
struct PositionChangeRequest {
    enum Axis axis;
    enum PositionDelta delta;
};

union InternalRequest {
    struct PositionChangeRequest position_change_request;
};

enum RequestKind { POSITION_REQUEST, ATTACK_REQUEST };
struct EntityRequest {
    struct Entity *requesting_entity;
    enum RequestKind kind;
    union InternalRequest request;
};

union InternalCell {
    wchar_t character;
    struct Entity *entity_stack[STACK_LIMIT];
};

enum CellKind { CHARACTER, ENTITY_STACK };
struct Cell {
    enum CellKind tag;
    union InternalCell cell;
};

struct VisibleWorld {
    struct Cell (*world)[WORLD_WIDTH];
    struct Position quadrant;
    bool is_new_quadrant;
    struct Position start_point;
    struct Position end_point;
    struct Entity *visible_entities[STACK_LIMIT];
    struct EntityRequest requests_stack[STACK_LIMIT];
};

void init_world(struct Cell world[WORLD_LENGTH][WORLD_WIDTH]);
void init_visible_world(struct VisibleWorld * const, struct Cell world[WORLD_LENGTH][WORLD_WIDTH]);
void init_entity(struct Entity *, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character);
void init_player(struct Entity * const new_player, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character, struct Resolution gameplay_resolution);

bool add_entity_to_cell_stack(struct Entity * const, struct Cell *cell);
bool remove_entity_from_cell_stack(struct Entity * const, struct Cell *cell);


#endif