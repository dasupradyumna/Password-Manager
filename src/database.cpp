#include "database.hpp"

namespace pm
{
  void database::new_entry(const entry &ent)
  {
    __m_data.push_back(ent);
    for (auto it { static_cast<selector>(__m_data.size()) - 1 }; --it >= 0;)
      if (__m_data[it].m_title() > __m_data[it + 1].m_title())
        std::swap(__m_data[it], __m_data[it + 1]);
  }

  void reload_database(database &target) { }
}  // namespace pm
