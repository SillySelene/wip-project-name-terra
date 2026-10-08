#include <ncurses.h>
#include <string.h>
#include <unistd.h>

#define SIDEBAR_WIDTH 30
#define SIDEBAR_HEIGHT LINES - 1

int startx = 0;
int starty = 0;

char *choices[] = {
    "Inventory",  "Stats/Attributes", "Beastiary", "Diety",
    "Your Story", "Quest Book",       "Exit",

};

char *choices_right[] = {"Fight", "Defend", "Magic", "Quick-Potion 1",
                         "Quick-Potion 2"};

int n_choices = sizeof(choices) / sizeof(char *);
int n_choices_right = sizeof(choices_right) / sizeof(char *);
void print_menu(WINDOW *menu_win, int highlight);
void print_right_menu(WINDOW *menu_win, int highlight);
WINDOW *create_newwin(int height, int width, int starty, int startx) {
  WINDOW *local_win;

  local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0); /* 0, 0 gives default characters
                         * for the vertical and horizontal
                         * lines			*/
  wrefresh(local_win);  /* Show that box 		*/

  return local_win;
}

void sidebar() {
  WINDOW *menu_win;
  int highlight = 1;
  int choice = 0;
  int c;

  startx = 0;
  starty = 1;

  menu_win = newwin(SIDEBAR_HEIGHT, SIDEBAR_WIDTH, starty, startx);
  keypad(menu_win, TRUE);
  mvprintw(0, 0,
           "Use arrow keys to go up and down, Press enter to select a choice");
  refresh();
  print_menu(menu_win, highlight);
  while (1) {
    c = wgetch(menu_win);
    switch (c) {
    case KEY_UP:
      if (highlight == 1)
        highlight = n_choices;
      else
        --highlight;
      break;
    case KEY_DOWN:
      if (highlight == n_choices)
        highlight = 1;
      else
        ++highlight;
      break;
    case 10:
      choice = highlight;
      break;
    default:
      mvprintw(24, 0,
               "Charcter pressed is = %3d Hopefully it can be printed as '%c'",
               c, c);
      refresh();
      break;
    }
    print_menu(menu_win, highlight);
    if (choice != 0) /* User did a choice come out of the infinite loop */
      break;
  }
}

int centerText(int windowLength, char inputString[]) {
  int stringLength = strlen(inputString);
  return (windowLength - stringLength) / 2;
}

int main() {

  initscr();
  clear();
  noecho();
  cbreak(); /* Line buffering disabled. pass on everything */
  keypad(stdscr, TRUE);

  WINDOW *sidebar = newwin(SIDEBAR_HEIGHT, SIDEBAR_WIDTH, starty, startx);
  refresh();
  print_menu(sidebar, -1);

  WINDOW *gamespace = create_newwin(30, 80, (LINES - 30) / 2, (COLS - 80) / 2);
  WINDOW *title = create_newwin(5, 80, 0, (COLS - 80) / 2);
  char titletext[] = "SILLYSELENE PRESENTS: ETHICAL";
  mvwprintw(title, 2, centerText(80, titletext), titletext);
  wrefresh(title);

  WINDOW *footer = create_newwin(5, 80, LINES - 6, (COLS - 80) / 2);

  char yourHP[] = "CURRENT HP: %% - CURRENT MP: %% - CURRENT STAMINA: %%";
  char enemyHP[] = "ENEMY HP: %% -  ENEMY MP: %% - ENEMY STANIMA: %%";

  mvwprintw(footer, 1, centerText(80, yourHP), yourHP);
  mvwprintw(footer, 2, centerText(80, enemyHP), enemyHP);
  wrefresh(footer);

  WINDOW *right_sidebar =
      newwin(SIDEBAR_HEIGHT, SIDEBAR_WIDTH, 0, COLS - SIDEBAR_WIDTH);
  refresh();
  print_right_menu(right_sidebar, -1);

  int ch;

  sleep(10);
  endwin();
  return 0;
}

void print_menu(WINDOW *menu_win, int highlight) {
  int x, y, i;

  x = 2;
  y = 2;
  box(menu_win, 0, 0);
  for (i = 0; i < n_choices; ++i) {
    if (highlight == i + 1) /* High light the present choice */
    {
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x, "%s", choices[i]);
      wattroff(menu_win, A_REVERSE);
    } else
      mvwprintw(menu_win, y, x, "%s", choices[i]);
    ++y;
  }
  wrefresh(menu_win);
}

void print_right_menu(WINDOW *menu_win, int highlight) {
  int x, y, i;

  x = 2;
  y = 2;
  box(menu_win, 0, 0);
  for (i = 0; i < n_choices_right; ++i) {
    if (highlight == i + 1) /* High light the present choice */
    {
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x, "%s", choices_right[i]);
      wattroff(menu_win, A_REVERSE);
    } else
      mvwprintw(menu_win, y, x, "%s", choices_right[i]);
    ++y;
  }
  wrefresh(menu_win);
}
