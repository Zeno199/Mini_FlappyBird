#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
/* 音效記得要加入這個 */
#include "audio.h"
/* 另外還有底下的設定 */
/* For sound effect: In [Project Options]->[Parameters]->[Linker] add the parameter -lwinmm */

#include "console_draw3.h"
#include "kb_input2.h"
#include "font.h"
#include "image.h"
#include "framework.h"

HANDLE Stdin;
INPUT_RECORD ir;
DWORD Read = 0;

#define OFFSET_X  85
#define OFFSET_Y  35
#define REFRESH_DURATION 5
#define NUM_KEYS 7
#define REFRESH_RATE 10
#define REFRESH_DURATION 5
#define BACK_HEIGHT 125


int compare(const void*a , const void *b)
{
    int *va = (int*)a;
    int *vb = (int*)b;
    return *vb-*va;
}
int my_atoi(const char* snum)
{
    int idx, strIdx = 0, accum = 0, numIsNeg = 0;
    const unsigned int NUMLEN = (int)strlen(snum);

    /* Check if negative number and flag it. */
    if(snum[0] == 0x2d)
        numIsNeg = 1;

    for(idx = NUMLEN - 1; idx >= 0; idx--)
    {
        /* Only process numbers from 0 through 9. */
        if(snum[strIdx] >= 0x30 && snum[strIdx] <= 0x39)
            accum += (snum[strIdx] - 0x30) * pow(10, idx);

        strIdx++;
    }

    /* Check flag to see if originally passed -ve number and convert result if so. */
    if(!numIsNeg)
        return accum;
    else
        return accum * -1;
}

void	initializeKeyInput(void)
{
   if ((Stdin = GetStdHandle (STD_INPUT_HANDLE)) == NULL)
      exit(1);
   SetConsoleMode (Stdin, ENABLE_LINE_INPUT);
}

int waitForKeyDown(float sec)
{
   if (WAIT_TIMEOUT == WaitForSingleObject (Stdin, (int)(sec*1000) ))
      return 0;
   else
      return 1;
}

int getKeyEventASCII(void)
{
   ReadConsoleInput(Stdin, &ir, 1, &Read);
   FlushConsoleInputBuffer(Stdin);
   if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
      return ir.Event.KeyEvent.uChar.AsciiChar;
   else
      return -1;
}

int getKeyEventVirtual(void)
{
   ReadConsoleInput(Stdin, &ir, 1, &Read);
   FlushConsoleInputBuffer(Stdin);
   if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
      return ir.Event.KeyEvent.wVirtualKeyCode;
   else
      return -1;
}



typedef struct _bird{
    int x;
    int y;
    int state;
    Image *initial;
    Image *up[3];
    Image *down[3];
    int moveup_mode;
    int fallingTime;
    int flyingTime;
} Bird;

typedef struct _tubeTop{
    int x;
    int y;
    Image *tubeImg;
} Tube_top;

typedef struct _tubeButtom{
    int x;
    int y;
    Image *tubeImg;
} Tube_buttom;


int show_tube(Tube_top *tbt,Tube_buttom *tbb, int diff);
void ran_tube(Tube_top *tbt,Tube_buttom *tbb);
int check_collision(Bird *bd, Tube_top *tbt,Tube_buttom *tbb);


//void show_bird(Bird *bd , Menu *m, char *str); //return wing's state
int check_collision(Bird *bd, Tube_top *tbt,Tube_buttom *tbb);
int stopfunction();
void gameoverfunction(int number);


int Is_difficult = 0;


void show_bird(Bird *bd, Menu *m, int number)
{

    char str[40] = {'\0'};
    bd->state += 1;
    if (bd->state==3) bd->state=0;

    if (bd->moveup_mode == 1) {
        bd->fallingTime = 0;
        bd->flyingTime += 1;
        if(bd->flyingTime > 6) bd->flyingTime = 6;
        bd->y -= bd->flyingTime*2; //move up_bird
        sprintf(str, "%10d",  number); // big string, score
        putStringLarge(m->large_font, 30, 20, str, 14);
        show_image(bd->up[bd->state%3], bd->x, bd->y);
        drawCmdWindow();
        int i,flag=0;
        for(i=0;i<100;i++){
            if (KEY_DOWN(VK_SPACE)) flag = 1;
            if (KEY_DOWN(VK_UP)) flag = 1;
        }
        if (flag) {
            bd->moveup_mode = 1;
        }else{
            bd->moveup_mode = 0;
        }
    }else{
        bd->flyingTime = 1;
        bd->fallingTime += 1;
        if(bd->fallingTime > 10) bd->fallingTime = 10;
        bd->y += bd->fallingTime/2;
        sprintf(str, "%10d",  number); // big string, score
        putStringLarge(m->large_font, 30, 20, str, 14);
        show_image(bd->up[bd->state%3], bd->x, bd->y);
        drawCmdWindow();
    }

}


