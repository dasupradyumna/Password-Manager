#ifndef WORKSPACE_JSON_INCLUDE_DATABASE_HPP_1663138405
#define WORKSPACE_JSON_INCLUDE_DATABASE_HPP_1663138405

#include "utils.hpp"

#include <memory>
#include <vector>

namespace pm
{
  class entry {  // 4*2+32*6 = 200B per instance
  public:
    // ACCESS methods for all (except m_time_created - only getter but not setter)
    // Corresponding methods for const instances of the class as well

    unixtime m_time_created() const { return __m_time_created; }

    unixtime &m_time_modified() { return __m_time_modified; }

    const unixtime &m_time_modified() const { return __m_time_modified; }

    std::string &m_title() { return __m_title; }

    const std::string &m_title() const { return __m_title; }

    std::string &m_website() { return __m_website; }

    const std::string &m_website() const { return __m_website; }

    std::string &m_username() { return __m_username; }

    const std::string &m_username() const { return __m_username; }

    std::string &m_email() { return __m_email; }

    const std::string &m_email() const { return __m_email; }

    std::string &m_password() { return __m_password; }

    const std::string &m_password() const { return __m_password; }

    std::string &m_notes() { return __m_notes; }

    const std::string &m_notes() const { return __m_notes; }

    entry(const unixtime t) : __m_time_created { t }, __m_time_modified { t } { }

  private:
    unixtime __m_time_created;
    unixtime __m_time_modified;
    std::string __m_title, __m_website;
    std::string __m_username, __m_email, __m_password;
    std::string __m_notes;
  };

  class database {
  public:
    using iterator = std::vector<entry>::iterator;
    database()     = default;
    ~database()    = default;

    iterator begin() { return __m_data.begin(); }

    iterator end() { return __m_data.end(); }

    bool empty() const { return !__m_data.size(); }

    const selector size() const { return __m_data.size(); }

    void new_entry(const entry &ent);

    void delete_entry(const iterator &del) { __m_data.erase(del); }

  private:
    std::vector<entry> __m_data;
  };

  void reload_database(database &target);
}  // namespace pm

#endif
