//
// Created by root on 10/31/22.
//
//ODDANE 22.11.2022 rok
#include <ncurses.h>
#include "player.h"
int spaces_value(char **board)
{
    int sum = 0;
    for (int i = 0; i < 25; ++i)
    {
        for (int j = 0; j < 51; ++j)
        {
            if(*(*(board+i)+j)==' ')
                sum++;
        }
    }
    return sum;
}

void create_coins(char ***board, char val, struct game *game)
{
    int x,y;
    random_cord(*board,&x,&y,game);

    *(*(*board + x)+ y) = val;
}


void random_cord(char **board, int *x, int *y, struct game *game)
{
    if(!x||!y|| spaces_value(board)==0)
        return;

    do{
        *x = rand()%23+1;
        *y = rand()%49+2;
        if(*(*(board + *x)+ *y)==' ' && *x != game->player1->x && *y != game->player1->y)
        {
            if(!Is2Player)
                return;
            if(Is2Player && game->player2->x != *x && game->player2->y != *y)
                return;
        }
    }while(1);
}
void set_player(char **board, struct contestant *player, struct game *game)
{

    int x,y;

    random_cord(board,&x,&y,game);

    player->x = x;
    player->y = y;

}

int CoinValue(char tile)
{
    switch (tile) {
        case 'c': return 1;
        case 't': return 10;
        case 'T': return 50;
        default: return 0;
    }
}

void add_coins(char ***board ,struct game *game)
{
    char tile = *(*(game->board+game->player1->x)+game->player1->y);
    int value = CoinValue(tile);

    game->player1->coins += value;
    if(value != 0)
        *(*(*board + game->player1->x)+ game->player1->y) = ' ';

    if(tile == 'A')
    {
        game->player1->CoinsBrought += game->player1->coins;
        game->player1->coins = 0;
    }

    if(Is2Player)
    {
        tile = *(*(game->board+game->player2->x)+game->player2->y);
        value = CoinValue(tile);
        game->player2->coins += value;
        if(value != 0)
            *(*(*board + game->player2->x)+ game->player2->y) = ' ';
        if(tile == 'A')
        {
            game->player2->CoinsBrought += game->player2->coins;
            game->player2->coins = 0;
        }
    }
}

void FindTreasure(struct game *game, struct contestant* player)
{
    struct LostTreasure *pom = game->Treasures.head;

    if(player->x==pom->x && player->y==pom->y)
    {
        player->coins += pom->value;
        pom->value = 0;
        pom->x = 200;
        pom->y = 200;
        *(*(game->board + player->x) + player->y) = ' ';
        return;
    }

    for (int i = 0; pom; ++i)
    {
        if(player->x==pom->x && player->y==pom->y)
        {
            player->coins += pom->value;
            pom->value = 0;
            pom->x = 200;
            pom->y = 200;
            *(*(game->board + player->x) + player->y) = ' ';
            break;
        }
        pom = pom->next;
    }
}

int move_player2(struct contestant *player, char **map, int id)
{

    int x = 2, y = 2;
    char **board = map;
    int keyboard = ' ';
    int keyboard2 = ' ';
    if(id==1)
        keyboard = ruch;
    else
        keyboard2 = RuchGracza;
    if(keyboard=='d'||keyboard=='D'||(int)keyboard2==261||keyboard=='w'||keyboard=='W'||(int)keyboard2==259
       ||keyboard=='a'||keyboard=='A'||(int)keyboard2==260||keyboard=='s'||keyboard=='S'||(int)keyboard2==258){
        if(*(*(board + x)+ y)=='#' && player->InBush)
        {
            player->InBush = FALSE;
            return 1;
        }
    }


    if(keyboard=='q'||keyboard=='Q')
        return 0;

    if(keyboard=='d'||keyboard=='D'||(int)keyboard2==261)
    {
        if(*(*(board + x)+ y + 1)!='w') {
            player->y++;
            if(*(*(board + x)+ y)=='#' && !player->InBush)
            {
                player->InBush = TRUE;
            }
        }
    }
    else if(keyboard=='w'||keyboard=='W'||(int)keyboard2==259)
    {
        if(*(*(board + x - 1)+ y)!='w')
        {
            player->x--;
            if(*(*(board + x)+ y)=='#' && !player->InBush)
            {
                player->InBush = TRUE;
            }
        }

    }
    else if(keyboard=='a'||keyboard=='A'||(int)keyboard2==260)
    {
        if(*(*(board + x)+ y - 1)!='w')
        {
            player->y--;
            if(*(*(board + x)+ y)=='#' && !player->InBush)
            {
                player->InBush = TRUE;
            }
        }

    }
    else if(keyboard=='s'||keyboard=='S'||(int)keyboard2==258)
    {
        if(*(*(board + x + 1)+ y)!='w')
        {
            player->x++;
            if(*(*(board + x)+ y)=='#' && !player->InBush)
            {
                player->InBush = TRUE;
            }
        }
    }
    return 1;
}

struct LostTreasure *PushBack(struct ListLostTreasure *ll)
{
        if(!ll->head){
            ll->head = calloc(1,sizeof(struct LostTreasure));
            if(!ll->head)
                return NULL;
            ll->tail = ll->head;
            ll->head->next = ll->tail;
            ll->tail->next = NULL;
            return ll->head;
        }
        else{
            struct LostTreasure* pom;
            struct LostTreasure* wsk = ll->head;
            while(wsk->next)
                wsk = wsk->next;
            wsk->next = calloc(1,sizeof(struct LostTreasure));
            if(!wsk->next)
                return NULL;
            pom = wsk->next;
            pom->next = NULL;
            ll->tail = pom;

        }
        return ll->tail;
    }


int CreateTreasure(char ***board, struct game *game)
{
    if(!game)
        return -1;
    game->player1->Deaths+=1;
    game->player2->Deaths+=1;
    if(game->player1[0].coins>0 || game->player2[0].coins>0)
    {
        struct LostTreasure *NewNode = PushBack(&game->Treasures);
        if(!NewNode)
            return -2;
        NewNode->x = game->player1[0].x;
        NewNode->y = game->player1[0].y;
        NewNode->value = game->player1[0].coins + game->player2[0].coins;
        game->player1->coins = 0;
        game->player2->coins = 0;
        *(*(*board + NewNode->x) + NewNode->y) = 'D';
    }
    set_player(game->board,&game->player1[0],game);
    set_player(game->board,&game->player2[0],game);
    return 1;
}

void MakeMap(char ***board)
{
    *board = calloc(5,sizeof(char*));
    if(!*board)
        return;
    for (int i = 0; i < 5; ++i) {
        *(*board + i) = calloc(5,sizeof(char));
        if(!*(*board + i))
        {
            for (int j = 0; j < i; ++j) {
                free(*(*board+j));
            }
            free(*board);
            return;
        }
    }
}

void CopyBoard(char ***board, struct game *game, struct contestant *player)
{
    int x = player->x;
    int y = player->y;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if(x-i<=0 || x+i>25 || y-j<=0 || y+j>51 || x+i-2<0 || y+j-2<0 || x==23 || x==24)
            {
                if(i==0||j==0||y + j-2<0 || x + i-2<0 || y + j-2>50 || x + i-2>24)
                    *(*(*board + i) + j) = 'w';
                else
                    *(*(*board + i) + j) = *(*(game->board + x + i-2) + y + j-2);

            }
            else
                *(*(*board + i) + j) = *(*(game->board + x + i-2) + y + j-2);
        }
    }
}

