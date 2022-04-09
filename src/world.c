#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "world.h"


void fill_world(const uint16_t height, const uint16_t width, char (*map)[*]);
int32_t rand_min_max(int32_t min, int32_t max);


World *init_world(const uint16_t height, const uint16_t width) {
    World *world_struct = calloc(1, sizeof (*world_struct) + sizeof(char[height][width]));//Cambiar

    world_struct->height = height;
    world_struct->width = width;

    //Sintaxis del diablo, funciona como un arreglo bidimensional normal https://stackoverflow.com/questions/54709981/how-to-initiliaze-a-dynamic-2d-array-inside-a-struct-in-c
    char (*world)[width] = (char(*)[width]) world_struct->world_ptr;
    fill_world(height, width, *world);

    return world_struct;
}

void fill_world(const uint16_t height, const uint16_t width, char map[height][width]) {
    int row, column;
    for(row = 0; row < height; row++) {
        map[row][0] = '#';
        map[row][width - 1] = '#';
    }
    for(column = 0; column < width; column++) {
        map[0][column] = '#';
        map[height - 1][column] = '#';
    }
    for (row = 1; row < height - 1; row++) {
        for (column = 1; column < width - 1; column++) {
            map[row][column] = ' ';
        }
    }
}

Entity *init_entity(World *world_struct) {
    //Forma muy extraña de hacer una referencia al mapa del mundo
    char (*world_map)[world_struct->width] = (char(*)[world_struct->width]) world_struct->world_ptr;

    Entity *player = malloc(sizeof(Entity));
    player->y_coordinate = rand_min_max(1, world_struct->height - 2);
    player->x_coordinate = rand_min_max(1, world_struct->width - 2);
    player->character = '*';

    //world_map[player->y_coordinate][player->x_coordinate] = '*';    

    return player;
}

bool update_world(World *world_struct, Entity *entities[], uint16_t array_length) {
    char (*world_map)[world_struct->width] = (char(*)[world_struct->width]) world_struct->world_ptr;

    int i;
    for (i = 0; i < array_length; i++) {
        world_map[entities[i]->y_coordinate][entities[i]->x_coordinate] = entities[i]->character;
    }

    return 0;
}

int32_t rand_min_max(int32_t min, int32_t max) {
    srand(time(NULL));
    return (rand() % max) + min;
}