#include <string>

#include "arviss_cpp/arviss_cpp.hpp"

auto main() -> int
{
  auto const exported = exported_class {};

  return std::string("arviss_cpp") == exported.name() ? 0 : 1;
}
