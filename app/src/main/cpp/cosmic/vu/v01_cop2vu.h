#pragma once

#include <common/types.h>
#include <vu/vecu.h>
namespace cosmic::vu {
    // Just a communication interface between these two VUs
    class MacroModeCop2 {
    public:
        MacroModeCop2(RawReference<vu::VectorUnit> vus[2]);
        void clearInterlock();
        bool checkInterlock();
        bool interlockCheck(bool isCop2);

        u32 cfc2(u32 special);
        void ctc2(u32 special, u32 value);

        RawReference<vu::VectorUnit> v0;
        RawReference<vu::VectorUnit> v1;
        bool cop2il,
            vuIl;
    };
}