/*p1*/
int player_one(void)
{
    int IsEnding = 0;
    int inst =0, k ;
    Audio travel;
    Audio chicken;
    openAudioFile("Traveler.wav", &travel);
    openAudioFile("chicken1.wav", &chicken);


    Tube_top* mintop = NULL;
    Tube_buttom* minbut = NULL;

    clock_t startc;
    int game_tick = 0;

    int number= 0;

    Image *background;
    background = read_image("background_wide.pixel", "background_wide.color");

    Bird red_bird;
    red_bird.state = 0;
    red_bird.initial = read_image("red_1.pixel", "red_1.color");
    red_bird.up[0] = read_image("red_1.pixel", "red_1.color");
    red_bird.up[1] = read_image("red_2.pixel", "red_2.color");
    red_bird.up[2] = read_image("red_3.pixel", "red_3.color");
    red_bird.x = 20;
    red_bird.y = 55;
    red_bird.moveup_mode = 0;
    red_bird.fallingTime = 0;
    red_bird.flyingTime = 0;



    Menu scoreMenu;
    setMainmenu(&scoreMenu);

    int rand_y = (rand()%20);

    /*new*/

    Tube_top top1;
    top1.tubeImg = read_image("top.pixel", "top.color");
    top1.x = 130;
    top1.y = -120 + rand_y;

    Tube_buttom but1;
    but1.tubeImg = read_image("buttom.pixel", "buttom.color");
    but1.x = 130;
    but1.y = 80 + rand_y;

    rand_y = (rand()%20);

    Tube_top top2;
    top2.tubeImg = read_image("top.pixel", "top.color");
    top2.x = 220;
    top2.y = -120 + rand_y;

    Tube_buttom but2;
    but2.tubeImg = read_image("buttom.pixel", "buttom.color");
    but2.x = 220;
    but2.y = 80 + rand_y;

    rand_y = (rand()%20);

    Tube_top top3;
    top3.tubeImg = read_image("top.pixel", "top.color");
    top3.x = 310;
    top3.y = -120 + rand_y;

    Tube_buttom but3;
    but3.tubeImg = read_image("buttom.pixel", "buttom.color");
    but3.x = 310;
    but3.y = 80 + rand_y;

    clearScreen();
    startc = clock();

    while (!IsEnding) { /* game loop */
             //playAudio(&travel);
              playAudio(&travel);

        if ((double)(clock()-startc) > 0.01*CLOCKS_PER_SEC) {
            game_tick++;  /* game_tick 0.01 秒 */
            startc = clock();
        }

        if (KEY_DOWN(VK_SPACE)) {
            red_bird.moveup_mode = 1;
        }
        if(KEY_DOWN((VK_H))) Is_difficult = 1;

         if (KEY_DOWN(VK_Q)) {
            IsEnding = 1;
            break;
        }

        if (KEY_DOWN(VK_ESCAPE)) {
            saveScreen();
            clearScreen();
            inst = stopfunction();
            if(inst == 0)
            {
                restoreScreen();
                drawCmdWindow();
            }
            if(inst ==2)
            {
                pauseAudio(&travel);
                return 0;
            }
            else if(inst == 3)
            {
                pauseAudio(&travel);
                return 1;
            }
        }

        if (game_tick > REFRESH_DURATION) {
            game_tick = 0;
            clearScreen();

            show_image(background, 0, 0);
            number+=show_tube(&top1,&but1, Is_difficult);
            number+=show_tube(&top2,&but2, Is_difficult);
            number+=show_tube(&top3,&but3, Is_difficult);
            show_bird(&red_bird, &scoreMenu,number);
            //show_bird(&blue_bird);
            if(top1.x<top2.x && top1.x<top3.x){
                mintop = &top1;
                minbut = &but1;
            }else if(top2.x<top3.x && top2.x<top1.x){
                mintop = &top2;
                minbut = &but2;
            }else{
                mintop = &top3;
                minbut = &but3;
            }
            IsEnding = check_collision(&red_bird,mintop,minbut);
            if(!IsEnding) drawCmdWindow();  /* update window immediately */
        }

    }

    playAudio(&chicken);
    clearScreen();


    /*WRITE RANK*/
    FILE *open_rank;
    open_rank = fopen("rank.txt", "r");
    int j = 0, target = 0;
    char ranks[50]={'\0'};

    int save_array[6]={-1};


    for(j = 0;j<5;j++)
    {
        //fgets(ranks, 50, open_rank);
        fscanf(open_rank,"%s",ranks);
        save_array[j] = my_atoi(ranks);
    }
    save_array[j] = number;

    fclose(open_rank);
    qsort(save_array, 6, sizeof(int), compare);


    open_rank = fopen("rank.txt", "w");


    for(j = 0;j<5;j++)
    {
         sprintf(ranks, "%d", save_array[j]);
        //itoa(save_array[j], ranks, 10);
        fprintf(open_rank,"%s\n", ranks);
    }

    fclose(open_rank);

    gameoverfunction(number);
    pauseAudio(&travel);

    return 0;
}

