#ifndef STCC_TEST_LEXER_HELPER_H

#  include <string>
#  include <vector>
#  include "stcc/lexer/token_kinds.h"

std::vector<stcc::PPTokenKind> code2pptoken(std::string code);

#endif // STCC_TEST_LEXER_HELPER_H
