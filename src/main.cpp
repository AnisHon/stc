#include <iostream>
#include "stc/source/manager.h"

int main() {
    std::cout << "Hello World" << std::endl;

    stc::source::Manager manager;
    manager.get_spelling_loc();
    return 0;
}