/*p2*/
int player_two(void)
{
    int IsEnding = 0;
    int inst =0, k ;
    Audio travel;
    Audio chicken;
    //char str[40] = {'\0'};
    int number = 0;

    Menu scoreMenu;
    setMainmenu(&scoreMenu);

    openAudioFile("Traveler.wav", &travel);
    openAudioFile("chicken1.wav", &chicken);

    Tube_top* mintop = NULL;
    Tube_buttom* minbut = NULL;

    /* 用來計時 time.h */
    clock_t startc;
    int game_tick = 0;

    //Background Initial
    Image *background;
    background = read_image("background_wide.pixel", "background_wide.color");


    //Initial player 1 red bird
    Bird red_bird;
    red_bird.state = 0;
    red_bird.initial = read_image("red_1.pixel", "red_1.color");
    red_bird.up[0] = read_image("red_1.pixel", "red_1.color");
    red_bird.up[1] = read_image("red_2.pixel", "red_2.color");
    red_bird.up[2] = read_image("red_3.pixel", "red_3.color");
    red_bird.x = 20;
    red_bird.y = 55;
    red_bird.moveup_mode = 0;
    red_bird.fallingTime = 0;
    red_bird.flyingTime = 0;



    //Initial player 2 blue bird
    Bird blue_bird;
    blue_bird.state = 0;
    blue_bird.initial = read_image("blue_1.pixel", "blue_1.color");
    blue_bird.up[0] = read_image("blue_1.pixel", "blue_1.color");
    blue_bird.up[1] = read_image("blue_2.pixel", "blue_2.color");
    blue_bird.up[2] = read_image("blue_3.pixel", "blue_3.color");
    blue_bird.x = 20;
    blue_bird.y = 40;
    blue_bird.moveup_mode = 0;
    blue_bird.fallingTime = 0;
    blue_bird.flyingTime = 0;


    //Initial tube*3
    int rand_y = (rand()%20);

    Tube_top top1;
    top1.tubeImg = read_image("top.pixel", "top.color");
    top1.x = 130;
    top1.y = -120 + rand_y;

    Tube_buttom but1;
    but1.tubeImg = read_image("buttom.pixel", "buttom.color");
    but1.x = 130;
    but1.y = 80 + rand_y;

    rand_y = (rand()%20);

    Tube_top top2;
    top2.tubeImg = read_image("top.pixel", "top.color");
    top2.x = 220;
    top2.y = -120 + rand_y;

    Tube_buttom but2;
    but2.tubeImg = read_image("buttom.pixel", "buttom.color");
    but2.x = 220;
    but2.y = 80 + rand_y;

    rand_y = (rand()%20);

    Tube_top top3;
    top3.tubeImg = read_image("top.pixel", "top.color");
    top3.x = 310;
    top3.y = -120 + rand_y;

    Tube_buttom but3;
    but3.tubeImg = read_image("buttom.pixel", "buttom.color");
    but3.x = 310;
    but3.y = 80 + rand_y;

    clearScreen();
    startc = clock();



    while (!IsEnding) {

            /* game loop */
        playAudio(&travel);
        if ((double)(clock()-startc) > 0.01*CLOCKS_PER_SEC) {
            game_tick++;  /* game_tick 0.01 秒 */
            startc = clock();
        }

        if (KEY_DOWN(VK_SPACE)) {
            red_bird.moveup_mode = 1;
        }

        if (KEY_DOWN(VK_UP)) {
            blue_bird.moveup_mode = 1;
        }
        if(KEY_DOWN((VK_H))) Is_difficult = 1;


          if (KEY_DOWN(VK_Q)) {
            IsEnding = 1;
            break;
        }

        if (KEY_DOWN(VK_ESCAPE)) {
            saveScreen();
            clearScreen();
            inst = stopfunction();
            if(inst == 0)
            {
                restoreScreen();
                drawCmdWindow();
            }
            if(inst ==2)
            {
                pauseAudio(&travel);
                return 0;
            }
            else if(inst == 3)
            {
                pauseAudio(&travel);
                return 1;
            }
        }

        if (game_tick > REFRESH_DURATION) {
            game_tick = 0;
            clearScreen();

            show_image(background, 0, 0);
            number+=show_tube(&top1,&but1, Is_difficult);
            number+=show_tube(&top2,&but2, Is_difficult);
            number+=show_tube(&top3,&but3, Is_difficult);
            show_bird(&red_bird, &scoreMenu,number);
            show_bird(&blue_bird, &scoreMenu,number);
            //show_bird1(&red_bird);
            //show_bird1(&blue_bird)
            if(top1.x<top2.x && top1.x<top3.x){
                mintop = &top1;
                minbut = &but1;
            }else if(top2.x<top3.x && top2.x<top1.x){
                mintop = &top2;
                minbut = &but2;
            }else{
                mintop = &top3;
                minbut = &but3;
            }
            if(check_collision(&red_bird,mintop,minbut)==1 &&check_collision(&blue_bird,mintop,minbut)==1) IsEnding = 1;
            if(!IsEnding) drawCmdWindow();  /* update window immediately */
        }
    } /* while (IsEnding) */

    playAudio(&chicken);
    clearScreen();

    /*WRITE RANK*/
     FILE *open_rank;
    open_rank = fopen("rank.txt", "r");
    int j = 0, target = 0;
    char ranks[50]={'\0'};

    int save_array[6]={-1};


    for(j = 0;j<5;j++)
    {
        //fgets(ranks, 50, open_rank);
        fscanf(open_rank,"%s",ranks);
        save_array[j] = my_atoi(ranks);
    }
    save_array[j] = number;

    fclose(open_rank);
    qsort(save_array, 6, sizeof(int), compare);


    open_rank = fopen("rank.txt", "w");


    for(j = 0;j<5;j++)
    {
         sprintf(ranks, "%d", save_array[j]);
        //itoa(save_array[j], ranks, 10);
        fprintf(open_rank,"%s\n", ranks);
    }

    fclose(open_rank);


    gameoverfunction(number);
    pauseAudio(&travel);

    return 0;

}

