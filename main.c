#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
/* ���İO�o�n�[�J�o�� */
#include "audio.h"
/* �t�~�٦����U���]�w */
/* For sound effect: In [Project Options]->[Parameters]->[Linker] add the parameter -lwinmm */

#include "console_draw3.h"
#include "kb_input2.h"
#include "font.h"
#include "image.h"
#include "framework.h"

/*
drawChar()  �̫��ӰѼƤ��O�O�r���I���M�e�����C��
�i�H�ѦҤU�����C��ȹ�Ӫ�ӳ]�w�A�Q�n���C��
   0: ��     1: �t��   2: �t��   3: �t�C
   4: �t��   5: �t��   6: �t��   7: �L��
   8: �t��   9: �G��  10: �G��  11: �G�C
  12: �G��  13: �G��  14: �G��  15: ��
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



    /* �Ұ���L���� ��ӵ{�����u�n���@���N��F*/
    initializeKeyInput();
    cur_tick = last_tick = timer();

    setMainmenu(&mainmenu);


    show_image(background, 0, 0);
    show_image(logo, 57, 5);
    show_image(mainmenu.pic[0], 20, 10);
    saveScreen();

    int fly = 0; // for for bird
    int bird_yplace = 50;
    /* �L�a�j��  �̭��]�t�F�o�ӵ{�����D�n�u�@
    �ӥB�C���j��|���_��̷s���e����ܨ�ù��W
    ���O���ʵe�@�� �C���j���s�@���e�� */

    while (!IsEnding) {

         playAudio(&audio1);

        cur_tick = timer(); /* �C�� tick 0.01 �� */
        //sprintf(str, "%10d", cur_tick/1000);
        /* �C�g�L REFRESH_RATE �� ticks �~�|��s�@���e�� */
        cur_tick = timer(); /* �C�� tick 0.01 �� */
        //sprintf(str);

        if (cur_tick-last_tick > REFRESH_RATE) {
            last_tick = cur_tick;
            clearScreen();
            restoreScreen();
            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+100, str, 14);
            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+110, str1, 14);

            /* ����e�X�� */
             //menu_bird(&mainmenu, background );
            show_image(mainmenu.pic[0], 20, 10);
            showMenu(&mainmenu);

            show_image(mainmenu.pic[0], 20, 10);
            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+100, str, 14);
            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y+110, str1, 14);
            /* ���F�n���@�s�� drawChar() ���ʧ@���ͮĪG
               �����n�I�s�@�� drawCmdWindow() �⤧�e�e���������e�@����ܨ�ù��W */
            drawCmdWindow();
        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */
        for (k=0; k<NUM_KEYS; k++) {
            /* ����q�쥻�Q���U�����A �ܦ���}�����A  �o�O���F�B�z���ۤ��񪺱��p */
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }

        /* ��L���� �B�z���ۤ��񪺪��p */
        for (k=0; k<NUM_KEYS; k++) {

            for(fly=1; fly<=4; fly++) /*bird animations*/
            {
                show_image(mainmenu.pic[fly%3], 40, bird_yplace); /*(x,y)*/
                drawCmdWindow();
            }

            /* ����q�쥻�Q���U�����A �ܦ���}�����A  �o�O���F�B�z���ۤ��񪺱��p */


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

                    /*�H�U������*/
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
    m->state[m->cursor] = m->state[m->cursor] | 1;  /* �ثe��ܪ����� */

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

        cur_tick = timer(); /* �C�� tick 0.01 �� */
        //sprintf(str, "%10d", cur_tick/1000);
        /* �C�g�L REFRESH_RATE �� ticks �~�|��s�@���e�� */
        cur_tick = timer(); /* �C�� tick 0.01 �� */
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


