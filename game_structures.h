//
// Created by root on 10/30/22.
//

#ifndef WATKI_GAME_STRUCTURES_H
#define WATKI_GAME_STRUCTURES_H
int ruch = 'u';
int RuchGracza = 'u';
int Is2Player = 0;

struct contestant {
    int x, y, coins, Deaths, CoinsBrought, PID;
    bool InBush;
    char **map;
};
enum Directions{
    LEFT=1,
    UP=2,
    RIGHT=3,
    DOWN=4,
    NONE=5
};
struct beast {
    int x,y;
    enum Directions direction;
};
struct LostTreasure{
    int x,y,value;
    struct LostTreasure *next;
};

struct ListLostTreasure{
    struct LostTreasure *head;
    struct LostTreasure *tail;
};
struct game{
    struct contestant *player1;
    struct contestant *player2;
    struct beast beast1;
    int HowManyPlayers;
    char **board;
    struct ListLostTreasure Treasures;
    pthread_mutex_t Player1Mutex;
    pthread_mutex_t Player2Mutex;
    pthread_mutex_t BeastMutex;
    pthread_mutex_t MapMutex;
    int ServerPID;
    int xCampsite, yCampsite;
    int Rounds;
};
#endif //WATKI_GAME_STRUCTURES_H
