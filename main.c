#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "beast.c"
#include "beast.h"
#include "game_structures.h"
#include "player.h"
#include "player.c"
#include "map.h"
#include "map.c"
struct game ServerGame;
void *PlayerMovement(void *arg)
{
    struct contestant *player = (struct contestant*) arg;
    player->PID = getpid();
    while(1)
    {
        pthread_mutex_lock(&ServerGame.Player1Mutex);
        flushinp();
        timeout(1000);
        ruch = getch();
        timeout(-1);
        if(ruch == ERR)
            ruch='u';
        if(ruch=='q'||ruch=='Q')
        {
            break;
        }

        move_player2(player,player->map,1);

    }

    return NULL;
}

void *SetBeast(void *arg)
{
    struct game *game = &ServerGame;
    setbeast(game);
    while (1)
    {
        pthread_mutex_lock(&game->BeastMutex);
        if(ruch=='q'||ruch=='Q')
        {
            break;
        }
        MoveBeast(game);
    }


    return NULL;
}



void *ForCommunication(void *arg)
{
    struct game *game = &ServerGame;
    mkfifo("ToServer",0777);
    mkfifo("ToPlayer",0777);
    int ToServer = open("ToServer",O_RDONLY);
    int ToPlayer = open("ToPlayer",O_WRONLY);
    ServerGame.HowManyPlayers = 2;
    ServerGame.player2 = calloc(1,sizeof(struct contestant));
    MakeMap(&ServerGame.player2->map);
    set_player(ServerGame.board, &ServerGame.player2[0],game);
    struct contestant *player2 = &game->player2[0];

    int ServerActive=1;
    Is2Player = 1;
    CopyBoard(&ServerGame.player2->map,&ServerGame,ServerGame.player2);
    while(1)
    {
        if(Is2Player)
        {
            pthread_mutex_lock(&game->Player2Mutex);

            if(ruch=='q'||ruch=='Q')
            {
                ServerActive = 0;
                Is2Player = 0;
                write(ToPlayer,&ServerActive,sizeof(int));
                break;
            }
            else
            {
                write(ToPlayer,&ServerActive,sizeof(int));
            }

            write(ToPlayer,&game->ServerPID,sizeof(int));
            write(ToPlayer,&game->Rounds,sizeof(int));
            write(ToPlayer,&player2->x,sizeof(int));
            write(ToPlayer,&player2->y,sizeof(int));
            write(ToPlayer,&player2->coins,sizeof(int));
            write(ToPlayer,&player2->CoinsBrought,sizeof(int));
            write(ToPlayer,&player2->Deaths,sizeof(int));

            for (int i = player2->x-2; i < player2->x+3; ++i)
            {
                for (int j = player2->y-2; j < player2->y+3; ++j)
                {
                    if(game->player1[0].x==i && game->player1[0].y==j)
                    {
                        char znak = '1';
                        write(ToPlayer, &znak, sizeof(char));
                    }
                    else if(game->beast1.x==i && game->beast1.y==j)
                    {
                        char znak = '*';
                        write(ToPlayer, &znak, sizeof(char));
                    }
                    else if(game->player2[0].x==i && game->player2[0].y==j) {
                        char znak = '2';
                        write(ToPlayer, &znak, sizeof(char));
                    }
                    else if( i>=0 && i<=24 && j>=0&&j<=50)
                        write(ToPlayer, &(*(*(game->board + i) + j)), sizeof(char));
                    else
                        write(ToPlayer, &(**game->board), sizeof(char));
                }
            }
            RuchGracza='u';
            read(ToServer,&game->player2[0].PID,sizeof(int));
            read(ToServer,&RuchGracza,sizeof(int));

            if(RuchGracza == 'q')
            {
                map_free(&ServerGame.player2->map,5);
                free(ServerGame.player2);
                ServerGame.player2 = NULL;
                ServerGame.HowManyPlayers = 1;
                Is2Player = 0;
                close(ToServer);
                close(ToPlayer);
                break;
            }
            move_player2(player2,player2->map,2);
        }
    }

    return NULL;
}
void *RecevingSignal()
{
    pthread_t Fifo;
    mkfifo("KeepOpenedToPlayer",0777);

    while(1)
    {
        if(ruch=='q'||ruch=='Q')
            break;
        int ProcessWantToJoin = 0;
        if(ServerGame.HowManyPlayers == 1)
        {
            int OpenedPlayer = open("KeepOpenedToPlayer",O_WRONLY);
            ProcessWantToJoin = 1;
            write(OpenedPlayer,&ProcessWantToJoin,sizeof(int));
            close(OpenedPlayer);
            ServerGame.HowManyPlayers = 2;
            pthread_create(&Fifo,NULL,ForCommunication,&ServerGame);
        }
        else{
            int OpenedPlayer = open("KeepOpenedToPlayer",O_WRONLY);
            ProcessWantToJoin = 0;
            write(OpenedPlayer,&ProcessWantToJoin,sizeof(int));
            close(OpenedPlayer);
        }
    }
}


