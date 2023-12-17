#include "arviss_cpp/arviss_cpp.hpp"

auto main() -> int
{
  auto const result = name();

  return result == "arviss_cpp" ? 0 : 1;
}
