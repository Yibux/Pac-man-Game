#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>

int movement='b';
struct Contestant{
    int x,y,Coins,CoinsBrought, Deaths;
    int PID;
};

struct GameForPlayer{
    char **Board;
    struct Contestant Player1;
    pthread_mutex_t ForPlayer;
    int ServerPID;
    int ServerActive;
    int Rounds;
};

void color_tile(short id, int i, int j,char ch)
{
    attrset(COLOR_PAIR(id));
    move(i,j);
    addch(ch);
    attroff(COLOR_PAIR(id));
}

void primary_colouring(char ***board)
{
    if(has_colors())
    {
        if(start_color() == OK)
        {
            init_pair(98,COLOR_RED,COLOR_RED);
            attrset(COLOR_PAIR(98));

            for (int w = 0; w < 7; w++) {
                for (int i = 0; i < 7; ++i) {
                    if(w==0 || w==6 || i==0 || i==6)
                        color_tile(98,w+4,i+4,' ');
                }
            }
            attroff(COLOR_PAIR(98));
            init_pair(1, COLOR_BLACK, COLOR_BLACK);     //WALL
            init_pair(2, COLOR_WHITE, COLOR_MAGENTA);   //PLAYER
            init_pair(3, COLOR_WHITE, COLOR_WHITE);     //SPACE TO MOVE
            init_pair(4,COLOR_MAGENTA,COLOR_WHITE);     //BUSHES
            init_pair(5,COLOR_GREEN,COLOR_WHITE);       //STARS
            init_pair(6,COLOR_RED,COLOR_YELLOW);        //C t
            init_pair(7,COLOR_YELLOW,COLOR_GREEN);      //A
            init_pair(8,COLOR_YELLOW,COLOR_RED);        //T
            init_pair(9,COLOR_BLACK,COLOR_YELLOW);        //D


            for (int i = 0; i < 5; ++i)
            {
                for (int j = 0; j < 5; ++j)
                {
                    char one_tile = *(*(*board + i)+ j);

                    switch (one_tile)
                    {
                        case 'w':
                            color_tile(1,i+5,j+5,one_tile);
                            break;
                        case '*':
                            color_tile(5,i+5,j+5,one_tile);
                            break;
                        case ' ':
                            color_tile(3,i+5,j+5,one_tile);
                            break;
                        case 'c': case 't':
                            color_tile(6,i+5,j+5,one_tile);
                            break;
                        case '#':
                            color_tile(4,i+5,j+5,one_tile);
                            break;
                        case 'A':
                            color_tile(7,i+5,j+5,one_tile);
                            break;
                        case 'T':
                            color_tile(8,i+5,j+5,one_tile);
                            break;
                        case 'D':
                            color_tile(9,i+5,j+5,one_tile);
                            break;
                        case '1': case '2':
                            color_tile(2,i+5,j+5,one_tile);
                            break;
                        default:
                            break;
                    }

                }
            }

            refresh();

        }
        else
        {
            addstr("Cannot start colours\n");
            refresh();
        }
    }
    else
    {
        addstr("Not colour capable\n");
        refresh();
    }
}




void print_statistics(struct GameForPlayer *game)
{


    mvprintw(5,13,"Server's PID: %d         ",game->ServerPID);
    mvprintw(6,13,"Campsite X/Y: unknown         ");
    mvprintw(7,13,"Round number: %d         ",game->Rounds);


    mvprintw(9,13,"Player:         ");
    mvprintw(10,13,"Type            HUMAN         ");
    mvprintw(11,13,"Curr X/Y:       %d/%d         ",game->Player1.x,game->Player1.y);
    mvprintw(12,13,"Deaths          %d         ",game->Player1.Deaths);
    mvprintw(14,13,"Coins found     %d         ", game->Player1.Coins);
    mvprintw(15,13,"Coins brought   %d         ", game->Player1.CoinsBrought);
    mvprintw(19,13,"Legend");
    init_pair(1, COLOR_BLACK, COLOR_BLACK);     //WALL
    init_pair(2, COLOR_WHITE, COLOR_MAGENTA);   //PLAYER
    init_pair(3, COLOR_WHITE, COLOR_WHITE);     //SPACE TO MOVE
    init_pair(4,COLOR_MAGENTA,COLOR_WHITE);     //BUSHES
    init_pair(5,COLOR_GREEN,COLOR_WHITE);       //STARS
    init_pair(6,COLOR_RED,COLOR_YELLOW);        //C t
    init_pair(7,COLOR_YELLOW,COLOR_GREEN);      //A
    init_pair(8,COLOR_YELLOW,COLOR_RED);        //T
    init_pair(9,COLOR_BLACK,COLOR_YELLOW);        //D
    color_tile(2,20,13,'1');
    color_tile(2,20,14,'2');
    color_tile(2,20,15,'3');
    color_tile(2,20,16,'4');

    mvprintw(20,18,"- players");

    color_tile(1,21,13,'w');
    mvprintw(21,18,"- wall");

    color_tile(4,22,13,'#');
    mvprintw(22,18,"- bushes (slow down)");

    color_tile(5,23,13,'*');
    mvprintw(23,18,"- wild beast");

    color_tile(6,24,13,'c');
    mvprintw(24,18,"- one coin");

    color_tile(9,25,13,'D');
    mvprintw(25,18,"- dropped treasure");

    color_tile(6,26,13,'t');
    mvprintw(26,18,"- treasure (10 coins)");

    color_tile(8,27,13,'T');
    mvprintw(27,18,"- large treasure (50 coins)");

    color_tile(7,28,13,'A');
    mvprintw(28,18,"- campsite");

    move(0,0);
    refresh();
}

