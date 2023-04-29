//
// Created by root on 10/30/22.
//

#ifndef WATKI_BEAST_H
#define WATKI_BEAST_H
#include <stdbool.h>
#include <stdlib.h>
#include "game_structures.h"

enum Directions RandomDirectionForBeast(struct game *game);
int MoveBeast(struct game *game);
void setbeast(struct game *game);
void DestroyTreasure(struct ListLostTreasure *ll);
int ll_size(const struct ListLostTreasure* ll);
#endif //WATKI_BEAST_H
