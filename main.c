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

/*
drawChar()  最後兩個參數分別是字元背景和前景的顏色
可以參考下面的顏色值對照表來設定你想要的顏色
   0: 黑     1: 暗藍   2: 暗綠   3: 暗青
   4: 暗紅   5: 暗紫   6: 暗黃   7: 淺灰
   8: 暗灰   9: 亮藍  10: 亮綠  11: 亮青
  12: 亮紅  13: 亮紫  14: 亮黃  15: 白
*/

#define WIDTH     30
#define HEIGHT    20
#define OFFSET_X  5
#define OFFSET_Y  5

#define NUM_KEYS 7
#define REFRESH_RATE 10


int timer(void);

int my_game_one(void);
int my_game_two(void);
void gameoverfunction(void);
void about(void);

void setMainmenu(Menu *m);
void menu_bird(Menu *m, Image *bg);
int stopfunction();

int main(void)
{
    int IsEnding = 0;
    int k;
    char str[40] = "105065532 Zeno Peng\n";
    char str1[40] = "105065526 John Fu\n";


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
    Audio audio1;
    openAudioFile("metalplate.wav", &audio);
    openAudioFile("Forest.wav", &audio1);



    /* 啟動鍵盤控制 整個程式中只要做一次就行了*/
    initializeKeyInput();
    cur_tick = last_tick = timer();

    setMainmenu(&mainmenu);


    show_image(background, 0, 0);
    show_image(logo, 57, 5);
    show_image(mainmenu.pic[0], 20, 10);
    saveScreen();

    int fly = 0; // for for bird
    int bird_yplace = 50;
    /* 無窮迴圈  裡面包含了這個程式的主要工作
    而且每次迴圈會不斷把最新的畫面顯示到螢幕上
    像是播動畫一樣 每次迴圈更新一次畫面 */

    while (!IsEnding) {

         playAudio(&audio1);

        cur_tick = timer(); /* 每個 tick 0.01 秒 */
        //sprintf(str, "%10d", cur_tick/1000);
        /* 每經過 REFRESH_RATE 個 ticks 才會更新一次畫面 */
        cur_tick = timer(); /* 每個 tick 0.01 秒 */
        //sprintf(str);

        if (cur_tick-last_tick > REFRESH_RATE) {
            last_tick = cur_tick;
            clearScreen();
            restoreScreen();
            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+100, str, 14);
            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+110, str1, 14);

            /* 把選單畫出來 */
             //menu_bird(&mainmenu, background );
            show_image(mainmenu.pic[0], 20, 10);
            showMenu(&mainmenu);

            show_image(mainmenu.pic[0], 20, 10);
            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+100, str, 14);
            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+110, str1, 14);
            /* 為了要讓一連串 drawChar() 的動作產生效果
               必須要呼叫一次 drawCmdWindow() 把之前畫的全部內容一次顯示到螢幕上 */
            drawCmdWindow();
        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */
        for (k=0; k<NUM_KEYS; k++) {
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

            /* 按鍵從原本被按下的狀態 變成放開的狀態  這是為了處理按著不放的情況 */


            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;

                switch (key_val[k]) {
                case VK_UP:
                    scrollMenu(&mainmenu, -1);

                    /*restore and cursor == final bird go downward*/
                    restoreScreen();
                    if (mainmenu.cursor == 4)
                    {
                        playAudio(&audio);
                        bird_yplace = 75; // downward
                    }
                    else{
                            playAudio(&audio);
                            bird_yplace = bird_yplace-5;} // upward

                    /*以下防延遲*/
                   /* show_image(mainmenu.pic[0], 20, 10);
                    showMenu(&mainmenu);
                     putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+110, str, 14);
                     putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+110, str1, 14);
                    drawCmdWindow();*/

                    show_image(mainmenu.pic[0], 20, 10);
                    showMenu(&mainmenu);
                    putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+100, str, 14);
                    putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+110, str1, 14);
                    drawCmdWindow();
                    break;

                case VK_DOWN:
                    //clearScreen();


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
                    putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+100, str, 14);
                    putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+110, str1, 14);
                    drawCmdWindow();
                    break;
                case VK_RETURN:

                        if (IsOnItem(&mainmenu, 0)) {
                            clearScreen();
                            pauseAudio(&audio1);
                            playAudio(&audio);
                            if(player_one()==1) IsEnding =1;
                            playAudio(&audio1);
                            restartAudio(&audio1);
                            show_image(background, 0, 0);
                            show_image(logo, 57, 5);
                            saveScreen();
                            drawCmdWindow();

                        }
                        if (IsOnItem(&mainmenu, 1)) {
                            pauseAudio(&audio1);
                            playAudio(&audio);
                            if(player_two()==1) IsEnding = 1;
                            restartAudio(&audio1);

                            show_image(background, 0, 0);
                            show_image(logo, 57, 5);
                            saveScreen();
                            drawCmdWindow();
                        }

                        if (IsOnItem(&mainmenu, 3)) {
                             clearScreen();
                            about();
                            show_image(background, 0, 0);
                            show_image(logo, 57, 5);
                            saveScreen();
                            drawCmdWindow();
                        }
                        if (IsOnItem(&mainmenu, 2)) {
                            saveScreen();
                            clearScreen();
                             showrank();
                             show_image(background, 0, 0);
                            show_image(logo, 57, 5);
                            saveScreen();
                            drawCmdWindow();
                        }



                     else if (IsOnItem(&mainmenu, 4)) {

                         showMenu(&mainmenu);
                         IsEnding = 1;

                    }
                    break;
                }
            }
        }

    } /* while (IsEnding) */
    return 0;
}