int stopfunction()
{

    int IsEnding = 0;
    int k;

    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN, VK_SPACE};
    int cur_tick, last_tick;

    Menu mainmenu;
    Image* background, * logo;

    /*bird animations*/
    mainmenu.initial = read_image("red_1.pixel", "red_1.color");
    mainmenu.pic[0] = read_image("red_1.pixel", "red_1.color");
    mainmenu.pic[1] = read_image("red_2.pixel", "red_2.color");
    mainmenu.pic[2] = read_image("red_3.pixel", "red_3.color");

    background = read_image("background_wide.pixel", "background_wide.color");
    logo = read_image("logo.pixel", "logo.color");
    Audio audio;
    openAudioFile("metalplate.wav", &audio);
    cur_tick = last_tick = timer();

    setMainmenu1(&mainmenu);


    show_image(background, 0, 0);
    show_image(logo, 57, 5);
    show_image(mainmenu.pic[0], 20, 10);
    saveScreen();
    int fly = 0; // for for bird
    int bird_yplace = 50;
    while (!IsEnding) {

        cur_tick = timer(); /* 每個 tick 0.01 秒 */
        cur_tick = timer(); /* 每個 tick 0.01 秒 */

        if (cur_tick-last_tick > REFRESH_RATE) {
            last_tick = cur_tick;
            clearScreen();
            restoreScreen();

            show_image(mainmenu.pic[0], 20, 10);
            showMenu(&mainmenu);

            show_image(mainmenu.pic[0], 20, 10);
            drawCmdWindow();
        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */

        for (k=0; k<3; k++) {
            /* 按鍵從原本被按下的狀態 變成放開的狀態  這是為了處理按著不放的情況 */
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }

        /* 鍵盤控制 處理按著不放的狀況 */
        for (k=0; k<NUM_KEYS; k++) {

            for(fly=1; fly<=4; fly++) /*bird animations*/
            {
                show_image(mainmenu.pic[fly%3], 40, bird_yplace); /*(x,y)*/
                drawCmdWindow();
            }

            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;

                switch (key_val[k]) {
                case VK_UP:
                    scrollMenu(&mainmenu, -1);

                    /*restore and cursor == final bird go downward*/
                    restoreScreen();
                    if (mainmenu.cursor == 3)
                    {
                        playAudio(&audio);
                        bird_yplace = 65; // downward
                    }
                    else{
                            playAudio(&audio);
                            bird_yplace = bird_yplace-5;} // upward

                    /*以下防延遲*/
                    show_image(mainmenu.pic[0], 20, 10);
                    drawCmdWindow();
                    break;

                case VK_DOWN:
                    scrollMenu(&mainmenu, +1);
                    restoreScreen();
                    if (mainmenu.cursor == 0) // upward
                    {
                        playAudio(&audio);
                        bird_yplace = 50;
                    }

                    else{ playAudio(&audio);
                            bird_yplace = bird_yplace+5;} //downward
                    show_image(mainmenu.pic[0], 20, 10);
                    showMenu(&mainmenu);
                    drawCmdWindow();
                    break;
                case VK_RETURN:

                        if (IsOnItem(&mainmenu, 0)) {
                            return 0;
                        }
                        if (IsOnItem(&mainmenu, 1)) {
                            return 2;
                        }

                        if (IsOnItem(&mainmenu, 2)) {
                                return 3;
                        }
                    break;
                }
            }
            }
    }
    return 0;
}


