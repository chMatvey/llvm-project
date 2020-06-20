//
// Created by matvey on 6/20/20.
//

#ifndef SANITAZER_TS_STRINGCOMPARATOR_H
#define SANITAZER_TS_STRINGCOMPARATOR_H


#include "../util/TS_Comparator.h"

#include <cstring>

using namespace __tsan::ts::util;

namespace __tsan::ts::parser {

    class TS_StringComparator : public TS_Comparator<const char *> {
    private:
        int compare(const char *a, const char *b) override {
            return strcmp(a, b);
        }
    };
}


#endif //SANITAZER_TS_STRINGCOMPARATOR_H
