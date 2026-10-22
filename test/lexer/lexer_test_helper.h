#ifndef STC_TEST_LEXER_HELPER_H

#  include <string>
#  include <vector>
#  include "stc/lexer/token_kinds.h"

std::vector<stc::PPTokenKind> code2pptoken(std::string code);

#endif // STC_TEST_LEXER_HELPER_H
