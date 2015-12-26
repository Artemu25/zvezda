#include <menu.h>
#include <stdlib.h> 
#include <string.h> 
#include <dirent.h>
#include <curses.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD   4
#define ENTER 10

unsigned char isFile = 0x8;

void new_choice(char ***choices, char path[], int *n_choices) {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(path);
    int j, i = 0;
    while((entry = readdir(dir)) != NULL) {
        i++;
    };
    *choices = (char **)malloc((i + 1)*sizeof(char *));
    for(j = 0; j < i; j++) {
        *(*choices + j) = (char *)malloc(256*sizeof(char));
    }
    dir = opendir(path);
    int f = 0;
    while ((entry = readdir(dir)) != NULL) {
        *(*choices + f) = entry->d_name;
        f++;
    }
    *(*choices + f) = (char *)NULL;
    *n_choices = f + 1;
} 

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat lala;
    ITEM **my_items1, **my_items2;
    int c;              
    MENU *my_menu1, *my_menu2;
    WINDOW *my_menu_win1, *my_menu_win2;
    int n_choices1, n_choices2, i, j, currwin = 0;
    char path1[2048] = "/";
    char path2[2048] = "/";
    char **choices1;
    char **choices2;
    
    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);

    new_choice(&choices1, path1, &n_choices1);
    new_choice(&choices2, path2, &n_choices2);

    /* Create items */
    /*n_choices1 = ARRAY_SIZE(choices1);*/
    my_items1 = (ITEM **)calloc(n_choices1, sizeof(ITEM *));
    for(i = 0; i < n_choices1; ++i)
        my_items1[i] = new_item(choices1[i], "");
    /*n_choices2 = ARRAY_SIZE(choices2);*/
    my_items2 = (ITEM **)calloc(n_choices2, sizeof(ITEM *));
    for(i = 0; i < n_choices2; ++i)
        my_items2[i] = new_item(choices2[i], "");

    /* Crate menu */
    my_menu1 = new_menu((ITEM **)my_items1);
    my_menu2 = new_menu((ITEM **)my_items2);

    /* Create the window to be associated with the menu */
    my_menu_win1 = newwin(22, 39, 1, 1);
    keypad(my_menu_win1, TRUE);
    my_menu_win2 = newwin(22, 39, 1, COLS/2 + 1);
    keypad(my_menu_win2, TRUE);
     
    /* Set main window and sub window */
    set_menu_win(my_menu1, my_menu_win1);
    set_menu_sub(my_menu1, derwin(my_menu_win1, 17, 38, 3, 1));
    set_menu_format(my_menu1, 17, 1);
    set_menu_win(my_menu2, my_menu_win2);
    set_menu_sub(my_menu2, derwin(my_menu_win2, 17, 38, 3, 1));
    set_menu_format(my_menu2, 17, 1);
    
    /* Print a border around the main window and print a title */
    box(my_menu_win1, 0, 0);
    mvwaddch(my_menu_win1, 2, 39, ACS_RTEE);
    box(my_menu_win2, 0, 0);
    mvwaddch(my_menu_win2, 2, 39, ACS_RTEE);
    mvprintw(LINES - 1, 0, "F10 to exit");
    refresh();

    /* Post the menu */
    post_menu(my_menu1);
    wrefresh(my_menu_win1);
    post_menu(my_menu2);
    wrefresh(my_menu_win2);

    while((c = getch()) != KEY_F(10)) {
        switch(c) {
            case KEY_DOWN:
                if (currwin == 0)
                    menu_driver(my_menu1, REQ_DOWN_ITEM);
                else
                    menu_driver(my_menu2, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                if (currwin == 0)
                    menu_driver(my_menu1, REQ_UP_ITEM);
                else
                    menu_driver(my_menu2, REQ_UP_ITEM);
                break;
            case 9:
                currwin = (currwin + 1) % 2;
                break;
            case ENTER:
                if (currwin == 0) {
                    char helper[2048];
                    strcpy(helper, path1);
                    strcat(path1, "/");
                    strcat(path1, item_name(current_item(my_menu1)));
                    free_menu(my_menu1); 
                    if (stat(path1, &lala) >= 0) {
                    if (S_ISDIR(lala.st_mode)) {
                        unpost_menu(my_menu1);
                        new_choice(&choices1, path1, &n_choices1);
                        my_items1 = (ITEM **)calloc(n_choices1, sizeof(ITEM *));
                        for(i = 0; i < n_choices1; ++i) my_items1[i] = new_item(choices1[i], "");
                        my_menu1 = new_menu((ITEM **)my_items1);
                        set_menu_win(my_menu1, my_menu_win1);
                        set_menu_sub(my_menu1, derwin(my_menu_win1, 17, 38, 3, 1));
                        set_menu_format(my_menu1, 17, 1);
                        box(my_menu_win1, 0, 0);
                        mvwaddch(my_menu_win1, 2, 39, ACS_RTEE);
                        refresh();
                        post_menu(my_menu1);
                        wrefresh(my_menu_win1);
                    }
                    }
                    else {
                        strcpy(path1, helper);
                        unpost_menu(my_menu1);
                        new_choice(&choices1, path1, &n_choices1);
                        my_items1 = (ITEM **)calloc(n_choices1, sizeof(ITEM *));
                        for(i = 0; i < n_choices1; ++i) my_items1[i] = new_item(choices1[i], "");
                        my_menu1 = new_menu((ITEM **)my_items1);
                        set_menu_win(my_menu1, my_menu_win1);
                        set_menu_sub(my_menu1, derwin(my_menu_win1, 17, 38, 3, 1));
                        set_menu_format(my_menu1, 17, 1);
                        box(my_menu_win1, 0, 0);
                        mvwaddch(my_menu_win1, 2, 39, ACS_RTEE);
                        refresh();
                        post_menu(my_menu1);
                        wrefresh(my_menu_win1);
                    }
                } else {
                    char helper[2048];
                    strcpy(helper, path1);
                    strcat(path2, "/");
                    strcat(path2, item_name(current_item(my_menu2)));
                    free_menu(my_menu2);  
                    if (stat(path1, &lala) >= 0) {
                    if (S_ISDIR(lala.st_mode)) {
                    unpost_menu(my_menu2);
                    strcat(path2, "/");
                    strcat(path2, item_name(current_item(my_menu2)));
                    new_choice(&choices2, path2, &n_choices2);
                    my_items2 = (ITEM **)calloc(n_choices2, sizeof(ITEM *));
                    for(i = 0; i < n_choices2; ++i)
                        my_items2[i] = new_item(choices2[i], "");
                    my_menu2 = new_menu((ITEM **)my_items2);
                    set_menu_win(my_menu2, my_menu_win2);
                    set_menu_sub(my_menu2, derwin(my_menu_win2, 17, 38, 3, 1));
                    set_menu_format(my_menu2, 17, 1);
                    box(my_menu_win2, 0, 0);
                    mvwaddch(my_menu_win2, 2, 39, ACS_RTEE);
                    refresh();
                    post_menu(my_menu2);
                    wrefresh(my_menu_win2); 
                    }
                    }
                    else {
                        strcpy(path2, helper);
                        unpost_menu(my_menu2);
                    strcat(path2, "/");
                    strcat(path2, item_name(current_item(my_menu2)));
                    new_choice(&choices2, path2, &n_choices2);
                    my_items2 = (ITEM **)calloc(n_choices2, sizeof(ITEM *));
                    for(i = 0; i < n_choices2; ++i)
                        my_items2[i] = new_item(choices2[i], "");
                    my_menu2 = new_menu((ITEM **)my_items2);
                    set_menu_win(my_menu2, my_menu_win2);
                    set_menu_sub(my_menu2, derwin(my_menu_win2, 17, 38, 3, 1));
                    set_menu_format(my_menu2, 17, 1);
                    box(my_menu_win2, 0, 0);
                    mvwaddch(my_menu_win2, 2, 39, ACS_RTEE);
                    refresh();
                    post_menu(my_menu2);
                    wrefresh(my_menu_win2); 
                    }
                } 
                break;
        }
        wrefresh(my_menu_win1);
        wrefresh(my_menu_win2); 
    }   

    /* Unpost and free all the memory taken up */
    unpost_menu(my_menu1);
    free_menu(my_menu1);
    for(i = 0; i < n_choices1; ++i)
        free_item(my_items1[i]);
    endwin();
}