void CreateMap(char ***board)
{

    *board = calloc(5,sizeof(char*));
    if(!*board)
        return;

    for (int i = 0; i < 5; ++i)
    {
      *(*board + i) = calloc(5,sizeof(char));
      if(!*(*board + i))
      {
          for (int j = 0; j < i; ++j) {
              free(*(*board + j));
          }
          free(*board);
          return;
      }
    }
}

void *PlayerMovement(void *arg)
{
    struct GameForPlayer *game = (struct GameForPlayer*)arg;
    while(1)
    {
        pthread_mutex_lock(&game->ForPlayer);
        flushinp();
        movement = getch();
        if(movement=='q'||movement=='Q')
        {
            break;
        }
        pthread_mutex_unlock(&game->ForPlayer);
    }

    return NULL;
}

int main()
{
    initscr();
    start_color();
    noecho();
    keypad(stdscr,true);
    printw("Oczekiwanie na serwer");
    refresh();

    int ReadingInfo =5;
    int CanJoin = 0;
    int OpenedPlayer = open("KeepOpenedToPlayer",O_RDONLY);

    ReadingInfo = read(OpenedPlayer,&CanJoin,sizeof(int));
    if(ReadingInfo == -1)
    {
        printw("Nie dziala fifo");
        return 0;
    }
    if(CanJoin == 0)
    {
        mvprintw(0,0,"Serwer Pelny                ");
        refresh();
        return 0;
    }

    int ToServer = open("ToServer",O_WRONLY);
    int ToPlayer = open("ToPlayer",O_RDONLY);
    mvprintw(0,0,"                            ");
    refresh();
    struct GameForPlayer game;
    CreateMap(&game.Board);

    int PlayerPID = getpid();
    game.Player1.PID = PlayerPID;
    pthread_t Movement;
    pthread_mutex_init(&game.ForPlayer,NULL);
    pthread_create(&Movement,NULL,PlayerMovement,&game);

    while(movement!='q'||movement!='Q')
    {
        ReadingInfo = read(ToPlayer,&game.ServerActive,sizeof(int));
        if(ReadingInfo == -1)
        {
            printw("Nie dziala fifo");
            break;
        }
        if(game.ServerActive == 0)
            break;
        ReadingInfo = read(ToPlayer,&game.ServerPID,sizeof(int));
        if(ReadingInfo == -1)
        {
            printw("Nie dziala fifo");
            break;
        }
        ReadingInfo = read(ToPlayer,&game.Rounds,sizeof(int));
        if(ReadingInfo == -1)
        {
            printw("Nie dziala fifo");
            break;
        }
        ReadingInfo = read(ToPlayer,&game.Player1.x,sizeof(int));
        if(ReadingInfo == -1)
        {
            printw("Nie dziala fifo");
            break;
        }

        ReadingInfo = read(ToPlayer,&game.Player1.y,sizeof(int));
        if(ReadingInfo == -1)
        {
            printw("Nie dziala fifo");
            break;
        }

        ReadingInfo = read(ToPlayer,&game.Player1.Coins,sizeof(int));
        if(ReadingInfo == -1)
        {
            printw("Nie dziala fifo");
            break;
        }
        ReadingInfo = read(ToPlayer,&game.Player1.CoinsBrought,sizeof(int));
        if(ReadingInfo == -1)
        {
            printw("Nie dziala fifo");
            break;
        }
        ReadingInfo = read(ToPlayer,&game.Player1.Deaths,sizeof(int));
        if(ReadingInfo == -1)
        {
            printw("Nie dziala fifo");
            break;
        }


        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                ReadingInfo = read(ToPlayer,&(*(*(game.Board + i) + j)),sizeof(char));
                if(ReadingInfo == -1)
                {
                    printw("Nie dziala fifo");
                    break;
                }
            }
        }
        write(ToServer,&PlayerPID,sizeof(int));
        primary_colouring(&game.Board);
        print_statistics(&game);
        if(movement=='q'||movement=='Q')
        {
            write(ToServer,&movement,sizeof(int));
            close(ToServer);
            close(ToPlayer);
            return 0;
        }
        write(ToServer,&movement,sizeof(int));
        movement = 'u';
        primary_colouring(&game.Board);
        print_statistics(&game);


    }


    close(ToServer);
    close(ToPlayer);
    clear();
    endwin();

    return 0;
}
