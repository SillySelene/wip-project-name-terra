#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <ncurses.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <vector>

using namespace std;

#define SIDEBAR_WIDTH 30
#define SIDEBAR_HEIGHT LINES - 1

#define MAP_WIDTH 79
#define MAP_HEIGHT 29

const int MONSTERSAMOUNT = 5;

int startx = 0;
int starty = 0;

map<string, map<string, int>> mobLocationList;
vector<string> listUUID;

map<string, string> monsters[MONSTERSAMOUNT] = {
    {{"name", "Vampire Bat"}, {"sprite", "V"}},
    {{"name", "Raskghar"}, {"sprite", "G"}},
    {{"name", "Living Armor"}, {"sprite", "5"}},
    {{"name", "Ogre"}, {"sprite", "O"}},
    {{"name", "Troll"}, {"sprite", "8"}},

};

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

string makeUUID() {
  while (true) {
    int isValid = 1;
    int testUUID = rand();
    int i;
    for (i = 0; i < listUUID.size(); i++) {
      if (std::to_string(testUUID) == listUUID.at(i)) {
        isValid = 0;
      }
    }
    if (isValid == 1) {
      listUUID.push_back(std::to_string(testUUID));
      return std::to_string(testUUID);
    }
  }
}

class Entity {
public:
  map<string, string> name;
  int constitution;
  int dexterity;
  int strength;
  int intelligence;
  int wisdom;
  int perception;
  int level;
  int healthMax;
  int health;
  int xPos;
  int yPos;
  string UUID;

  int maxHealth() { return constitution * 2 + (strength / 2); }
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

  void moveTowards(int y, int x) {
    /*
     * TODO: make this function
     */
    if (posX > x) {
      /* move */
    }
  }
  /* Calculator Damage */
  int calcDamage() { return strength * 2 + (constitution / 2); }

  void turn(Entity player) {
    int i, j;
    for (i = -1; i < 3; i++) {
      for (j = -1; j < 3; j++) {
        if (xPos == posX && yPos == posY) {
          /* Player takes damage */
          player.takeDamage(calcDamage());
        }
      }
    }

    /* check for any players nearby, then move to them */

    int visRange = (int)(perception / 2.0) + 3;

    for (i = visRange * -1; j < visRange; i++) {
      for (j = visRange * -1; j < visRange; j++) {
        if (xPos + j == posX && yPos + i == yPos) {
          moveTowards(posY, posX);
        }
      }
    }
  }

  Entity(map<string, string> myName, int con, int dex, int str, int intel,
         int wis, int per, int lvl, int startingX, int startingY) {
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
    UUID = makeUUID();

    /* string, map<string, int> */

    yPos = startingY;
    xPos = startingX;
    if (name.at("name").compare("player") != 0) {
      mobLocationList[UUID]["x"] = xPos;
      mobLocationList[UUID]["y"] = yPos;
    }
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

map<string, string> playerData = {{"name", "player"}, {"sprite", "@"}};
Entity player(playerData, 3, 3, 3, 3, 3, 3, 1, posX, posY);

Entity initMonster(int floorNum, int y, int x) {

  int rmons = rand() % MONSTERSAMOUNT;
  map<string, string> monster = monsters[0];

  int lvl = rand() % (floorNum * 3) + 1;

  int con = rand() % (lvl * 2) + 1;
  int dex = rand() % (lvl * 2) + 1;
  int str = rand() % (lvl * 2) + 1;
  int intel = rand() % (lvl * 2) + 1;
  int wis = rand() % (lvl * 2) + 1;
  int per = rand() % (lvl * 2) + 1;

  Entity localMonster(monster, con, dex, str, intel, wis, per, lvl, y, x);

  return localMonster;
};

int checkLocation(int y, int x) {
  int i;
  for (i = 1; i < listUUID.size(); i++) {
    if (x == mobLocationList.at(listUUID[i]).at("x") &&
        y == mobLocationList.at(listUUID[i]).at("y")) {
      return 0;
    }
  }
  return 1;
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
      if (highlight == 1) {
        highlight = n_choices;
      } else {
        --highlight;
      }
      break;
    case KEY_DOWN:
      if (highlight == n_choices) {
        highlight = 1;
      } else {
        ++highlight;
        break;
      }
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

  srand(time(0));

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

  Entity myMonster = initMonster(1, 5, 5);

  while (continueGame == 1 && player.health > 0) {
    int ch = wgetch(gamespace);
    floor1.renderMap();
    switch (ch) {
    case KEY_BACKSPACE:
      continueGame = 0;
      break;
    case KEY_LEFT:
      if (floor1.gameMatrix[posY][posX - 1] == '.' &&
          checkLocation(posY, posX - 1) == 1) {
        posX -= 1;
      }
      break;
    case KEY_RIGHT:
      if (floor1.gameMatrix[posY][posX + 1] == '.' &&
          checkLocation(posY, posX + 1) == 1) {
        posX += 1;
      }
      break;
    case KEY_UP:
      if (floor1.gameMatrix[posY - 1][posX] == '.' &&
          checkLocation(posY - 1, posX) == 1) {
        posY -= 1;
      }
      break;
    case KEY_DOWN:
      if (floor1.gameMatrix[posY + 1][posX] == '.' &&
          checkLocation(posY + 1, posX) == 1) {
        posY += 1;
      }
      break;
    case ' ':
      mvwaddch(footer, 3, 5, '8');
      break;
    case '1':
      if (floor1.gameMatrix[posY + 1][posX - 1] == '.' &&
          checkLocation(posY + 1, posX - 1) == 1) {
        posX--;
        posY++;
      }
      break;
    case '2':
      if (floor1.gameMatrix[posY + 1][posX] == '.' &&
          checkLocation(posY + 1, posX) == 1) {
        posY++;
      }
      break;
    case '3':
      if (floor1.gameMatrix[posY + 1][posX + 1] == '.' &&
          checkLocation(posY + 1, posX + 1) == 1) {
        posX++;
        posY++;
      }
      break;
    case '4':
      if (floor1.gameMatrix[posY][posX - 1] == '.' &&
          checkLocation(posY, posX - 1) == 1) {
        posX--;
      }
      break;
    case '5':
      break;
    case '6':
      if (floor1.gameMatrix[posY][posX + 1] == '.' &&
          checkLocation(posY, posX + 1) == 1) {
        posX++;
      }
      break;
    case '7':
      if (floor1.gameMatrix[posY - 1][posX - 1] == '.' &&
          checkLocation(posY - 1, posX - 1) == 1) {
        posX--;
        posY--;
      }
      break;
    case '8':
      if (floor1.gameMatrix[posY - 1][posX] == '.' &&
          checkLocation(posY - 1, posX) == 1) {
        posY--;
      }
      break;
    case '9':
      if (floor1.gameMatrix[posY - 1][posX + 1] == '.' &&
          checkLocation(posY - 1, posX + 1) == 1) {
        posX++;
        posY--;
      }
      break;
    }
    floor1.renderMap();
    mvwaddch(gamespace, posY, posX, player.name["sprite"].c_str()[0]);
    mvwaddch(gamespace, myMonster.yPos, myMonster.xPos,
             myMonster.name["sprite"].c_str()[0]);
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
