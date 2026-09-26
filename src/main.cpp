#include <stc/utils/macros.h>

#include "fmt/base.h"
#include "stc/version.h"
#include <fmt/format.h>

int main() {

  fmt::println("Hello World! version: {}", stc::get_version());

  // ASSERT(1 == 2, "shoule be");

  return 0;
}
