#ifndef SANITAZER_TS_PARSER_H
#define SANITAZER_TS_PARSER_H


#include "../util/TS_Queue.h"
#include "../util/TS_Map.h"
#include "../TS_Point.h"
#include "../lexer/TS_Token.h"
#include "TS_StringComparator.h"

#include <cstring>

using namespace __tsan::ts::util;
using namespace __tsan;
using namespace __tsan::ts::lexer;

namespace __tsan::ts::parser {
    class TS_Parser {
    private:
        int *currentThreadIndex = nullptr;
        int newThreadIndex = 1;
        TS_Map<const char *, int *> threads;

    public:
        TS_Parser() {
            auto *comparator = new TS_StringComparator();
            threads.setComparator(comparator);
        }

    private:
        void handleError() {
            // todo handle error
        }

        void H(TS_Queue<TS_Token> *tokens, TS_Queue<TS_Point *> * result) {
            if (tokens->getCurrent().state != TS_State::H) {
                handleError();
                return;
            }

            int mainTreadIndex = 0;
            threads.add("main", &mainTreadIndex);

            tokens->next();
            if (tokens->isEmpty()) {
                return;
            }
            switch (tokens->getCurrent().state) {
                case TS_State::TH: {
                    TH(tokens, result);
                    break;
                }
                default: {
                    handleError();
                    break;
                }
            }
        }

        void TH(TS_Queue<TS_Token> *tokens, TS_Queue<TS_Point *> *result) {
            currentThreadIndex = threads.search(tokens->getCurrent().value);
            if (currentThreadIndex == nullptr) {
                handleError();
                return;
            }
            if (!result->isEmpty()) {
                result->getLast()->nextTid = *currentThreadIndex;
            }

            tokens->next();
            if (tokens->isEmpty()) {
                return;
            }
            switch (tokens->getCurrent().state) {
                case TS_State::COL: {
                    COL(tokens, result);
                    break;
                }
                default: {
                    handleError();
                    break;
                }
            }
        }

        void COL(TS_Queue<TS_Token> *tokens, TS_Queue<TS_Point *> *result) {
            tokens->next();
            if (tokens->isEmpty()) {
                return;
            }
            switch (tokens->getCurrent().state) {
                case TS_State::ACT_T: {
                    ACT_T(tokens, result);
                    break;
                }
                case TS_State::ACT_M: {
                    ACT_M(tokens, result);
                    break;
                }
                default: {
                    handleError();
                    break;
                }
            }
        }

        void ACT_T(TS_Queue<TS_Token> *tokens, TS_Queue<TS_Point *> *result) {
            if (strcmp(tokens->getCurrent().value, "create_thread") == 0) {
                result->add(new TS_Point(TS_Action::CREATE_THREAD, *currentThreadIndex, *currentThreadIndex));
            } else if (strcmp(tokens->getCurrent().value, "complete_thread") == 0) {
                result->add(new TS_Point(TS_Action::COMPLETE_THREAD, *currentThreadIndex, *currentThreadIndex));
            } else {
                handleError();
                return;
            }

            tokens->next();
            if (tokens->isEmpty()) {
                return;
            }
            switch (tokens->getCurrent().state) {
                case TS_State::ID: {
                    ID(tokens, result);
                    break;
                }
                default: {
                    handleError();
                    break;
                }
            }
        }

        void ACT_M(TS_Queue<TS_Token> *tokens, TS_Queue<TS_Point *> *result) {
            if (strcmp(tokens->getCurrent().value, "mutex_lock") == 0) {
                result->add(new TS_Point(TS_Action::LOCK_MUTEX, *currentThreadIndex, *currentThreadIndex));
            } else if (strcmp(tokens->getCurrent().value, "mutex_unlock") == 0) {
                result->add(new TS_Point(TS_Action::UNLOCK_MUTEX, *currentThreadIndex, *currentThreadIndex));
            } else {
                handleError();
                return;
            }

            tokens->next();
            if (tokens->isEmpty()) {
                return;
            }
            switch (tokens->getCurrent().state) {
                case TS_State::TH: {
                    TH(tokens, result);
                    break;
                }
                case TS_State::ACT_T: {
                    ACT_T(tokens, result);
                    break;
                }
                case TS_State::ACT_M: {
                    ACT_M(tokens, result);
                    break;
                }
                default: {
                    handleError();
                    break;
                }
            }
        }

        void ID(TS_Queue<TS_Token> *tokens, TS_Queue<TS_Point *> *result) {
            if (result->getLast()->action == TS_Action::CREATE_THREAD) {
                int index = newThreadIndex;
                newThreadIndex++;
                threads.add(tokens->getCurrent().value, &index);
            } else {
                threads.remove(tokens->getCurrent().value);
            }

            tokens->next();
            if (tokens->isEmpty()) {
                return;
            }
            switch (tokens->getCurrent().state) {
                case TS_State::TH: {
                    TH(tokens, result);
                    break;
                }
                case TS_State::ACT_T: {
                    ACT_T(tokens, result);
                    break;
                }
                case TS_State::ACT_M: {
                    ACT_M(tokens, result);
                    break;
                }
                default: {
                    handleError();
                    break;
                }
            }
        }

    public:
        TS_Queue<TS_Point *> *parse(TS_Queue<TS_Token> *tokens) {
            if (tokens == nullptr || tokens->isEmpty()) {
                return nullptr;
            }

            auto *result = new TS_Queue<TS_Point *>();
            H(tokens, result);
            result->getLast()->nextTid = -1;
            return result;
        }
    };
}


#endif //SANITAZER_TS_PARSER_H
