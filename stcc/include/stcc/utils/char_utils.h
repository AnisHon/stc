/**
 * @file char_utils.h
 * Safe wrappers for the <cctype> character helpers.
 */

#ifndef STCC_CHAR_UTILS_H
#define STCC_CHAR_UTILS_H

#include <cctype>

namespace stcc::utils {

// The <cctype> functions require EOF or a value representable as unsigned char.
// Converting char first avoids undefined behavior for bytes above ASCII when
// char is signed.
inline bool is_space(const char ch) { return std::isspace(static_cast<unsigned char>(ch)); }
inline bool is_alpha(const char ch) { return std::isalpha(static_cast<unsigned char>(ch)); }
inline bool is_digit(const char ch) { return std::isdigit(static_cast<unsigned char>(ch)); }
inline bool is_alnum(const char ch) { return std::isalnum(static_cast<unsigned char>(ch)); }
inline bool is_xdigit(const char ch) { return std::isxdigit(static_cast<unsigned char>(ch)); }
inline bool is_upper(const char ch) { return std::isupper(static_cast<unsigned char>(ch)); }
inline bool is_lower(const char ch) { return std::islower(static_cast<unsigned char>(ch)); }
inline bool is_punct(const char ch) { return std::ispunct(static_cast<unsigned char>(ch)); }
inline bool is_control(const char ch) { return std::iscntrl(static_cast<unsigned char>(ch)); }

inline char to_lower(const char ch) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

inline char to_upper(const char ch) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
}

} // namespace stcc::utils

#endif // STCC_CHAR_UTILS_H