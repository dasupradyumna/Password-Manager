#ifndef PASSWORD_MANAGER_INCLUDE_NCURSES_HPP_1662085557
#define PASSWORD_MANAGER_INCLUDE_NCURSES_HPP_1662085557

#include <ncurses.h>

namespace ncur
{
  using ::cbreak;
  using ::clear;
  using ::endwin;
  using ::getch;
  using ::initscr;
  using ::mvprintw;
  using ::noecho;
  using ::printw;
  using ::refresh;
  using ::wgetch;
}  // namespace ncur

#endif