int timer(void)
{
    return (clock()/(0.001*CLOCKS_PER_SEC));
}


void setMainmenu(Menu *m)
{
    int i;

    m->large_font = read_font("font.txt");
    m->x = 7;
    m->y = 8;
    m->fgcolor = 17;
    m->bgcolor = 15;
    m->num_options = 5;
    m->cursor = 0;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }
    m->state[m->cursor] = m->state[m->cursor] | 1;  /* 目前選擇的項目 */

    strcpy(m->text[0], "ONE PLAYER");
    strcpy(m->text[1], "TWO PLAYERS");
     strcpy(m->text[2], "RANK");
      strcpy(m->text[3], "ABOUT");
    strcpy(m->text[4], "EXIT");


}
/*--------------------------------------------------------------*/


void about()
{

    int IsEnding = 0;
    int k;
    int cur_tick, last_tick;
    Image* background, * logo;

    Menu about_mainmenu;

    /*bird animations*/
     background = read_image("background_wide.pixel", "background_wide.color");
    logo = read_image("logo.pixel", "logo.color");
   about_mainmenu.initial = read_image("red_1.pixel", "red_1.color");
   about_mainmenu.pic[0] = read_image("red_1.pixel", "red_1.color");
    about_mainmenu.pic[1] = read_image("red_2.pixel", "red_2.color");
    about_mainmenu.pic[2] = read_image("red_3.pixel", "red_3.color");

    cur_tick = last_tick = timer();

    setAbout_Mainmenu(&about_mainmenu);
    show_image(background, 0, 0);
    show_image(logo, 57, 5);
    show_image(about_mainmenu.pic[0], 20, 10);
    saveScreen();

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

            show_image(about_mainmenu.pic[0], 20, 10);
            showMenu(&about_mainmenu);

            show_image(about_mainmenu.pic[0], 20, 10);
            drawCmdWindow();
        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */


            if(KEY_DOWN(VK_Q)) {
                    IsEnding = 1;
                    break;
                }
            }

    return 0;


}


void setAbout_Mainmenu(Menu *m)
{
    int i;

    m->large_font = read_font("font.txt");
    m->x = 7;
    m->y = 8;
    m->fgcolor = 14;
    m->bgcolor = 14;
    m->num_options = 5;
     for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }

    strcpy(m->text[0], "SPACE JUMP H:HARD");
    strcpy(m->text[1], "UP FOR TWO MODE");
    strcpy(m->text[2], "ESC TO STOP");
    strcpy(m->text[3], "AND QUIT GAMEOVER");
    strcpy(m->text[4], "Q TO ABOUT");

}


