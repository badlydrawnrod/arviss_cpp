#include <string>

#include "arviss_cpp/arviss_cpp.hpp"

exported_class::exported_class()
    : m_name {"arviss_cpp"}
{
}

auto exported_class::name() const -> char const*
{
  return m_name.c_str();
}
