#include "fmt/base.h"
#include "stcc/version.h"
#include "stcc/utils/macros.h"
#include <fmt/format.h>

int main() {
    fmt::println("Hello World! version: {}", stcc::get_version());

    // ASSERT(1 == 2, "shoule be");

    return 0;
}