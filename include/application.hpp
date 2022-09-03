#ifndef PASSWORD_MANAGER_INCLUDE_APPLICATION_HPP_1662144704
#define PASSWORD_MANAGER_INCLUDE_APPLICATION_HPP_1662144704

#include "state.hpp"

#include <memory>

namespace pm
{
  class application {
  public:
    static application &instance();

    state get_state() const;
    void update(const action &act);
    action display();

  private:
    application();

    state __m_curr_state, __m_prev_state;
    std::unique_ptr<screen> __m_state_space[states::TOTAL];
  };
}  // namespace pm

#endif
