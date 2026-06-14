#include <iostream>
#include <fmt/format.h>

#include "stc/source/manager.h"
#include "stc/utils/version.h"

int main() {

    std::cout << "Hello World, version: " << stc::get_version() << '\n';

    stc::source::Manager manager;
    manager.get_spelling_loc();
    return 0;
}

