#ifndef WORLD_H

#define WORLD_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

typedef struct {
    uint16_t x;
    uint16_t y;
} Position;

typedef struct {
    uint16_t length;
    uint16_t width;
    wchar_t raw_table[]; // No tocar directamente
} DynamicTable;
DynamicTable *table_allocate(const uint16_t height, const uint16_t width);

// Sinónimos
typedef DynamicTable World;
typedef DynamicTable Structure;
World *init_world(const uint16_t height, const uint16_t width);

typedef struct {
    Position current_position;
    Position previous_position;
    wchar_t character;
} Entity;
Entity *init_entity(const World *world, wchar_t character);
uint8_t request_change_of_position(const int8_t delta_x, const int8_t delta_y, Entity *entity, const World *world_struct);

bool update_world(World *world_struct, Entity *entities[], uint16_t array_length);


#endif