/**
 * @file string_interner.cpp
 * @author Anis Han
 * @date 2026/6/16
 */

#include "stc/lexer/string_interner.h"


namespace stc::lexer {

const char8_t* StringInterner::intern(const std::u8string_view str) {
    // 如果找到，返回池内的字符串，不能信任用户传进来的 string_view 所以这里进行两次哈希
    if (const auto it = this->intern_table_.find(str); it != intern_table_.end()) {
        return it->data();
    }
    // 没找到，创建，多申请一个字节放 NULL 字符
    const auto block = static_cast<char8_t*>(
        this->arena_.allocate(str.size() + 1, alignof(char8_t))
    );

    // 初始化字符串
    std::memcpy(block, str.data(), str.size());
    const std::u8string_view view(block, str.size());
    block[str.size()] = '\0'; // 伪装成C字符串，好处远大于坏处

    this->intern_table_.insert(view);

    return block;
}

}