int main() {
    //ODDANE 22.11.2022 rok
    signal(SIGPIPE,SIG_IGN);
    initscr();
    start_color();
    noecho();
    pthread_t Movement, Beast, FifoReceiver;
    srand(time(NULL));
    keypad(stdscr,true);


    memset(&ServerGame,0,sizeof(struct game));
    ServerGame.player2=NULL;
    ServerGame.player1=NULL;
    map_load(&ServerGame.board);
    ServerGame.player1 = calloc(1,sizeof(struct contestant));
    set_player(ServerGame.board, &ServerGame.player1[0],&ServerGame);
    MakeMap(&ServerGame.player1->map);
    ServerGame.ServerPID = getpid();
    ServerGame.Treasures.head = NULL;
    ServerGame.Treasures.tail = NULL;
    ServerGame.Rounds = 0;
    ServerGame.xCampsite = 11;
    ServerGame.yCampsite = 23;
    ServerGame.HowManyPlayers = 1;
    pthread_mutex_init(&ServerGame.MapMutex,NULL);
    pthread_mutex_init(&ServerGame.Player1Mutex,NULL);
    pthread_mutex_init(&ServerGame.BeastMutex,NULL);

    pthread_create(&Movement,NULL,PlayerMovement,ServerGame.player1);
    pthread_create(&Beast,NULL,SetBeast,&ServerGame);
    pthread_create(&FifoReceiver,NULL,RecevingSignal,&ServerGame);

    while(1) {
        CopyBoard(&ServerGame.player1->map,&ServerGame,ServerGame.player1);
        if(Is2Player)
            CopyBoard(&ServerGame.player2->map,&ServerGame,ServerGame.player2);
        pthread_mutex_unlock(&ServerGame.Player1Mutex);
        pthread_mutex_unlock(&ServerGame.BeastMutex);
        pthread_mutex_unlock(&ServerGame.Player2Mutex);
        sleep(1);
        ServerGame.Rounds++;
        CopyBoard(&ServerGame.player1->map,&ServerGame,ServerGame.player1);
        if(Is2Player)
            CopyBoard(&ServerGame.player2->map,&ServerGame,ServerGame.player2);
        if (Is2Player && ServerGame.player1[0].x == ServerGame.player2[0].x && ServerGame.player1[0].y == ServerGame.player2[0].y) {
            CreateTreasure(&ServerGame.board, &ServerGame);
        }
        if(*(*(ServerGame.board+ServerGame.player1->x)+ServerGame.player1->y) == 'D')
            FindTreasure(&ServerGame,ServerGame.player1);
        else if(Is2Player && *(*(ServerGame.board+ServerGame.player2->x)+ServerGame.player2->y) == 'D')
            FindTreasure(&ServerGame,ServerGame.player2);

        add_coins(&ServerGame.board,&ServerGame);

        if(ruch == 'c' || ruch == 't' || ruch == 'T')
            create_coins(&ServerGame.board,(char)ruch,&ServerGame);

        primary_colouring(&ServerGame.board, &ServerGame);
        print_statistics(&ServerGame);

        if (ruch == 'q' || ruch == 'Q') {
            break;
        }
    }

    free(ServerGame.player1);
    map_free(&ServerGame.player1->map,5);

    map_free(&ServerGame.board,25);
    DestroyTreasure(&ServerGame.Treasures);
    clear();
    endwin();
    return 0;
}
