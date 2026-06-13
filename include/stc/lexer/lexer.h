//
// Created by anishan on 2026/6/12.
//

#ifndef STC_LEXER_H
#define STC_LEXER_H
#include "token.h"
//#include "stc/source/location.h"
#include "source/location.h"

namespace stc::lexer {

class Lexer {

public:
    Token next_token();

private:

};

}

#endif //STC_LEXER_H
