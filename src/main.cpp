#include "ncurses.hpp"

#include <iostream>

int main()
{
  ncur::initscr();
  ncur::mvprintw(10, 10, "IT WORKS!!");
  ncur::refresh();
  int x { ncur::getch() };
  ncur::endwin();
  std::cout << (char)x << '\n';
  return 0;
}
