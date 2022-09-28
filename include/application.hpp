#ifndef PASSWORD_MANAGER_INCLUDE_APPLICATION_HPP_1662144704
#define PASSWORD_MANAGER_INCLUDE_APPLICATION_HPP_1662144704

#include "file.hpp"
#include "state.hpp"

namespace pm
{
  class application {
  public:
    static application &instance();

    state get_state() const;
    void update(const action &act);
    action get_action();

  private:
    application();

    state __m_curr_state, __m_prev_state;
    std::unique_ptr<screen> __m_state_space[states::total];
    database __m_core;
    file_handler __m_file_handler;
  };
}  // namespace pm

#endif
