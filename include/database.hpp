#ifndef WORKSPACE_JSON_INCLUDE_DATABASE_HPP_1663138405
#define WORKSPACE_JSON_INCLUDE_DATABASE_HPP_1663138405

#include "utils.hpp"

#include <memory>
#include <vector>

namespace pm
{
  class entry {  // 4*2+32*6 = 200B per instance
  public:
    entry(const unixtime t) : __m_time_created { t }, __m_time_modified { t } { }

    // ACCESS methods for all (except m_time_created - only getter but not setter)
    // Corresponding methods for const instances of the class as well
    unixtime m_time_created() const { return __m_time_created; }
    GETTER_SETTER(unixtime, m_time_modified)
    GETTER_SETTER(std::string, m_title)
    GETTER_SETTER(std::string, m_website)
    GETTER_SETTER(std::string, m_username)
    GETTER_SETTER(std::string, m_email)
    GETTER_SETTER(std::string, m_password)
    GETTER_SETTER(std::string, m_notes)

    bool operator==(const entry &other) const
    {
      return __m_title == other.__m_title && __m_website == other.__m_website
          && __m_username == other.__m_username && __m_email == other.__m_email
          && __m_password == other.__m_password && __m_notes == other.__m_notes;
    }

    bool operator!=(const entry &other) const { return !(*this == other); }

  private:
    unixtime __m_time_created;
    unixtime __m_time_modified;
    std::string __m_title, __m_website;
    std::string __m_username, __m_email, __m_password;
    std::string __m_notes;
  };

  class database {
  public:
    using iterator       = std::vector<entry>::iterator;
    using const_iterator = std::vector<entry>::const_iterator;
    database()           = default;
    ~database()          = default;

    iterator begin() noexcept { return __m_data.begin(); }

    const_iterator begin() const noexcept { return __m_data.cbegin(); }

    iterator end() noexcept { return __m_data.end(); }

    const_iterator end() const noexcept { return __m_data.cend(); }

    bool empty() const noexcept { return !__m_data.size(); }

    const selector size() const noexcept { return __m_data.size(); }

    entry &operator[](const selector idx) noexcept { return __m_data[idx]; }

    void clear() { __m_data.clear(); }

    void new_entry(const entry &ent);

    void delete_entry(const iterator &del) { __m_data.erase(del); }

  private:
    std::vector<entry> __m_data;
  };
}  // namespace pm

#endif
