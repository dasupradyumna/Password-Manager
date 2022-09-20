#include "application.hpp"

#define DOWNCAST(cls, flag) (dynamic_cast<cls &>(*__m_state_space[flag]))

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
    if (act.m_from != act.m_to) {
      __m_prev_state = act.m_from;
      __m_curr_state = act.m_to;
    }

    const std::string choice { act.m_description };
    switch (act.m_to) {
      case states::DBOPEN: {
        // if choice is empty, user is just switching between columns in DBOPEN
        // if choice is "Back" or "Delete", do not update and reload the database
        if (choice.empty() || choice == "Back" || choice == "Delete") break;
        g_db_name =
          choice == "New File" ? "[ UNNAMED ]" : choice.substr(choice.find('.') + 2);
        reload_database(*DOWNCAST(db_open, states::DBOPEN).__m_core);
        break;
      }
      case states::ENTRYVIEW: {
        auto &view { DOWNCAST(entry_view, states::ENTRYVIEW) };
        if (DOWNCAST(db_open, states::DBOPEN).__m_delete) {  // Delete from "db_open"
          view.__m_view  = entry_view::DELETE;
          view.__m_entry = view.__m_core->begin() + std::stoi(choice);
        } else {  // Delete from "entry_view::VIEW" and New option/ENTER from "db_open"
          view.__m_view = choice == "New"
                          ? entry_view::NEW
                          : (choice == "Delete" ? entry_view::DELETE
                                                : (choice == "Edit" ? entry_view::EDIT
                                                                    : entry_view::VIEW));
          if (view.__m_view == entry_view::NEW)
            view.__m_entry = view.__m_core->end();
          else if (view.__m_view == entry_view::VIEW)
            view.__m_entry = view.__m_core->begin() + std::stoi(choice);
        }
        break;
      }
    }
  }

  action application::get_action()
  {
    return __m_state_space[__m_curr_state]->get_action();
  }

  application::application()
    : __m_curr_state { states::TITLE }, __m_prev_state { states::TITLE }
  {
    __m_state_space[states::TITLE]      = std::make_unique<title>();
    __m_state_space[states::SETTINGS]   = std::make_unique<settings>();
    __m_state_space[states::SELECTFILE] = std::make_unique<select_file>();
    __m_state_space[states::DBOPEN]     = std::make_unique<db_open>();
    //////////// XXX TEST CASES
    entry ent { std::time(nullptr) };
    ent.m_title    = "Test2";
    ent.m_website  = "https://tester.com";
    ent.m_username = "testerman";
    ent.m_email    = "testerman@tester.com";
    ent.m_password = "Bullshit";
    ent.m_notes    = "Because I'm lazy ...";
    DOWNCAST(db_open, states::DBOPEN).__m_core->new_entry(ent);
    ent.m_title    = "Test";
    ent.m_website  = "https://tester.com";
    ent.m_username = "testerman";
    ent.m_email    = "testerman@tester.com";
    ent.m_password = "Bullshit";
    ent.m_notes    = "Because I'm lazy ...";
    DOWNCAST(db_open, states::DBOPEN).__m_core->new_entry(ent);
    ////////////
    __m_state_space[states::ENTRYVIEW] =
      std::make_unique<entry_view>(DOWNCAST(db_open, states::DBOPEN).__m_core);
  }
}  // namespace pm
