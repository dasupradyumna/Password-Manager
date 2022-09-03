#ifndef PASSWORD_MANAGER_INCLUDE_STATE_HPP_1662169646
#define PASSWORD_MANAGER_INCLUDE_STATE_HPP_1662169646

#include <string>
#include <vector>

namespace pm
{
  using state = int;

  namespace states
  {
    enum : int { EXIT = -1, TITLE, SETTINGS, SELECTFILE, DBOPEN, TOTAL };
  }  // namespace states

  struct action {
    std::string m_description;
    state m_from, m_to;
  };

  class screen {
  public:
    virtual ~screen() = default;
    virtual action display();

  protected:
    std::size_t _m_choice;
    std::vector<action> _m_actions;
  };

  class title : public screen {
  public:
    title();
    action display() override;
  };

  class settings : public screen {
  public:
    settings();
    action display() override;
  };

  class select_file : public screen {
  public:
    select_file();
    action display() override;
  };

  class db_open : public screen {
  public:
    db_open();
    action display() override;
  };
}  // namespace pm

#endif
