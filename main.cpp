#include <cstring>
#include <iostream>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

#define SIDEBAR_WIDTH 30
#define SIDEBAR_HEIGHT LINES - 1

#define MAP_WIDTH 79
#define MAP_HEIGHT 29

int startx = 0;
int starty = 0;

string choices[7] = {
    "Inventory",  "Stats/Attributes", "Beastiary", "Diety",
    "Your Story", "Quest Book",       "Exit",

};

string choices_right[5] = {"Fight", "Defend", "Magic", "Quick-Potion 1",
                           "Quick-Potion 2"};

int posX = 40;
int posY = 15;

int n_choices = 7;
int n_choices_right = 5;
void print_menu(WINDOW *menu_win, int highlight);
void print_right_menu(WINDOW *menu_win, int highlight);

WINDOW *gamespace;

class Floor {
public:
  WINDOW *gameWindow;
  int gameWindowLength;
  int gameWindowHeight;
  int floorNum;
  char gameMatrix[MAP_HEIGHT][MAP_WIDTH];
  /*
   * TODO: RENDER MAP METHOD
   * TODO: INIT METHOD
   * TODO: MODIFY MAP METHODS
   * TODO: GENERATE MAP METHOD
   * TODO:
   */

  void renderMap() {
    int y, x;
    for (y = 1; y < 29; y++) {
      for (x = 1; x < 79; x++) {
        mvwaddch(gamespace, y, x, gameMatrix[y][x]);
        /* mvwaddch(gameWindow, y, x, gameMatrix[y][x]); */
        wrefresh(gamespace);
      }
    }
  }
  void generateMap() {
    int y, x;
    for (y = 1; y < MAP_HEIGHT; y++) {
      for (x = 1; x < MAP_WIDTH; x++) {
        if ((y == 1 || y == MAP_HEIGHT - 1) || (x == 1 || x == MAP_WIDTH - 1)) {
          gameMatrix[y][x] = '#';
        } else {
          gameMatrix[y][x] = '.';
        }
      }
    }
  }

  Floor(WINDOW *gameW, int gameWL, int gameWH, int floorN) {

    gameWindow = gameW;
    gameWindowLength = gameWL;
    gameWindowHeight = gameWH;
    floorNum = floorN;
    generateMap();
  }
};

class Entity {
public:
  string name;
  int constitution;
  int dexterity;
  int strength;
  int intelligence;
  int wisdom;
  int perception;
  int level;
  int healthMax;
  int health;

  int maxHealth() { return constitution * 1 + (strength / 2); }
  void takeDamage(int dmgAmount) {
    health -= dmgAmount;
    if (health <= -1) {
      handleDeath();
    }
  }
  void handleDeath() {
    /*
     * TODO: add death
     */
  }

  Entity(string myName, int con, int dex, int str, int intel, int wis, int per,
         int lvl, int startingX, int startingY) {
    name = myName;
    constitution = con;
    dexterity = dex;
    strength = str;
    intelligence = intel;
    wisdom = wis;
    perception = per;
    level = lvl;
    healthMax = maxHealth();
    health = healthMax;
  }
};

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
  curs_set(0);

  WINDOW *sidebar = newwin(SIDEBAR_HEIGHT, SIDEBAR_WIDTH, starty, startx);
  refresh();
  print_menu(sidebar, -1);

  gamespace = create_newwin(30, 80, (LINES - 30) / 2, (COLS - 80) / 2);
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

  Floor floor1(gamespace, 30, 80, 0);
  floor1.generateMap();
  floor1.renderMap();
  wrefresh(gamespace);

  int continueGame = 1;

  keypad(gamespace, TRUE);

  while (continueGame == 1) {
    int ch = wgetch(gamespace);
    floor1.renderMap();
    switch (ch) {
    case KEY_BACKSPACE:
      continueGame = 0;
      break;
    case KEY_LEFT:
      if (floor1.gameMatrix[posY][posX - 1] == '.') {
        posX -= 1;
      }
      break;
    case KEY_RIGHT:
      if (floor1.gameMatrix[posY][posX + 1] == '.') {
        posX += 1;
      }
      break;
    case KEY_UP:
      if (floor1.gameMatrix[posY - 1][posX] == '.') {
        posY -= 1;
      }
      break;
    case KEY_DOWN:
      if (floor1.gameMatrix[posY + 1][posX] == '.') {
        posY += 1;
      }
      break;
    }
    floor1.renderMap();
    mvwaddch(gamespace, posY, posX, '@');
    mvwprintw(footer, 3, 30, "X: %d , Y: %d ", posX, posY);
    wrefresh(footer);
    wrefresh(gamespace);
  }
  curs_set(1);
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
      mvwprintw(menu_win, y, x, "%s", choices[i].c_str());
      wattroff(menu_win, A_REVERSE);
    } else
      mvwprintw(menu_win, y, x, "%s", choices[i].c_str());
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
      mvwprintw(menu_win, y, x, "%s", choices_right[i].c_str());
      wattroff(menu_win, A_REVERSE);
    } else
      mvwprintw(menu_win, y, x, "%s", choices_right[i].c_str());
    ++y;
  }
  wrefresh(menu_win);
}