int showrank()
{

    int IsEnding = 0;
    int k;
    char str[40] = "Rank Score";
    int cur_tick, last_tick;

    Menu mainmenu;

    /*bird animations*/
    mainmenu.initial = read_image("red_1.pixel", "red_1.color");
    mainmenu.pic[0] = read_image("red_1.pixel", "red_1.color");
    mainmenu.pic[1] = read_image("red_2.pixel", "red_2.color");
    mainmenu.pic[2] = read_image("red_3.pixel", "red_3.color");

    cur_tick = last_tick = timer();

    setMainmenu2(&mainmenu);

    show_image(mainmenu.pic[0], 20, 10);

    saveScreen();

    int fly = 0; // for for bird
    int bird_yplace = 50;
    while (!IsEnding) {

        cur_tick = timer(); /* 每個 tick 0.01 秒 */
        //sprintf(str, "%10d", cur_tick/1000);
        /* 每經過 REFRESH_RATE 個 ticks 才會更新一次畫面 */
        cur_tick = timer(); /* 每個 tick 0.01 秒 */
        //sprintf(str);

        if (cur_tick-last_tick > REFRESH_RATE) {
            last_tick = cur_tick;
            clearScreen();
            restoreScreen();

            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y, str, 14);
            show_image(mainmenu.pic[0], 20, 10);
            showMenu(&mainmenu);

            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y, str, 14);
            show_image(mainmenu.pic[0], 20, 10);
            drawCmdWindow();
        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */

        for (k=0; k<NUM_KEYS; k++) {

            for(fly=1; fly<=4; fly++) /*bird animations*/
            {
                show_image(mainmenu.pic[fly%3], 40, bird_yplace); /*(x,y)*/
                drawCmdWindow();
            }

            if(KEY_DOWN(VK_SPACE)) {
                    IsEnding = 1;
                    break;
                }
            }
        }
    return 0;
}

