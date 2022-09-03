#include "state.hpp"

#include "ncurses.hpp"

namespace pm
{
  action screen::display()
  {
    while (true) {
      for (std::size_t i { 0 }; i < _m_actions.size(); ++i)
        ncur::mvprintw(i + 2, 4, "%s %s", (_m_choice == i ? ">" : " "),
          _m_actions[i].m_description.c_str());

      ncur::refresh();

      switch (ncur::getch()) {
        case 'j': _m_choice += (_m_choice < _m_actions.size() - 1); break;
        case 'k': _m_choice -= (_m_choice > 0); break;
        case '\n': return _m_actions[_m_choice];
      }
    }
  }

  title::title() : screen {}
  {
    _m_actions.push_back(action { "Select file", states::TITLE, states::SELECTFILE });
    _m_actions.push_back(action { "Settings", states::TITLE, states::SETTINGS });
    _m_actions.push_back(action { "Exit", states::TITLE, states::EXIT });
  }

  settings::settings() : screen {}
  {
    _m_actions.push_back(action { "Back", states::SETTINGS, states::TITLE });
  }

  select_file::select_file() : screen {}
  {
    _m_actions.push_back(action { "Back", states::SELECTFILE, states::TITLE });
  }

  // TODO implement
  db_open::db_open() : screen {} { }

  action title::display()
  {
    ncur::clear();
    ncur::printw("~~~~~~~~~~ PASSWORD-MANAGER ~~~~~~~~~~");
    return screen::display();
  }

  action settings::display()
  {
    ncur::clear();
    ncur::printw("~~~~~~~~~~ SETTINGS ~~~~~~~~~~");
    return screen::display();
  }

  action select_file::display()
  {
    ncur::clear();
    ncur::printw("~~~~~~~~~~ SELECT FILE ~~~~~~~~~~");
    return screen::display();
  }

  // TODO implement
  action db_open::display() { return action {}; }
}  // namespace pm
