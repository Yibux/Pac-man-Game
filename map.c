//
// Created by root on 10/31/22.
//

#include "map.h"

void color_tile(short id, int i, int j,char ch)
{
    attrset(COLOR_PAIR(id));
    move(i,j);
    addch(ch);
    attroff(COLOR_PAIR(id));
}

void primary_colouring(char ***board, struct game *game)
{
    if(has_colors())
    {
        if(start_color() == OK)
        {
            init_pair(1, COLOR_BLACK, COLOR_BLACK);     //WALL
            init_pair(2, COLOR_WHITE, COLOR_MAGENTA);   //PLAYER
            init_pair(3, COLOR_WHITE, COLOR_WHITE);     //SPACE TO MOVE
            init_pair(4,COLOR_MAGENTA,COLOR_WHITE);     //BUSHES
            init_pair(5,COLOR_GREEN,COLOR_WHITE);       //STARS
            init_pair(6,COLOR_RED,COLOR_YELLOW);        //C t
            init_pair(7,COLOR_YELLOW,COLOR_GREEN);      //A
            init_pair(8,COLOR_YELLOW,COLOR_RED);        //T
            init_pair(9,COLOR_BLACK,COLOR_YELLOW);        //D

            struct contestant *player = &game->player1[0];

            int x = player[0].x, y = player[0].y;
            int x2 = 0, y2 = 0;
            if(Is2Player)
            {
                struct contestant *player2 = &game->player2[0];
                x2 = player2[0].x;
                y2 = player2[0].y;
            }
            char TempPlayer1 = *(*(*board + x)+ y);
            char TempBeast = *(*(*board + game->beast1.x)+ game->beast1.y);
            char TempPlayer2 = 'x';
            if(Is2Player)
            {
                TempPlayer2 = *(*(*board + x2)+ y2);
            }
            *(*(*board + x)+ y) = '1';
            *(*(*board + game->beast1.x)+ game->beast1.y) = '*';
            mvprintw(5,60,"x2: %d y2: %d",x2,y2);
            if(Is2Player) *(*(*board + x2)+ y2) = '2';

            for (int i = 0; i < 25; ++i)
            {
                for (int j = 0; j < 51; ++j)
                {
                    char one_tile = *(*(*board + i)+ j);

                    switch (one_tile)
                    {
                        case 'w':
                            color_tile(1,i,j,one_tile);
                            break;
                        case '*':
                            color_tile(5,game->beast1.x,game->beast1.y,one_tile);
                            break;
                        case ' ':
                            color_tile(3,i,j,one_tile);
                            break;
                        case 'c': case 't':
                            color_tile(6,i,j,one_tile);
                            break;
                        case '#':
                            color_tile(4,i,j,one_tile);
                            break;
                        case 'A':
                            color_tile(7,i,j,one_tile);
                            break;
                        case 'T':
                            color_tile(8,i,j,one_tile);
                            break;
                        case 'D':
                            color_tile(9,i,j,one_tile);
                            break;
                        case '1':
                            color_tile(2,x,y,'1');
                            break;
                        case '2':
                            if(Is2Player==1) color_tile(2,x2,y2,'2');
                            break;
                        default:
                            break;
                    }

                }
            }

            *(*(*board + x)+ y) = TempPlayer1;
            *(*(*board + game->beast1.x)+ game->beast1.y) = TempBeast;
            if(Is2Player) *(*(*board + x2)+ y2) = TempPlayer2;

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




void print_statistics(struct game *game)
{

    mvprintw(5,60,"Server's PID: %d         ",game->ServerPID);
    mvprintw(6,60,"Campsite X/Y: %d/%d         ",game->xCampsite,game->yCampsite);
    mvprintw(7,60,"Round number: %d         ",game->Rounds);
    mvprintw(8,60,"Players: %d         ",game->HowManyPlayers);

    mvprintw(9,60,"Parameter:   Player1  Player2         ");


    if(Is2Player)
    {
        mvprintw(10,60,"PID:         %d      %d         ", game->player1[0].PID, game->player2[0].PID);
        mvprintw(11,60,"Type         HUMAN    HUMAN         ");
        mvprintw(12,60,"Curr X/Y     %d/%d    %d/%d         ",game->player1[0].x,game->player1[0].y,
                 game->player2[0].x,game->player2[0].y);
        mvprintw(13,60,"Deaths        %d        %d         ",game->player1[0].Deaths,game->player2[0].Deaths);
        mvprintw(15,60,"    carried     %d  %d         ", game->player1[0].coins, game->player2[0].coins);
        mvprintw(16,60,"    brought     %d  %d         ", game->player1[0].CoinsBrought, game->player2[0].CoinsBrought);
    }

    else
    {
        mvprintw(10,60,"PID:         %d       -         ", game->player1[0].PID);
        mvprintw(11,60,"Type         HUMAN      -         ");
        mvprintw(12,60,"Curr X/Y    %d/%d     -/-           ",game->player1[0].x,game->player1[0].y);
        mvprintw(13,60,"Deaths       %d          -           ",game->player1[0].Deaths);
        mvprintw(15,60,"    carried     %d  -           ", game->player1[0].coins);
        mvprintw(16,60,"    brought     %d  -           ", game->player1[0].CoinsBrought);
    }

    mvprintw(14,60,"Coins");
    mvprintw(19,60,"Legend");
    init_pair(1, COLOR_BLACK, COLOR_BLACK);     //WALL
    init_pair(2, COLOR_WHITE, COLOR_MAGENTA);   //PLAYER
    init_pair(3, COLOR_WHITE, COLOR_WHITE);     //SPACE TO MOVE
    init_pair(4,COLOR_MAGENTA,COLOR_WHITE);     //BUSHES
    init_pair(5,COLOR_GREEN,COLOR_WHITE);       //STARS
    init_pair(6,COLOR_RED,COLOR_YELLOW);        //C t
    init_pair(7,COLOR_YELLOW,COLOR_GREEN);      //A
    init_pair(8,COLOR_YELLOW,COLOR_RED);        //T
    init_pair(9,COLOR_BLACK,COLOR_YELLOW);        //D
    color_tile(2,20,60,'1');
    color_tile(2,20,61,'2');
    color_tile(2,20,62,'3');
    color_tile(2,20,63,'4');

    mvprintw(20,65,"- players");

    color_tile(1,21,60,'w');
    mvprintw(21,65,"- wall");

    color_tile(4,22,60,'#');
    mvprintw(22,65,"- bushes (slow down)");

    color_tile(5,23,60,'*');
    mvprintw(23,65,"- wild beast");

    color_tile(6,24,60,'c');
    mvprintw(24,65,"- one coin");

    color_tile(9,25,60,'D');
    mvprintw(25,65,"- dropped treasure");

    color_tile(6,26,60,'t');
    mvprintw(26,65,"- treasure (10 coins)");

    color_tile(8,27,60,'T');
    mvprintw(27,65,"- large treasure (50 coins)");

    color_tile(7,28,60,'A');
    mvprintw(28,65,"- campsite");

    move(0,0);
    refresh();
}

void map_load(char ***board)
{
    FILE *f = fopen("mapa.txt","r");
    if(!f)
        return;
    char buf[53];
    *board = calloc(25,sizeof(char*));
    if(!*board)
        return;
    for (int i = 0; i < 25; ++i)
    {
        *(*board + i) = calloc(52,sizeof(char));
        if(!*(*board + i))
            return;
    }
    int j=0,k=0;
    for (int i = 0; i < 25; ++i)
    {
        memset(buf,0,53);
        fgets(buf,52,f);
        if(*buf=='\r'||*buf=='\n'){
            i--;
            continue;
        }
        for (int l = 0; l < strlen(buf); ++l, k++) {
            *(*(*board+j)+k) = *(buf+l);
        }
        j++;
        k=0;
    }
    fclose(f);
}

void map_free(char ***board, int size)
{
    for (int i = 0; i < size; ++i)
    {
        free(*(*board + i));
    }
    free(*board);
}

void map_display(char **board)
{
    for (int i = 0; i < 25; ++i) {
        printf("%s\n",*(board+i));
    }
}
