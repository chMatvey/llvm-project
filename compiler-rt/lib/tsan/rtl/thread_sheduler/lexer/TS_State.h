#ifndef SANITAZER_TS_MACHINESTATE_H
#define SANITAZER_TS_MACHINESTATE_H


namespace __tsan::ts::lexer {

    enum class TS_State {
        H,          // begin state;
        TH,         // Thread id state;
        COL,        // Active thread detection state;
        ACT_T,      // Interaction with thread state;
        ACT_M,      // Interaction with mutex state;
        ID,         // Id state;
        UNKNOWN     // Error state.
    };
}


#endif //SANITAZER_TS_MACHINESTATE_H
