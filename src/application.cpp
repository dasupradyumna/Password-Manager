#include "application.hpp"

namespace pm
{
  application &application::instance()
  {
    static application core;
    return core;
  }

  state application::get_state() const { return __m_curr_state; }

  void application::update(const action &act)
  {
    namespace fs = std::filesystem;

    if (act.m_from != act.m_to) {
      __m_prev_state = act.m_from;
      __m_curr_state = act.m_to;
    }

    // TODO very messy code, change design
    const std::string choice { act.m_description };
    switch (act.m_to) {
      case states::db_open:
        // if choice is empty, user is just switching between columns in DBOPEN
        // if choice is "Back" or "Delete" or "Save" or "No", do not reload the database
        // if choice is "Yes" (save mode), save the current database to file
        if (choice.empty() || choice == "Back" || choice == "Delete" || choice == "Save"
            || choice == "No")
          break;
        else if (choice == "Yes") {
          __m_file_handler.set_filepath(fs::path { g_save_dir } / g_db_name);
          __m_file_handler.save(__m_core);
          break;
        }
        g_db_name =
          choice == "New File" ? "[ UNNAMED ]" : choice.substr(choice.find('.') + 2);
        __m_file_handler.set_filepath(fs::path { g_save_dir } / g_db_name);
        __m_file_handler.load(__m_core);
        break;
      case states::entry_view: {
        auto &view { DOWNCAST_SCREEN(entry_view) };
        if (DOWNCAST_SCREEN(db_open).__m_delete) {  // Delete from "db_open"
          view.__m_view  = entry_view::DELETE;
          view.__m_entry = __m_core.begin() + std::stoi(choice);
        } else {  // Delete from "entry_view::VIEW" and New option/ENTER from "db_open"
          view.__m_view = choice == "New"
                          ? entry_view::NEW
                          : (choice == "Delete" ? entry_view::DELETE
                                                : (choice == "Edit" ? entry_view::EDIT
                                                                    : entry_view::VIEW));
          if (view.__m_view == entry_view::NEW)
            view.__m_entry = __m_core.end();
          else if (view.__m_view == entry_view::VIEW)
            view.__m_entry = __m_core.begin() + std::stoi(choice);
        }
      } break;
    }
  }

  action application::get_action()
  {
    return __m_state_space[__m_curr_state]->get_action();
  }

  application::application()
    : __m_curr_state { states::title },
      __m_prev_state { states::title },
      __m_file_handler {},
      __m_core {}
  {
    __m_state_space[states::title]       = std::make_unique<title>();
    __m_state_space[states::settings]    = std::make_unique<settings>();
    __m_state_space[states::select_file] = std::make_unique<select_file>();
    __m_state_space[states::db_open]     = std::make_unique<db_open>(__m_core);
    __m_state_space[states::entry_view]  = std::make_unique<entry_view>(__m_core);
  }
}  // namespace pm
