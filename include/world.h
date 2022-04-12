#ifndef WORLD_H

#define WORLD_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t x;
    uint16_t y;
} Position;
Position *init_position();
Position *init_and_set_position(uint16_t x, uint16_t y);
void position_set(uint16_t x, uint16_t y, Position *position_struct);

typedef struct {
    uint16_t height;
    uint16_t width;
    char world_ptr[]; //Se supone que esto es un puntero a un arreglo bidimensional
} World;
World *init_world(const uint16_t height, const uint16_t width);

typedef struct {
    Position *current_position;
    Position *previous_position;
    char character;
} Entity;
Entity *init_entity(const World *world, char character);
uint8_t request_change_of_position(const int8_t delta_x, const int8_t delta_y, const Entity *entity, const World *world_struct);

bool update_world(World *world_struct, Entity *entities[], uint16_t array_length);


#endif