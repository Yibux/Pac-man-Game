//
// Created by root on 10/30/22.
//

#include "beast.h"
#include "player.h"
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>



void setbeast(struct game *game)
{
    int x,y;
    random_cord(game->board,&x,&y,game);
    game->beast1.x = x;
    game->beast1.y = y;
    game->beast1.direction=NONE;
}

int ll_size(const struct ListLostTreasure* ll){
    if(!ll)
        return -1;
    struct LostTreasure* pom = ll->head;
    int licznik = 0;
    while(pom){
        pom = pom->next;
        licznik++;
    }

    return licznik;
}

void DestroyTreasure(struct ListLostTreasure *ll)
{
    if(!ll) return;
    struct LostTreasure* pom = ll->head;
    int dlugosc = ll_size(ll);
    for (int i = dlugosc-1; i >= 0; --i) {
        for (int j = 0; j < i; ++j) {
            pom = pom->next;
        }
        free(pom);
        pom = NULL;
        pom = ll->head;
    }
    ll->head = NULL;
}

enum Directions RandomDirectionForBeast(struct game *game)
{

    struct beast *bestia = &game->beast1;

    int direction = game->beast1.direction;
    srand(time(NULL));

    do {
        if(bestia->direction==NONE)
            direction = rand()%4+1;

        if(direction == LEFT)
        {
            if (*(*(game->board + bestia->x) + bestia->y - 1) != 'w')
                return direction;
        }
        else if(direction == UP)
        {
            if (*(*(game->board + bestia->x - 1) + bestia->y) != 'w')
                return direction;
        }
        else if(direction == RIGHT)
        {
            if (*(*(game->board + bestia->x) + bestia->y + 1) != 'w')
                return direction;
        }
        else if(direction == DOWN)
        {
            if (*(*(game->board + bestia->x + 1) + bestia->y) != 'w')
                return direction;
        }
    }while(1);
}


int MoveBeast(struct game *game)
{

    struct beast *bestia = &game->beast1;
    char **board = game->board;
    int Xb = game->beast1.x;
    int Yb = game->beast1.y;
    int flag=0;
    int distance, distance2;
    //Left i right
    if(Is2Player)
    {
        if(Xb==game->player2[0].x || abs(game->player2[0].x-Xb) == 1)
        {
            distance2 = game->player2[0].y-Yb;
            if(distance2<0 && distance2>-3)
            {

                //bestia jest po prawej gracz po lewej
                if(*(*(board+Xb)+Yb-1)!='w')
                {
                    flag=1;
                    bestia->direction = LEFT;
                }
            }
            else if(distance2>0 && distance2<3)
            {
                if(*(*(board+Xb)+Yb+1)!='w')
                {
                    flag=1;
                    bestia->direction = RIGHT;
                }
            }

        }
        else if(Yb==game->player2[0].y || abs(game->player2[0].y-Yb) == 1)
        {
            distance2 = game->player2[0].x-Xb;
            if(distance2<0 && distance2>-3)
            {
                //bestia
                //gracz
                if(*(*(board+Xb+1)+Yb)!='w')
                {
                    flag=1;
                    bestia->direction = DOWN;
                }
            }
                //odwrotnie do poprzedniego
            else if(distance2>0 && distance2<3) //|| Xb==game->player1[1].x-Xb>0
            {
                if(*(*(board+Xb - 1)+Yb)!='w')
                {
                    flag=1;
                    bestia->direction = UP;
                }
            }
        }
    }
    //p cos bestia

    if(Xb==game->player1[0].x || abs(game->player1[0].x-Xb) == 1)
    {
        distance = game->player1[0].y-Yb;

        if(distance<0 && distance>-3)
        {

            //bestia jest po prawej gracz po lewej
            if(*(*(board+Xb)+Yb-1)!='w')
            {
                flag=1;
                bestia->direction = LEFT;
            }
        }
            //odwrotnie do poprzedniego
        else if(distance>0 && distance<3)
        {
            if(*(*(board+Xb)+Yb+1)!='w')
            {
                flag=1;
                bestia->direction = RIGHT;
            }
        }
    }
    else if(Yb==game->player1[0].y || abs(game->player1[0].y-Yb) == 1)
    {
        distance = game->player1[0].x-Xb;
        if(distance<0 && distance>-3)
        {
            //bestia
            //gracz
            if(*(*(board+Xb+1)+Yb)!='w')
            {
                flag=1;
                bestia->direction = DOWN;
            }
        }
            //odwrotnie do poprzedniego
        else if(distance>0 && distance<3) //|| Xb==game->player1[1].x-Xb>0
        {
            if(*(*(board+Xb - 1)+Yb)!='w')
            {
                flag=1;
                bestia->direction = UP;
            }
        }
    }
    if(game->player1[0].x==Xb && game->player1[0].y==Yb )
    {

        if(game->player1[0].coins>0)
        {
            struct LostTreasure *NewNode = PushBack(&game->Treasures);
            if(!NewNode)
                return -2;
            NewNode->x = game->player1[0].x;
            NewNode->y = game->player1[0].y;
            NewNode->value = game->player1[0].coins;
            game->player1[0].coins = 0;
            *(*(game->board + NewNode->x) + NewNode->y) = 'D';
        }
        game->player1[0].Deaths += 1;
        set_player(game->board,game->player1,game);
    }
    else if(Is2Player && game->player2[0].x==Xb && game->player2[0].y==Yb)
    {
        if(game->player2[0].coins>0)
        {
            struct LostTreasure *NewNode = PushBack(&game->Treasures);
            if(!NewNode)
                return -2;
            NewNode->x = game->player2[0].x;
            NewNode->y = game->player2[0].y;
            NewNode->value = game->player2[0].coins;
            game->player2[0].coins = 0;
            *(*(game->board + NewNode->x) + NewNode->y) = 'D';
        }
        if(Is2Player)
            game->player2[0].Deaths += 1;
        set_player(game->board,game->player2,game);
    }
    if(!flag)
    {
        bestia->direction = RandomDirectionForBeast(game);
    }


    switch (bestia->direction)
    {
        case UP: bestia->x--; break;  //2
        case RIGHT: bestia->y++; break; //3
        case DOWN: bestia->x++; break;    //4
        case LEFT: bestia->y--; break;  //1
        default: break;
    }

    bestia->direction = NONE;
    return 1;
}


