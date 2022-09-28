#ifndef PASSWORD_MANAGER_INCLUDE_FILE_HPP_1664285428
#define PASSWORD_MANAGER_INCLUDE_FILE_HPP_1664285428

#include "database.hpp"

#include <fstream>

namespace pm
{
  class file_handler {
  public:
    void set_filepath(const std::string &filepath);
    void load(database &core_db);
    void save(const database &core_db);

  private:
    std::string __m_filepath;
    std::fstream __m_savefile;
  };
}  // namespace pm

#endif
