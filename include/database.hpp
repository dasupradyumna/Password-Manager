#ifndef WORKSPACE_JSON_INCLUDE_DATABASE_HPP_1663138405
#define WORKSPACE_JSON_INCLUDE_DATABASE_HPP_1663138405

#include "utils.hpp"

#include <memory>
#include <string>
#include <vector>

namespace pm
{
  struct entry {  // 8*2+32*6 = 208B per instance
    const std::time_t m_time_created;
    std::time_t m_time_modified;
    std::string m_title, m_website;
    std::string m_username, m_email, m_password;
    std::string m_notes;

    entry(const std::time_t t) : m_time_created { t }, m_time_modified { t } { }
  };

  class database {
  public:
    using iterator = std::vector<std::unique_ptr<entry>>::const_iterator;
    database()     = default;
    ~database()    = default;

    iterator begin() const { return __m_data.begin(); }

    iterator end() const { return __m_data.end(); }

    bool empty() const { return !__m_data.size(); }

    std::size_t size() const { return __m_data.size(); }

    const entry &operator[](const std::size_t idx) const { return *__m_data[idx]; }

    void new_entry(const entry &ent);
    void delete_entry(const iterator &del);

  private:
    std::vector<std::unique_ptr<entry>> __m_data;
  };

  void reload_database(database &target);
}  // namespace pm

#endif
