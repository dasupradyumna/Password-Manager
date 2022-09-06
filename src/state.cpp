#include "state.hpp"

#include "ncurses.hpp"

#include <filesystem>

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
    update_actions();
    ncur::clear();
    ncur::printw("~~~~~~~~~~ %s FILE ~~~~~~~~~~", __m_delete ? "DELETE" : "SELECT");
    const auto &choice { screen::display() };
    if (__m_delete)
      delete_file(choice.m_description);
    else
      __m_delete = choice.m_description == "Delete ...";
    return choice;
  }

  void select_file::update_actions()
  {
    _m_actions.clear();
    _m_choice = 0;

    using namespace std;
    if (filesystem::exists(__m_save_dir))
      for (const auto &entry : filesystem::directory_iterator { __m_save_dir }) {
        stringstream act;
        act << _m_actions.size() + 1 << ". " << entry.path().filename().string();
        _m_actions.push_back(action { act.str(), states::SELECTFILE,
          __m_delete ? states::SELECTFILE : states::DBOPEN });
      }

    if (!__m_delete) {
      _m_actions.push_back(action { "New ...", states::SELECTFILE, states::DBOPEN });
      _m_actions.push_back(
        action { "Delete ...", states::SELECTFILE, states::SELECTFILE });
    }
    _m_actions.push_back(action {
      "Back", states::SELECTFILE, __m_delete ? states::SELECTFILE : states::TITLE });
  }

  void select_file::delete_file(const std::string &option)
  {
    using namespace std;

    if (option == "Back")
      __m_delete = false;
    else {
      const int id { atoi(option.substr(0, option.find('.')).c_str()) };
      ncur::mvprintw(id + 1, 2, ">>> %s\t: Delete ? Yes[y] / No[n]", option.c_str());
      ncur::refresh();
      while (true) switch (ncur::getch()) {
          case 'y': filesystem::remove(string { __m_save_dir } + "/" + option.substr(3));
          case 'n': return;
        }
    }
  }

  // TODO implement
  action db_open::display() { return action {}; }
}  // namespace pm
