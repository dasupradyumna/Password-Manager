#ifndef WORKSPACE_JSON_INCLUDE_UTILS_HPP_1663168532
#define WORKSPACE_JSON_INCLUDE_UTILS_HPP_1663168532

#include <ctime>
#include <iomanip>
#include <string>

// preprocessor substitution functions
#define GETTER_SETTER(ret, attr)  \
 ret &attr() { return __##attr; } \
 const ret &attr() const { return __##attr; }

#define DOWNCAST_SCREEN(screen) (dynamic_cast<screen &>(*__m_state_space[states::screen]))

namespace pm
{
  // type aliases for readability
  using cursorloc = int;
  using unixtime  = unsigned;
  using selector  = int;
  using state     = int;

  // global variables
  inline constexpr std::string_view g_save_dir { "pmsav" };
  extern std::string g_db_name;

  inline unixtime string_to_unixtime(const std::string &num) noexcept
  {
    return std::stoul(num);
  }

  inline std::string unix_to_datetime(const std::time_t timepoint)
  {
    std::stringstream out;
    out << std::put_time(std::localtime(&timepoint), "%e %b %Y, %T");
    return out.str();
  }

  inline std::string get_upper_case(const std::string &input)
  {
    std::string result {};
    for (const char c : input) result += std::toupper(c);
    return result;
  }
}  // namespace pm

#endif
