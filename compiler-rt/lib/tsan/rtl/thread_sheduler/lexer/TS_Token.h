#ifndef SANITAZER_TS_TOKEN_H
#define SANITAZER_TS_TOKEN_H


#include "TS_State.h"

namespace __tsan::ts::lexer {
    struct TS_Token {
        TS_State state;
        const char *value;
    };
}


#endif //SANITAZER_TS_TOKEN_H
