//
// Created by root on 10/31/22.
//

#ifndef WATKI_MAP_H
#define WATKI_MAP_H
#include "beast.h"
#include "player.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void map_display(char **board);
void map_free(char ***board, int size);
void map_load(char ***board);
void print_statistics(struct game *game);
void primary_colouring(char ***board, struct game *game);
void color_tile(short id, int i, int j,char ch);
#endif //WATKI_MAP_H
