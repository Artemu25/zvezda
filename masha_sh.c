#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <menu.h>
#include <stdlib.h> 
#include <string.h> 
#include <dirent.h>
#include <curses.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD   4

void get_choice(char ***choices, char path[], int *n_choices) {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(path);
    int i, j = 0;
    while((entry = readdir(dir)) != NULL) {
        i++;
    };
    *choices = (char **)malloc((i + 1)*sizeof(char *));
    for(j = 0; j < i; j++) *(*choices + j) = (char *)malloc(256*sizeof(char));
    dir = opendir(path);
    int f = 0;
    while ((entry = readdir(dir)) != NULL) {
        *(*choices + f) = entry->d_name;
        f++;
    }
    *(*choices + f) = (char *)NULL;
    *n_choices = f + 1;
} 

void unpost(MENU *menu, ITEM **my_items, int *n_choices) {
    unpost_menu(menu);
    free_menu(menu);
    for(i = 0; i <= n_choices-1; ++i) //
        free_item(my_items[i]);
    endwin();
}

void middle_write(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

int main() {
    struct dirent *entry;
    struct stat file;
    DIR *dir;
    ITEM **my_items1, **my_items2;              
    MENU *my_menu1, *my_menu2;
    WINDOW *my_menu_win1, *my_menu_win2;
    int c;
    int new_choices1, new_choices2, i, j, current_window = 0;
    char new_path1[2000] = "/";
    char new_path2[2000] = "/";
    char **my_choices1, **my_choices2;
    initscr();
    start_color();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    init_pair(1, COLOR_RED, COLOR_BLACK);

    get_choice(&my_choices1, new_path1, &new_choices1);
    get_choice(&my_choices2, new_path2, &new_choices2);
    my_items1 = (ITEM **)calloc(new_choices1, sizeof(ITEM *));
    my_items2 = (ITEM **)calloc(new_choices2, sizeof(ITEM *));
    for(i = 0; i < new_choices1; ++i) my_items1[i] = new_item(my_choices1[i], "");
    for(i = 0; i < new_choices2; ++i) my_items2[i] = new_item(my_choices2[i], "");
    my_menu1 = new_menu((ITEM **)my_items1);
    my_menu2 = new_menu((ITEM **)my_items2);
    my_menu_win1 = newwin(22, 39, 1, 1);
    my_menu_win2 = newwin(22, 39, 1, COLS/2 + 1);
    keypad(my_menu_win1, TRUE);
    keypad(my_menu_win2, TRUE);

    set_menu_win(my_menu1, my_menu_win1);
    set_menu_sub(my_menu1, derwin(my_menu_win1, 17, 38, 3, 1));
    set_menu_format(my_menu1, 17, 1);
    set_menu_win(my_menu2, my_menu_win2);
    set_menu_sub(my_menu2, derwin(my_menu_win2, 17, 38, 3, 1));
    set_menu_format(my_menu2, 17, 1);
    box(my_menu_win1, 0, 0);
    mvwaddch(my_menu_win1, 2, 39, ACS_RTEE);
    box(my_menu_win2, 0, 0);
    mvwaddch(my_menu_win2, 2, 39, ACS_RTEE);
    mvprintw(LINES - 1, 0, "EXIT = F10");
    refresh();
    post_menu(my_menu1);
    wrefresh(my_menu_win1);
    post_menu(my_menu2);
    wrefresh(my_menu_win2);

    while ((c = getch()) != KEY_F(10)) {
        switch (c) {
            case KEY_DOWN:
                if (current_window == 0) menu_driver(my_menu1, REQ_DOWN_ITEM);
                else menu_driver(my_menu2, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                if (current_window == 0) menu_driver(my_menu1, REQ_UP_ITEM);
                else menu_driver(my_menu2, REQ_UP_ITEM);
                break;
            case 9:
                current_window = (current_window + 1) % 2;
                break;
            case 10:
                if (current_window == 0) {
                    char add[2048];
                    strcpy(add, new_path1);
                    strcat(new_path1, "/");
                    strcat(new_path1, item_name(current_item(my_menu1)));
                    if (stat(new_path1, &file) >= 0) {
                        if (S_ISDIR(file.st_mode)) {
                            DIR *d = opendir(new_path1);
                            if (d != NULL) {
                                unpost_menu(my_menu1);
                                get_choice(&my_choices1, new_path1, &new_choices1);
                                my_items1 = (ITEM **)calloc(new_choices1, sizeof(ITEM *));
                                for(i = 0; i < new_choices1; ++i) my_items1[i] = new_item(my_choices1[i], "");
                                my_menu1 = new_menu((ITEM **)my_items1);
                                set_menu_win(my_menu1, my_menu_win1);
                                set_menu_sub(my_menu1, derwin(my_menu_win1, 17, 38, 3, 1));
                                set_menu_format(my_menu1, 17, 1);
                                box(my_menu_win1, 0, 0);
                                mvwaddch(my_menu_win1, 2, 39, ACS_RTEE);
                                refresh();
                                post_menu(my_menu1);
                                wrefresh(my_menu_win1);
                            } else {
                                strcpy(new_path1, add);
                            }
                        } else {
                            strcpy(new_path1, add);
                        }
                    } else {
                        strcpy(new_path1, add);
                    }
                } else {
                    char add[2048];
                    strcpy(add, new_path1);
                    strcat(new_path2, "/");
                    strcat(new_path2, item_name(current_item(my_menu2)));
                    if (stat(new_path2, &file) >= 0) {
                        if (S_ISDIR(file.st_mode)) {
                            DIR *d = opendir(new_path2);
                            if (d != NULL) {
                                unpost_menu(my_menu2);
                                get_choice(&my_choices2, new_path2, &new_choices2);
                                my_items2 = (ITEM **)calloc(new_choices2, sizeof(ITEM *));
                                for(i = 0; i < new_choices2; ++i) my_items2[i] = new_item(my_choices2[i], "");
                                my_menu2 = new_menu((ITEM **)my_items2);
                                set_menu_win(my_menu2, my_menu_win2);
                                set_menu_sub(my_menu2, derwin(my_menu_win2, 17, 38, 3, 1));
                                set_menu_format(my_menu2, 17, 1);
                                box(my_menu_win2, 0, 0);
                                mvwaddch(my_menu_win2, 2, 39, ACS_RTEE);
                                refresh();
                                post_menu(my_menu2);
                                wrefresh(my_menu_win2);
                            } else {
                                strcpy(new_path2, add);
                            }
                        } else {
                            strcpy(new_path2, add);
                        }
                    } else {
                        strcpy(new_path2, add);
                    }
                } 
                break;
        }
        wrefresh(my_menu_win1);
        wrefresh(my_menu_win2); 
    }   
    unpost(my_menu1, my_items1, new_choices1);
    unpost(my_menu2, my_items2, new_choices2);
}