void setMainmenu1(Menu *m)
{
    int i;

    m->large_font = read_font("font.txt");
    m->x = 7;
    m->y = 8;
    m->fgcolor = 23;
    m->bgcolor = 15;
    m->num_options = 4;
    m->cursor = 0;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }
    m->state[m->cursor] = m->state[m->cursor] | 1;  /* 目前選擇的項目 */

    strcpy(m->text[0], "COTINUE");
    strcpy(m->text[1], "BACKTO MAIN");
    strcpy(m->text[2], "EXIT");
}

/*for rank*/
void setMainmenu2(Menu *m)
{
    int i ,k ;

    FILE *find;
    find = fopen("rank.txt", "r");
    char s[50]={'\0'};
    char str1[50]="First ";
    char str2[50]="Second ";
    char str3[50]="Third ";
    char str4[50]="Fourth ";
    char str5[50]="Fifth ";
    char c;

    m->large_font = read_font("font.txt");
    m->x = 7;
    m->y = 8;
    m->fgcolor = 14;
    m->bgcolor = 14;
    m->num_options = 6;

    strcpy(m->text[0], "SPACE BACK TO MAIN");
    for(k=1;k<=5;k++)
    {
        fgets(s, 50, find);
        if(k==1)
        {
            strcat(str1,s);
            strcpy(m->text[k], str1);
        }
        if(k==2)
        {
            strcat(str2,s);
            strcpy(m->text[k], str2);
        }
        if(k==3)
        {
            strcat(str3,s);
            strcpy(m->text[k], str3);
        }
        if(k==4)
        {
            strcat(str4,s);
            strcpy(m->text[k], str4);
        }
        if(k==5)
        {
            strcat(str5,s);
            strcpy(m->text[k], str5);
        }
        //strcpy(m->text[k], str);
    }

     fclose(find);
}

void gameoverfunction(int number)
{
    Image* award, *gameover, *background;
    Menu bar;

    int IsEnding = 0;
    int i, j, k;

    char score[50]={'\0'};

    background = read_image("background_wide.pixel", "background_wide.color");
    gameover = read_image("gameover.pixel", "gameover.color");
    if(number>10)
    award = read_image("gold.pixel", "gold.color");
    else
    award = read_image("silver.pixel", "silver.color");


    show_image(background, 0, 0);

    show_image(award, 50, 5);
    show_image(gameover, 55, 100);



    sprintf(score, "%d", number);
    setgameover_bar(&bar);
    putStringLarge(bar.large_font, 105, 13, score, 14);


    drawCmdWindow();
    saveScreen();

    while(!IsEnding)
    {
            if(KEY_DOWN(VK_ESCAPE)) {
                    clearScreen();
                    break;
                }

    }

    return ;
}


void setgameover_bar(Menu *m)
{
    int i;

    m->large_font = read_font("font.txt");
    m->x = 7;
    m->y = 8;
    m->fgcolor = 14;
    m->bgcolor = 14;
    m->num_options = 1;
     for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }
}


int show_tube(Tube_top *tbt,Tube_buttom *tbb, int Is_difficult)
{
    int count = 0;
    show_image(tbt->tubeImg, tbt->x, tbt->y);
    show_image(tbb->tubeImg, tbb->x, tbb->y);

    tbt->x -= 2;
    tbb->x -= 2;


    if(tbt->x==0 && tbb->x==0)
    {
        count+=1;
    }

    if(tbt->x==0 && tbb->x==0){

        tbt->x+=280;
        tbb->x+=280;
        if(Is_difficult){
            ran_tube(tbt,tbb);
        }


    }
    return count;
}


void ran_tube(Tube_top *tbt, Tube_buttom *tbb){
    int ran_y = (rand()%20)*1.5;
    tbt->y = -120 + ran_y;
    tbb->y = 100 + ran_y;
}


int check_collision(Bird *bd, Tube_top *tbt, Tube_buttom *tbb)
{
    if(bd->y+17 > BACK_HEIGHT){
        return 1;//gameover
    }else if(bd->x+17 >= tbt->x  &&  bd->x <= tbt->x+26){
        if(bd->y <= tbt->y+160 || bd->y+12 >= tbb->y){
             return 1;
        }else  return 0;
    }else{
        return 0;
    }
}



