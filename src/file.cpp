#include "file.hpp"

namespace pm
{
  void file_handler::set_filepath(const std::string &filepath)
  {
    __m_filepath = filepath;
  }

  void file_handler::load(database &core_db)
  {
    core_db.clear();
    int field_count { 0 };
    std::string line {};
    entry *current;
    __m_savefile.open(__m_filepath, std::ios::in);

    if (__m_savefile.is_open())
      while (std::getline(__m_savefile, line)) {
        switch (field_count) {
          case 0: current = new entry { string_to_unixtime(line) }; break;
          case 1: current->m_time_modified() = string_to_unixtime(line); break;
          case 2: current->m_title() = line; break;
          case 3: current->m_website() = line; break;
          case 4: current->m_username() = line; break;
          case 5: current->m_email() = line; break;
          case 6: current->m_password() = line; break;
          case 7: current->m_notes() = line; break;
          case 8:
            core_db.new_entry(*current);
            delete current;
            field_count = -1;
            break;
        }
        ++field_count;
      }

    if (field_count != 0) delete current;
    __m_savefile.close();
  }

  void file_handler::save(const database &core_db)
  {
    __m_savefile.open(__m_filepath, std::ios::out);

    if (__m_savefile.is_open()) {
      for (const auto &ent : core_db) {
        __m_savefile << ent.m_time_created() << '\n';
        __m_savefile << ent.m_time_modified() << '\n';
        __m_savefile << ent.m_title() << '\n';
        __m_savefile << ent.m_website() << '\n';
        __m_savefile << ent.m_username() << '\n';
        __m_savefile << ent.m_email() << '\n';
        __m_savefile << ent.m_password() << '\n';
        __m_savefile << ent.m_notes() << '\n';
        __m_savefile << "---\n";
      }
    }

    __m_savefile.close();
  }
}  // namespace pm
