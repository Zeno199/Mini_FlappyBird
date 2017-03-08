#include "font.h"
#ifndef FRAMEWORK_
#define FRAMEWORK_
#include "image.h"
struct t_menu {
    int x;
    int y;
    int fgcolor;
    int bgcolor;
    int cursor;
    int num_options;
    int state[15];
    char text[15][30]; /* 程﹃  –﹃程 79 じ */
    char alt_text[15][30]; /* 程﹃  –﹃程 79 じ */
    Image *pic[3];  // for bird place
    Image *initial;


    Font *large_font;
};
typedef struct t_menu Menu;

extern void showMenu(Menu *m);


extern int IsOnItem(Menu *m, int index);
extern int IsItemSelected(Menu *m, int index);
extern void showMenu(Menu *m);
extern void scrollMenu(Menu *m, int diff);
extern void radioMenu(Menu *m);
extern void toggleMenu(Menu *m);


#endif

