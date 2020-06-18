#ifndef SANITAZER_TS_TOKEN_H
#define SANITAZER_TS_TOKEN_H


#include "TS_State.h"

namespace __tsan::ts::lexer {
    class TS_Token {
    public:
        TS_State state;
        const char *value;

        TS_Token(TS_State state, const char *value) {
            this->state = state;
            this->value = value;
        }
    };
}


#endif //SANITAZER_TS_TOKEN_H
