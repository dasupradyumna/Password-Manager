#include "state.hpp"

#include "ncurses.hpp"

#include <cctype>
#include <filesystem>

namespace pm
{
  // global variables
  std::string g_db_name {};

  void screen::display_actions(const int menu_offset) const
  {
    ncur::mvprintw(1, menu_offset, "|");
    for (std::size_t i { 0 }; i < _m_actions.size(); ++i)
      ncur::mvprintw(i + 2, menu_offset, "| %s %s", (_m_choice == i ? ">" : " "),
        _m_actions[i].m_description.c_str());
    ncur::refresh();
  }

  action screen::get_action(const int menu_offset)
  {
    while (true) {
      display_actions(menu_offset);
      switch (ncur::getch()) {
        case 'j': _m_choice += (_m_choice < _m_actions.size() - 1); break;
        case 'k': _m_choice -= (_m_choice > 0); break;
        case '\t':
          if (menu_offset == 43)
            return action { "", states::DBOPEN, states::DBOPEN };  // HACK improve design
          break;
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

  action title::get_action(const int)
  {
    ncur::clear();
    ncur::printw("~~~~~~~~~~~~~~~~~~~~~ PASSWORD-MANAGER ~~~~~~~~~~~~~~~~~~~~~\n|");
    return screen::get_action();
  }

  settings::settings() : screen {}
  {
    _m_actions.push_back(action { "Back", states::SETTINGS, states::TITLE });
  }

  action settings::get_action(const int)
  {
    ncur::clear();
    ncur::printw("~~~~~~~~~~~~~~~~~~~~~~~~~ SETTINGS ~~~~~~~~~~~~~~~~~~~~~~~~~\n|");
    return screen::get_action();
  }

  action select_file::get_action(const int)
  {
    update_actions();
    ncur::clear();
    ncur::printw("~~~~~~~~~~~~~~~~~~~~~~~ %s FILE ~~~~~~~~~~~~~~~~~~~~~~~~\n|",
      __m_delete ? "DELETE" : "SELECT");

    const auto &choice { screen::get_action() };
    if (__m_delete)
      delete_file(choice.m_description);
    else
      __m_delete = choice.m_description == "Delete Files";
    return choice;
  }

  void select_file::update_actions()
  {
    _m_actions.clear();
    _m_choice = 0;

    using namespace std;
    if (filesystem::exists(g_save_dir))
      for (const auto &entry : filesystem::directory_iterator { g_save_dir }) {
        stringstream act;
        act << _m_actions.size() + 1 << ". " << entry.path().filename().string();
        _m_actions.push_back(action { act.str(), states::SELECTFILE,
          __m_delete ? states::SELECTFILE : states::DBOPEN });
      }

    if (!__m_delete) {
      _m_actions.push_back(action { "New File", states::SELECTFILE, states::DBOPEN });
      _m_actions.push_back(
        action { "Delete Files", states::SELECTFILE, states::SELECTFILE });
    }
    _m_actions.push_back(action {
      "Back", states::SELECTFILE, __m_delete ? states::SELECTFILE : states::TITLE });
  }

  void select_file::delete_file(const std::string &option)
  {
    namespace fs = std::filesystem;

    if (option == "Back")
      __m_delete = false;
    else {
      const int id { atoi(option.substr(0, option.find('.')).c_str()) };
      ncur::mvprintw(id + 1, 1, ">> %s\t: Delete ? Yes[y] / No[n]", option.c_str());
      ncur::refresh();
      while (true) switch (ncur::getch()) {
          case 'y':
            fs::remove(fs::path { g_save_dir } / option.substr(option.find('.') + 2));
          case 'n': return;
        }
    }
  }

  db_open::db_open()
    : screen {},
      __m_active { 0 },
      __m_focus_db { false },
      __m_delete { false },
      __m_core { std::make_unique<database>() }
  { }

  void db_open::update_actions()
  {
    _m_actions.clear();
    if (__m_delete)
      _m_actions.push_back(action { "Back", states::DBOPEN, states::DBOPEN });
    else {
      _m_actions.push_back(action { "New", states::DBOPEN, states::ENTRYVIEW });
      _m_actions.push_back(action { "Delete", states::DBOPEN, states::DBOPEN });
      _m_actions.push_back(action { "Back", states::DBOPEN, states::SELECTFILE });
    }
  }

  action db_open::get_action(const int)
  {
    update_actions();
    ncur::clear();
    ncur::printw("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    if (__m_core->empty())
      __m_focus_db = false;
    else {
      ncur::printw("\n|\n|");
      if (__m_active >= __m_core->size()) __m_active = __m_core->size() - 1;
    }
    ncur::mvprintw(
      0, 30 - g_db_name.size() / 2, " %s ", get_upper_case(g_db_name).c_str());

    std::size_t i { 0 };
    for (const auto &ent : *__m_core) {
      ncur::mvprintw(3 * ++i, 0, "|\n|");
      ncur::mvprintw(
        3 * i, 4, "%s  ( %s )", ent->m_title.c_str(), ent->m_website.c_str());
      ncur::mvprintw(
        3 * i + 1, 8, "created - %s", unix_to_datetime(ent->m_time_created).c_str());
      ncur::mvprintw(3 * i + 2, 0, "| - - - - - - - - - - - - - - - - - - - - -|");
    }

    const auto &choice { __m_focus_db ? database_input() : screen::get_action(43) };
    if (choice.m_description.empty())
      __m_focus_db = !__m_focus_db;
    else if (choice.m_description == "Delete")
      __m_focus_db = __m_delete = true;
    else if (choice.m_description == "Back" && __m_delete)
      __m_delete = false;
    return choice;
  }

  action db_open::database_input()
  {
    std::size_t prev_active { __m_active };
    _m_choice = -1;  // HACK to make menu cursor invisible. Improve design
    screen::display_actions(43);
    while (true) {
      ncur::mvprintw(3 * (prev_active + 1), 2, " ");  // reset selection
      ncur::mvprintw(3 * (__m_active + 1), 2, ">");   // render selection
      ncur::refresh();

      prev_active = __m_active;
      switch (ncur::getch()) {
        case 'j': __m_active += (__m_active < __m_core->size() - 1); break;
        case 'k': __m_active -= (__m_active > 0); break;
        case '\t': _m_choice = 0; return action { "", states::DBOPEN, states::DBOPEN };
        case '\n':
          _m_choice = 0;
          return action { std::to_string(__m_active), states::DBOPEN, states::ENTRYVIEW };
      }
    }
  }

  entry_view::entry_view(const std::unique_ptr<database> &core_db)
    : screen {}, __m_core { core_db.get() }
  {
    _m_actions.push_back(action { "Edit", states::ENTRYVIEW, states::ENTRYVIEW });
    _m_actions.push_back(action { "Delete", states::ENTRYVIEW, states::ENTRYVIEW });
    _m_actions.push_back(action { "Back", states::ENTRYVIEW, states::DBOPEN });
  }

  action entry_view::get_action(const int)
  {
    ncur::clear();
    ncur::printw("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n|");
    const std::string title { __m_view == NEW ? "[ UNNAMED ]" : (*__m_entry)->m_title };
    ncur::mvprintw(0, 30 - title.size() / 2, " %s ", get_upper_case(title).c_str());
    ncur::mvprintw(2, 0, "|   Created :");
    ncur::mvprintw(3, 0, "|   Modified :");

    ncur::mvprintw(5, 0, "|   Website :");
    ncur::mvprintw(6, 0, "|   Username :");
    ncur::mvprintw(7, 0, "|   E-mail :");
    ncur::mvprintw(8, 0, "|   Password :");
    ncur::mvprintw(9, 0, "|   Notes :");

    static constexpr std::size_t cursorpos[8] { 14, 15, 12, 14, 15, 13, 15, 12 };

    switch (__m_view) {
      case VIEW: return view_entry(cursorpos);
      case NEW: return input_entry_details(cursorpos, true);
      case EDIT: return input_entry_details(cursorpos, false);
      case DELETE: return delete_entry(cursorpos);
      default: return action {};
    }
  }

  action entry_view::view_entry(const std::size_t *cursorpos)
  {
    ncur::mvprintw(
      2, cursorpos[0], unix_to_datetime((*__m_entry)->m_time_created).c_str());
    ncur::mvprintw(
      3, cursorpos[1], unix_to_datetime((*__m_entry)->m_time_modified).c_str());
    ncur::mvprintw(5, cursorpos[3], (*__m_entry)->m_website.c_str());
    ncur::mvprintw(6, cursorpos[4], (*__m_entry)->m_username.c_str());
    ncur::mvprintw(7, cursorpos[5], (*__m_entry)->m_email.c_str());
    ncur::mvprintw(8, cursorpos[6], (*__m_entry)->m_password.c_str());
    ncur::mvprintw(9, cursorpos[7], (*__m_entry)->m_notes.c_str());

    return screen::get_action(45);
  }

  inline bool is_valid_char(char ch, bool is_title_or_notes)
  {
    return std::isalnum(ch) || std::ispunct(ch) || (ch == ' ' && is_title_or_notes);
    // TODO add support for wide characters || ch == '₹' || ch == '€';
  }

  action entry_view::input_entry_details(const std::size_t *cursorpos, bool is_new)
  {
    entry NEW { std::time(nullptr) };
    entry &ent { is_new ? NEW : **__m_entry };
    std::size_t curr { 0 }, prev { 0 };
    std::string input {};

    // replace created and modified times with Save and Back options
    ncur::mvprintw(2, 0, "|   Save          ");
    ncur::mvprintw(3, 0, "|   Back           ");
    ncur::mvprintw(4, 0, "|   Title : %s", ent.m_title.c_str());
    if (!is_new) {
      ncur::mvprintw(5, cursorpos[3], ent.m_website.c_str());
      ncur::mvprintw(6, cursorpos[4], ent.m_username.c_str());
      ncur::mvprintw(7, cursorpos[5], ent.m_email.c_str());
      ncur::mvprintw(8, cursorpos[6], ent.m_password.c_str());
      ncur::mvprintw(9, cursorpos[7], ent.m_notes.c_str());
    }
    bool selected { false };
    while (true) {

      ncur::mvprintw(prev + 2, 2, " ");
      ncur::mvprintw(curr + 2, 2, ">");
      ncur::refresh();

      prev = curr;
      switch (ncur::getch()) {
        case 'j': curr += (curr < 7); break;
        case 'k': curr -= (curr > 0); break;
        case '\n': selected = true; break;
      }

      std::size_t loc;  // to handle deleting characters
      if (selected) {
        ncur::mvprintw(curr + 2, 1, ">");
        if (curr == 0 || curr == 1)
          break;
        else {
          ncur::curs_set(1);
          switch (curr) {
            case 2: input = ent.m_title; break;
            case 3: input = ent.m_website; break;
            case 4: input = ent.m_username; break;
            case 5: input = ent.m_email; break;
            case 6: input = ent.m_password; break;
            case 7: input = ent.m_notes; break;
          }
          loc = cursorpos[curr] + input.size();
          ncur::move(curr + 2, loc);
        }
      }
      while (selected) {
        if (curr == 2) {  // Update heading when title field is modified
          ncur::mvprintw(
            0, 0, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n|");
          const std::string title { input.empty() ? "[ UNNAMED ]" : input };
          ncur::mvprintw(0, 30 - title.size() / 2, " %s ", get_upper_case(title).c_str());
          ncur::move(curr + 2, loc);
        }

        auto inp { ncur::getch() };
        if (inp == '\n') {
          ncur::curs_set(0);
          selected = false;
          switch (curr) {
            case 2: ent.m_title = input; break;
            case 3: ent.m_website = input; break;
            case 4: ent.m_username = input; break;
            case 5: ent.m_email = input; break;
            case 6: ent.m_password = input; break;
            case 7: ent.m_notes = input; break;
          }
          input = "";
        } else if (inp == KEY_BACKSPACE && !input.empty()) {
          ncur::move(curr + 2, --loc);
          ncur::delch();
          input.pop_back();
        } else if (is_valid_char(inp, (curr == 2 || curr == 7))) {
          ++loc;
          ncur::addch(inp);
          input.push_back(inp);
        }
      }
      ncur::mvprintw(curr + 2, 1, " ");
    }

    if (curr == 0 && is_new) __m_core->new_entry(ent);
    return action { "Back", states::ENTRYVIEW, states::DBOPEN };
  }

  action entry_view::delete_entry(const std::size_t *cursorpos)
  {
    ncur::mvprintw(
      2, cursorpos[0], unix_to_datetime((*__m_entry)->m_time_created).c_str());
    ncur::mvprintw(
      3, cursorpos[1], unix_to_datetime((*__m_entry)->m_time_modified).c_str());

    ncur::mvprintw(5, cursorpos[3], (*__m_entry)->m_website.c_str());
    ncur::mvprintw(6, cursorpos[4], (*__m_entry)->m_username.c_str());
    ncur::mvprintw(7, cursorpos[5], (*__m_entry)->m_email.c_str());
    ncur::mvprintw(8, cursorpos[6], (*__m_entry)->m_password.c_str());
    ncur::mvprintw(9, cursorpos[7], (*__m_entry)->m_notes.c_str());

    ncur::mvprintw(2, 45, "Delete this entry?");
    ncur::mvprintw(3, 45, "Yes[y] / No[n]");
    ncur::refresh();

    while (true) switch (ncur::getch()) {
        case 'y': __m_core->delete_entry(__m_entry);
        case 'n': return action { "Back", states::ENTRYVIEW, states::DBOPEN };
      }
  }
}  // namespace pm
