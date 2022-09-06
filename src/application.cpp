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
    if (act.m_from != act.m_to) {
      __m_prev_state = act.m_from;
      __m_curr_state = act.m_to;
    }
  }

  action application::display() { return __m_state_space[__m_curr_state]->display(); }

  application::application()
    : __m_curr_state { states::TITLE }, __m_prev_state { states::TITLE }
  {
    __m_state_space[states::TITLE]      = std::make_unique<title>();
    __m_state_space[states::SETTINGS]   = std::make_unique<settings>();
    __m_state_space[states::SELECTFILE] = std::make_unique<select_file>();
    __m_state_space[states::DBOPEN]     = std::make_unique<db_open>();
  }
}  // namespace pm
