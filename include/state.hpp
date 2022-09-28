#ifndef PASSWORD_MANAGER_INCLUDE_STATE_HPP_1662169646
#define PASSWORD_MANAGER_INCLUDE_STATE_HPP_1662169646

#include "database.hpp"

#include <filesystem>

namespace pm
{
  namespace states
  {
    enum : int { exit = -1, title, settings, select_file, db_open, entry_view, total };
  }  // namespace states

  struct action {
    std::string m_description;
    state m_from, m_to;
  };

  class screen {
  public:
    screen()          = default;
    virtual ~screen() = default;
    void display_actions(const cursorloc menu_offset) const;
    virtual action get_action(const cursorloc menu_offset = 0);

  protected:
    selector _m_choice;
    std::vector<action> _m_actions;
  };

  class title : public screen {
  public:
    title();
    action get_action(const cursorloc = 0) override;
  };

  class settings : public screen {
  public:
    settings();
    action get_action(const cursorloc = 0) override;
  };

  class select_file : public screen {
  public:
    select_file() = default;
    action get_action(const cursorloc = 0) override;

  private:
    void update_actions();
    void delete_file(const std::string &filename);

    bool __m_delete;
  };

  class db_open : public screen {
  public:
    db_open(database &core_db);
    action get_action(const cursorloc = 0) override;

  private:
    void name_new_database();
    void update_actions();
    action database_input();

    selector __m_active;
    bool __m_focus_db, __m_delete, __m_save;
    database &__m_core;

    friend class application;
  };

  class entry_view : public screen {
  public:
    using view = int;

    enum : int { VIEW, NEW, EDIT, DELETE };

    entry_view(database &core_db);
    action get_action(const cursorloc = 0) override;

  private:
    action view_entry(const cursorloc *cursorpos);
    action input_entry_details(const cursorloc *cursorpos, bool is_new);
    action delete_entry(const cursorloc *cursorpos);

    view __m_view;
    database::iterator __m_entry;
    database &__m_core;

    friend class application;
  };
}  // namespace pm

#endif
