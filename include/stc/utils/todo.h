//
// Created by anishan on 2026/6/13.
//

#ifndef STC_TODO_H
#define STC_TODO_H
#include <stdexcept>
#include <fmt/core.h>

#define TODO(msg, ...) \
    throw std::runtime_error( \
        fmt::format(msg " unimplement at {}:{}", \
        ##__VA_ARGS__, __FILE__, __LINE__))

namespace stc::utils {

class UnimplementedException : public std::runtime_error {
public:
    explicit UnimplementedException(const char* what) : std::runtime_error(what) {
    }
};

}


#endif //STC_TODO_H
