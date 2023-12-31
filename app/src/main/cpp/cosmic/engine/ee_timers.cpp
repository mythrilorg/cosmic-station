#include <common/except.h>
#include <engine/ee_timers.h>

#include <vm/sched_logical.h>
namespace cosmic::engine {
    EeTimers::EeTimers() {}

    void EeTimers::resetTimers() {
        if (!clockWake)
            throw TimerFail("There is no valid schedule available");

        for (u8 tiEn{}; tiEn != timers.size(); tiEn++) {
            timers[tiEn].clocks = 0;
            timers[tiEn].isEnabled = false;

            clockWake->postMakeTimer(0xffff, tiEn, [this](u8 position) {
                timerReached(position);
            });
        }
    }
    void EeTimers::timerReached(u8 raised) {}
}


