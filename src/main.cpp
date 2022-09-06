#include "application.hpp"
#include "ncurses.hpp"
#include "state.hpp"

int main()
{
  ncur::initscr();
  ncur::cbreak();
  ncur::noecho();
  ncur::curs_set(0);

  auto &app { pm::application::instance() };
  while (app.get_state() != pm::states::EXIT) {
    const auto &input { app.display() };
    app.update(input);
  }

  ncur::endwin();
  return EXIT_SUCCESS;
}
