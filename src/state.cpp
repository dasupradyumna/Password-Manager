#include "state.hpp"

#include "ncurses.hpp"

#include <cctype>

namespace pm
{
  // global variables
  std::string g_db_name {};

  void screen::display_actions(const cursorloc menu_offset) const
  {
    ncur::mvprintw(3, menu_offset, "|");
    ncur::mvprintw(3, 59, "|");
    for (selector i { 0 }; i < _m_actions.size(); ++i) {
      ncur::mvprintw(i + 4, menu_offset, "| %s %s", (_m_choice == i ? ">" : " "),
        _m_actions[i].m_description.c_str());
      ncur::mvprintw(i + 4, 59, "|");
    }
    ncur::mvprintw(_m_actions.size() + 4, menu_offset, "|%s|",
      std::string(58 - menu_offset, ' ').c_str());
    ncur::mvprintw(_m_actions.size() + 5, menu_offset, "+%s+",
      std::string(58 - menu_offset, '-').c_str());
    ncur::refresh();
  }

  action screen::get_action(const cursorloc menu_offset)
  {
    const auto sz { static_cast<selector>(_m_actions.size()) };
    while (true) {
      display_actions(menu_offset);
      switch (ncur::getch()) {
        case 'j': _m_choice = (_m_choice + 1) % sz; break;
        case 'k': _m_choice = (sz + _m_choice - 1) % sz; break;
        case '\t':
          if (menu_offset == 46)
            return action { "", states::db_open,
              states::db_open };  // HACK improve design
          break;
        case '\n': return _m_actions[_m_choice];
      }
    }
  }

  title::title() : screen {}
  {
    _m_actions.push_back(action { "Select file", states::title, states::select_file });
    _m_actions.push_back(action { "Settings", states::title, states::settings });
    _m_actions.push_back(action { "Exit", states::title, states::exit });
  }

  action title::get_action(const cursorloc)
  {
    ncur::clear();
    ncur::printw("+----------------------------------------------------------+\n");
    ncur::printw("|                     PASSWORD-MANAGER                     |\n");
    ncur::printw("+----------------------------------------------------------+\n");
    return screen::get_action();
  }

  settings::settings() : screen {}
  {
    _m_actions.push_back(action { "Back", states::settings, states::title });
  }

  action settings::get_action(const cursorloc)
  {
    ncur::clear();
    ncur::printw("+----------------------------------------------------------+\n");
    ncur::printw("|                         SETTINGS                         |\n");
    ncur::printw("+----------------------------------------------------------+\n");
    return screen::get_action();
  }

  action select_file::get_action(const cursorloc)
  {
    update_actions();
    ncur::clear();
    ncur::printw("+----------------------------------------------------------+\n");
    ncur::printw("|                       %s FILE                        |\n",
      __m_delete ? "DELETE" : "SELECT");
    ncur::printw("+----------------------------------------------------------+\n");

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
        _m_actions.push_back(action { act.str(), states::select_file,
          __m_delete ? states::select_file : states::db_open });
      }

