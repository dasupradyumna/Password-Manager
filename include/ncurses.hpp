#ifndef PASSWORD_MANAGER_INCLUDE_NCURSES_HPP_1662085557
#define PASSWORD_MANAGER_INCLUDE_NCURSES_HPP_1662085557

#include <ncurses.h>

namespace ncur
{
  // start-end functions
  using ::cbreak;
  using ::curs_set;
  using ::endwin;
  using ::initscr;
  using ::keypad;

  // display functions
  using ::addch;
  using ::clear;
  using ::delch;
  using ::echo;
  using ::insch;
  using ::move;
  using ::mvprintw;
  using ::noecho;
  using ::printw;
  using ::refresh;
  using ::waddch;
  using ::wdelch;
  using ::winsch;

  // input functions
  using ::getch;
  using ::wgetch;
}  // namespace ncur

#endif
