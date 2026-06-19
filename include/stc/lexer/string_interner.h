/**
 * @file string_interner.h
 * @author Anis Han
 * @date 2026/6/16
 * lexer的字符串都要驻留到这里
 */

#ifndef STC_IDENT_TABLE_H
#define STC_IDENT_TABLE_H
#include <string_view>
#include <memory_resource>
#include <unordered_set>

namespace stc::lexer {

/**
 * 词素，比 view 等值比较速度更快
 */
struct Lexeme {
    const char8_t* const str;
    const size_t len;
};

/**
 * 更快的比较
 */
constexpr bool operator==(const Lexeme& a, const Lexeme& b) {
    return a.str == b.str;
}

/**
 * 返回一个空的词素
 * @return 空词素
 */
constexpr Lexeme null_lexeme() {
    return Lexeme(nullptr, 0);
}

/**
 * 字符串驻留池，保证字符串视图和当前驻留池同生命周期，驻留池失效后会所有字符串视图都会悬挂，
 * 所以一定要保证驻留池生命周期远大于所有其他使用者？或者可以放到 thread_local
 */
class StringInterner {
public:
    StringInterner() = default;

    StringInterner(const StringInterner&) = delete;

    StringInterner(StringInterner&&) = delete;

    /**
     * 将字符串加入驻留池
     * @param str 加入字符串
     * @return
     */
    Lexeme intern(std::u8string_view str);

private:
    std::unordered_set<std::u8string_view> intern_table_{};
    std::pmr::monotonic_buffer_resource arena_{};
};

inline thread_local StringInterner interner;

} // namespace stc::lexer

#endif //STC_IDENT_TABLE_H
