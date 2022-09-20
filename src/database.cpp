#include "database.hpp"

namespace pm
{
  void database::new_entry(const entry &ent)
  {
    __m_data.push_back(std::make_unique<entry>(ent));
    for (long it { static_cast<long>(__m_data.size()) - 1 }; --it >= 0;)
      if (__m_data[it]->m_title > __m_data[it + 1]->m_title)
        std::swap(__m_data[it], __m_data[it + 1]);
  }

  void database::delete_entry(const iterator &del) { __m_data.erase(del); }

  void reload_database(database &target) { }
}  // namespace pm
