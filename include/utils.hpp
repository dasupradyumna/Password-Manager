#ifndef WORKSPACE_JSON_INCLUDE_UTILS_HPP_1663168532
#define WORKSPACE_JSON_INCLUDE_UTILS_HPP_1663168532

#include <ctime>
#include <iomanip>
#include <string>

namespace pm
{
  // type aliases for readability
  using cursorloc = int;
  using unixtime  = unsigned;
  using selector  = int;
  using state     = int;

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