    if (!__m_delete) {
      _m_actions.push_back(action { "New File", states::select_file, states::db_open });
      _m_actions.push_back(
        action { "Delete Files", states::select_file, states::select_file });
    }
    _m_actions.push_back(action {
      "Back", states::select_file, __m_delete ? states::select_file : states::title });
  }

  void select_file::delete_file(const std::string &option)
  {
    namespace fs = std::filesystem;

    if (option == "Back")
      __m_delete = false;
    else {
      const cursorloc id { atoi(option.substr(0, option.find('.')).c_str()) };
      ncur::mvprintw(id + 3, 1, ">> %s\t: Delete ? Yes[y] / No[n]", option.c_str());
      ncur::refresh();
      while (true) switch (ncur::getch()) {
          case 'y':
            fs::remove(fs::path { g_save_dir } / option.substr(option.find('.') + 2));
          case 'n': return;
        }
    }
  }

  db_open::db_open(database &core_db)
    : screen {},
      __m_active { 0 },
      __m_focus_db { false },
      __m_delete { false },
      __m_core { core_db }
  { }

  void db_open::update_actions()
  {
    _m_actions.clear();
    _m_choice = 0;
    if (__m_delete)
      _m_actions.push_back(action { "Back", states::db_open, states::db_open });
    else if (__m_save) {
      _m_actions.push_back(action { "Yes", states::db_open, states::db_open });
      _m_actions.push_back(action { "No", states::db_open, states::db_open });
    } else {
      _m_actions.push_back(action { "New", states::db_open, states::entry_view });
      _m_actions.push_back(action { "Delete", states::db_open, states::db_open });
      _m_actions.push_back(action { "Save", states::db_open, states::db_open });
      _m_actions.push_back(action { "Back", states::db_open, states::select_file });
    }
  }

  void db_open::name_new_database()
  {
    ncur::curs_set(1);
    std::string name {};
    int inp {};
    while (inp != '\n') {
      ncur::mvprintw(
        1, 0, "|                                                          |");
      const auto title { name.empty() ? "< Name the new database >" : name };
      ncur::mvprintw(1, 31 - title.size() / 2, "%s", title.c_str());
      ncur::refresh();

      inp = ncur::getch();
      if (name.size() < 20 && (std::isalnum(inp) || inp == '-' || inp == '_'))
        name.push_back(inp);
      else if (inp == KEY_BACKSPACE && !name.empty())
        name.pop_back();
    }
    g_db_name = name;
    ncur::curs_set(0);
  }

  action db_open::get_action(const cursorloc)
  {
    update_actions();
    ncur::clear();
    ncur::printw("+----------------------------------------------------------+\n");
    ncur::printw("|                                                          |\n");
    ncur::printw("+---------------------------------------------+------------+\n");
    if (__m_save) {  // Save mode
      __m_focus_db = false;
      ncur::mvprintw(1, 51, "(Save?)");
    } else if (__m_core.empty())  // TAB switch
      __m_focus_db = false;
    else {  // View or Delete mode
      __m_active -= (__m_active == __m_core.size());
      if (__m_delete)
        ncur::mvprintw(1, 50, "(Delete)");
      else
        ncur::mvprintw(1, 52, "(View)");
    }
    ncur::mvprintw(
      1, 30 - g_db_name.size() / 2, " %s ", get_upper_case(g_db_name).c_str());

    cursorloc i { 0 };
    for (const auto &ent : __m_core) {
      ncur::mvprintw(3 * ++i, 0, "|\n|");
      std::stringstream display;
      display << ent.m_title() << "  ( " << ent.m_website() << " )";
      ncur::mvprintw(3 * i, 4, display.str().substr(0, 42).c_str());
      ncur::mvprintw(3 * i, 46, "|");
      ncur::mvprintw(
        3 * i + 1, 8, "created - %s", unix_to_datetime(ent.m_time_created()).c_str());
      ncur::mvprintw(3 * i + 1, 46, "|");
      ncur::mvprintw(3 * i + 2, 0, "| - - - - - - - - - - - - - - - - - - - - - - |");
    }
    ncur::mvprintw(
      3 * __m_core.size() + 2, 0, "+---------------------------------------------+");

    const auto &choice { __m_focus_db ? database_input() : screen::get_action(46) };
    if (choice.m_description.empty())
      __m_focus_db = !__m_focus_db;
    else if (choice.m_description == "Save")
      __m_save = true;
    else if (__m_save) {
      __m_save = false;
      if (g_db_name == "[ UNNAMED ]" && choice.m_description == "Yes")
        name_new_database();
    } else if (choice.m_description == "Delete")
      __m_focus_db = __m_delete = true;
    else if (choice.m_description == "Back" && __m_delete)
      __m_delete = false;
    return choice;
  }

  action db_open::database_input()
  {
    const auto sz { static_cast<selector>(__m_core.size()) };
    selector prev_active { __m_active };
    _m_choice = -1;  // HACK to make menu cursor invisible. Improve design
    screen::display_actions(46);
    while (true) {
      ncur::mvprintw(3 * (prev_active + 1), 2, " ");  // reset selection
      ncur::mvprintw(3 * (__m_active + 1), 2, ">");   // render selection
      ncur::refresh();

      prev_active = __m_active;
      switch (ncur::getch()) {
        case 'j': __m_active = (__m_active + 1) % sz; break;
        case 'k': __m_active = (sz + __m_active - 1) % sz; break;
        case '\t': _m_choice = 0; return action { "", states::db_open, states::db_open };
        case '\n':
          _m_choice = 0;
          return action { std::to_string(__m_active), states::db_open,
            states::entry_view };
      }
    }
  }

  entry_view::entry_view(database &core_db) : screen {}, __m_core { core_db }
  {
    _m_actions.push_back(action { "Edit", states::entry_view, states::entry_view });
    _m_actions.push_back(action { "Delete", states::entry_view, states::entry_view });
    _m_actions.push_back(action { "Back", states::entry_view, states::db_open });
  }

  action entry_view::get_action(const cursorloc)
  {
    ncur::clear();
    ncur::printw("+----------------------------------------------------------+\n");
    ncur::printw("|                                                          |\n");
    ncur::printw("+----------------------------------------------------------+\n|");
    const std::string title { __m_view == NEW ? "[ NEW ENTRY ]" : __m_entry->m_title() };
    ncur::mvprintw(1, 30 - title.size() / 2, " %s ", get_upper_case(title).c_str());
    ncur::mvprintw(4, 0, "|   Created :");
    ncur::mvprintw(5, 0, "|   Modified :");
    ncur::mvprintw(6, 0, "|");
    ncur::mvprintw(7, 0, "|   Website :");
    ncur::mvprintw(8, 0, "|   Username :");
    ncur::mvprintw(9, 0, "|   E-mail :");
    ncur::mvprintw(10, 0, "|   Password :");
    ncur::mvprintw(11, 0, "|   Notes :");
    ncur::mvprintw(12, 0, "|");

    static constexpr cursorloc cursorpos[8] { 14, 15, 12, 14, 15, 13, 15, 12 };

    switch (__m_view) {
      case VIEW: return view_entry(cursorpos);
      case NEW: return input_entry_details(cursorpos, true);
      case EDIT: return input_entry_details(cursorpos, false);
      case DELETE: return delete_entry(cursorpos);
      default: return action {};
    }
  }

  action entry_view::view_entry(const cursorloc *cursorpos)
  {
    ncur::mvprintw(2, 46, "+");
    ncur::mvprintw(
      4, cursorpos[0], unix_to_datetime(__m_entry->m_time_created()).c_str());
    ncur::mvprintw(
      5, cursorpos[1], unix_to_datetime(__m_entry->m_time_modified()).c_str());
    ncur::mvprintw(7, cursorpos[3], __m_entry->m_website().c_str());
    ncur::mvprintw(8, cursorpos[4], __m_entry->m_username().c_str());
    ncur::mvprintw(9, cursorpos[5], __m_entry->m_email().c_str());
    ncur::mvprintw(9, 46, "|");
    ncur::mvprintw(10, cursorpos[6], __m_entry->m_password().c_str());
    ncur::mvprintw(10, 46, "|");
    ncur::mvprintw(11, cursorpos[7], __m_entry->m_notes().c_str());
    ncur::mvprintw(11, 46, "|");
    ncur::mvprintw(12, 46, "|\n+%s+", std::string(45, '-').c_str());

    return screen::get_action(46);
  }

  inline bool is_valid_char(char ch, bool is_title_or_notes)
  {
    return std::isalnum(ch) || std::ispunct(ch) || (ch == ' ' && is_title_or_notes);
    // TODO add support for wide characters || ch == '₹' || ch == '€';
  }

  inline bool is_properly_filled(const entry &ent)
  {
    return !ent.m_title().empty() && !ent.m_website().empty() && !ent.m_password().empty()
        && !(ent.m_email().empty() && ent.m_username().empty());
  }

  action entry_view::input_entry_details(const cursorloc *cursorpos, bool is_new)
  {
    entry NEW { static_cast<unixtime>(std::time(nullptr)) };
    entry ent { is_new ? NEW : *__m_entry };
    selector curr { 0 }, prev { 0 };
    std::string input {};

    // replace created and modified times with Save and Back options
    ncur::mvprintw(4, 0, "|   Save          ");
    ncur::mvprintw(5, 0, "|   Back           ");
    ncur::mvprintw(6, 0, "|   Title : %s", ent.m_title().c_str());
    for (selector i { 3 }; i < 13; ++i) ncur::mvprintw(i, 59, "|");
    ncur::printw("\n+----------------------------------------------------------+\n");
    if (!is_new) {
      ncur::mvprintw(7, cursorpos[3], ent.m_website().c_str());
      ncur::mvprintw(8, cursorpos[4], ent.m_username().c_str());
      ncur::mvprintw(9, cursorpos[5], ent.m_email().c_str());
      ncur::mvprintw(10, cursorpos[6], ent.m_password().c_str());
      ncur::mvprintw(11, cursorpos[7], ent.m_notes().c_str());
    }
    bool selected { false };
    while (true) {

      ncur::mvprintw(prev + 4, 2, " ");
      ncur::mvprintw(curr + 4, 2, ">");

      prev = curr;
      switch (ncur::getch()) {
        case 'j': curr = (curr + 1) % 8; break;
        case 'k': curr = (8 + curr - 1) % 8; break;
        case '\n': selected = true; break;
      }
      ncur::mvprintw(4, 0, "|   Save                              ");
      ncur::refresh();

      cursorloc loc;  // to handle deleting characters
      if (selected) {
        if (curr == 1)  // Back
          break;
        else if (curr == 0)  // Save
          if (is_properly_filled(ent))
            break;
          else {
            selected = false;
            ncur::mvprintw(4, 8, " : some fields are not filled!");
          }
        else {
          ncur::mvprintw(curr + 4, 1, ">");
          ncur::curs_set(1);
          switch (curr) {
            case 2: input = ent.m_title(); break;
            case 3: input = ent.m_website(); break;
            case 4: input = ent.m_username(); break;
            case 5: input = ent.m_email(); break;
            case 6: input = ent.m_password(); break;
            case 7: input = ent.m_notes(); break;
          }
          loc = cursorpos[curr] + input.size();
          ncur::move(curr + 4, loc);
        }
      }
      while (selected) {
        if (curr == 2) {  // Update heading when title field is modified
          ncur::mvprintw(
            1, 0, "|                                                          |");
          const std::string title { input.empty() ? "[ NEW ENTRY ]" : input };
          ncur::mvprintw(1, 30 - title.size() / 2, " %s ", get_upper_case(title).c_str());
          ncur::move(curr + 4, loc);
        }

        auto inp { ncur::getch() };
        if (inp == '\n') {
          ncur::curs_set(0);
          selected = false;
          switch (curr) {
            case 2: ent.m_title() = input; break;
            case 3: ent.m_website() = input; break;
            case 4: ent.m_username() = input; break;
            case 5: ent.m_email() = input; break;
            case 6: ent.m_password() = input; break;
            case 7: ent.m_notes() = input; break;
          }
          input = "";
        } else if (inp == KEY_BACKSPACE && !input.empty()) {
          ncur::move(curr + 4, --loc);
          ncur::delch();
          ncur::insch(' ');
          input.pop_back();
        } else if (is_valid_char(inp, (curr == 2 || curr == 7))) {
          ++loc;
          ncur::addch(inp);
          input.push_back(inp);
        }
      }
      ncur::mvprintw(curr + 4, 1, " ");
    }

    if (curr == 0) {  // Save option selected
      if (is_new)
        __m_core.new_entry(ent);
      else if (ent != *__m_entry) {
        ent.m_time_modified() = static_cast<unixtime>(std::time(nullptr));
        *__m_entry            = ent;
      }
    }
    return action { "Back", states::entry_view, states::db_open };
  }

  action entry_view::delete_entry(const cursorloc *cursorpos)
  {
    ncur::mvprintw(
      4, cursorpos[0], unix_to_datetime(__m_entry->m_time_created()).c_str());
    ncur::mvprintw(
      5, cursorpos[1], unix_to_datetime(__m_entry->m_time_modified()).c_str());

    ncur::mvprintw(7, cursorpos[3], __m_entry->m_website().c_str());
    ncur::mvprintw(8, cursorpos[4], __m_entry->m_username().c_str());
    ncur::mvprintw(9, cursorpos[5], __m_entry->m_email().c_str());
    ncur::mvprintw(10, cursorpos[6], __m_entry->m_password().c_str());
    ncur::mvprintw(11, cursorpos[7], __m_entry->m_notes().c_str());
    for (selector i { 3 }; i < 13; ++i) ncur::mvprintw(i, 59, "|");
    ncur::printw("\n+----------------------------------------------------------+\n");

    ncur::mvprintw(4, 44, "Delete entry?");
    ncur::mvprintw(5, 44, "Yes[y] / No[n]");
    ncur::refresh();

    while (true) switch (ncur::getch()) {
        case 'y': __m_core.delete_entry(__m_entry);
        case 'n': return action { "Back", states::entry_view, states::db_open };
      }
  }
}  // namespace pm
