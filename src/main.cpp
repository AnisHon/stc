#include <stc/utils/macros.h>

#include <iostream>
#include <fmt/format.h>

#include "stc/source/manager.h"
#include "stc/utils/version.h"

int main() {

    std::cout << "Hello World, version: " << stc::get_version() << '\n';

    //ASSERT(1 == 2, "shoule be");
    return 0;
}

