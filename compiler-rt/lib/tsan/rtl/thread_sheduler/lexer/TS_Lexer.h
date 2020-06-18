#ifndef SANITAZER_TS_LEXER_H
#define SANITAZER_TS_LEXER_H


#include "../util/TS_Map.h"
#include "../util/TS_Queue.h"
#include "TS_State.h"
#include <regex.h>
#include <cstring>
#include "TS_Token.h"
#include <string.h>

namespace __tsan::ts::lexer {
    using namespace __tsan::ts::util;

    class TS_Lexer {
    private:
        TS_Map<TS_State, TS_Map<TS_State, const char *> *> rules;

        int tokenBufferSize = 64;

        void initRules() {
            // H:
            auto *rulesFromH = new TS_Map<TS_State , const char *>();
            rulesFromH->add(TS_State::TH, "main");
            rules.add(TS_State::H, rulesFromH);

            // TH:
            auto *rulesFromTH = new TS_Map<TS_State , const char *>();
            rulesFromTH->add(TS_State::COL, ":");
            rules.add(TS_State::TH, rulesFromTH);

            // COL:
            auto *rulesFromCOL = new TS_Map<TS_State , const char *>();
            rulesFromCOL->add(TS_State::ACT_T, "create_thread|complete_thread");
            rulesFromCOL->add(TS_State::ACT_M, "mutex_lock|mutex_unlock");
            rules.add(TS_State::COL, rulesFromCOL);

            // ACT_T:
            auto *rulesFromACT_T = new TS_Map<TS_State , const char *>();
            rulesFromACT_T->add(TS_State::ID, "[A-Za-z0-9]+");
            rules.add(TS_State::ACT_T, rulesFromACT_T);

            // ACT_M:
            auto *rulesFromACT_M = new TS_Map<TS_State , const char *>();
            rulesFromACT_M->add(TS_State::TH, "[A-Za-z0-9]+");
            rulesFromACT_M->add(TS_State::ACT_T, "create_thread|complete_thread");
            rulesFromACT_M->add(TS_State::ACT_M, "mutex_lock|mutex_unlock");
            rules.add(TS_State::ACT_M, rulesFromACT_M);

            // ID:
            auto *rulesFromID = new TS_Map<TS_State , const char *>();
            rulesFromID->add(TS_State::TH, "[A-Za-z0-9]+");
            rulesFromID->add(TS_State::ACT_T, "create_thread|complete_thread");
            rulesFromID->add(TS_State::ACT_M, "mutex_lock|mutex_unlock");
            rules.add(TS_State::ID, rulesFromID);
        }

        bool checkRegex(const char *pattern, const char *str) {
            regex_t regex;
            int reti;
            char msgbuf[100];
            bool result = false;

            /* Compile regular expression */
            reti = regcomp(&regex, pattern, REG_EXTENDED);
            if (reti) {
                // todo handle error
                regfree(&regex);
                return result;
            }

            /* Execute regular expression */
            regmatch_t pmatch[1];
            reti = regexec(&regex, str, 1, pmatch, 0);
            int len = strlen(str);
            if (!reti && pmatch[0].rm_so != -1 && pmatch[0].rm_eo - pmatch[0].rm_so == len) {
                result = true;
            } else if (reti == REG_NOMATCH) {
                result = false;
            } else {
                // todo handle error
            }
            regfree(&regex);

            return result;
        }

        TS_State findNextState(TS_State currentState, const char *token) {
            auto currentRules = rules.search(currentState);
            if (currentRules == nullptr) {
                return TS_State::UNKNOWN;
            }
            auto iterator = rules.search(currentState)->iterator();
            bool match = false;
            while (iterator.hasNext()) {
                auto rule = iterator.next();
                if (checkRegex(rule.getValue(), token)) {
                    return rule.getKey();
                }
            }

            return TS_State::UNKNOWN;
        }

        void handleError(const char *token) {
            // todo handle error
        }

    public:
        TS_Lexer() {
            initRules();
        }

        TS_Queue<TS_Token *> *scan(const char *str) {
            auto currentState = TS_State::H;
            auto *result = new TS_Queue<TS_Token *>();
            result->add(new TS_Token(TS_State::H, ""));

            char tokenBuffer[tokenBufferSize];
            int tokenBufferIndex = 0;
            for (int i = 0; i < strlen(str); i++) {
                if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t' || str[i] == ':' || str[i] == '(' || str[i] == ')') {
                    if (tokenBufferIndex > 0) {
                        tokenBuffer[tokenBufferIndex] = '\0';
                        currentState = findNextState(currentState, tokenBuffer);
                        if (currentState == TS_State::UNKNOWN) {
                            handleError(tokenBuffer);
                            return nullptr;
                        } else {
                            auto *tokenName = (char *) malloc(tokenBufferIndex + 1);
                            strcpy(tokenName, tokenBuffer);
                            result->add(new TS_Token(currentState, tokenName));
                        }
                        tokenBufferIndex = 0;
                    }
                    if (str[i] == ':') {
                        currentState = findNextState(currentState, ":");
                        if (currentState == TS_State::UNKNOWN) {
                            handleError(tokenBuffer);
                        } else {
                            result->add(new TS_Token(currentState, ":"));
                        }
                    }
                } else {
                    tokenBuffer[tokenBufferIndex] = str[i];
                    tokenBufferIndex++;
                }
            }

            return result;
        }
    };
}


#endif //SANITAZER_TS_LEXER_H
