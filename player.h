//
// Created by root on 10/31/22.
//

#ifndef WATKI_PLAYER_H
#define WATKI_PLAYER_H
#include "game_structures.h"
#include <time.h>
#include "beast.h"
int spaces_value(char **board);
void random_cord(char **board, int *x, int *y, struct game *game);
void set_player(char **board, struct contestant *player, struct game *game);
int move_player2(struct contestant *player, char **map,int id);
int CreateTreasure(char ***board, struct game *game);
void add_coins(char ***board ,struct game *game);
void create_coins(char ***board, char val, struct game *game);
void FindTreasure(struct game *game, struct contestant* player);
void MakeMap(char ***board);
void CopyBoard(char ***board, struct game *game, struct contestant *player);
struct LostTreasure *PushBack(struct ListLostTreasure *ll);
#endif //WATKI_PLAYER_H
