#pragma once
#include <vector>

#include <common/types.h>
namespace cosmic::vm {
    using TimerInvokable = std::function<void(u8)>;
    enum AffinityControl {
        EmotionEngine = 0x3,
        GS = 0x6,
        VUs = 0x7
    };
    enum AffinityModels {
        Normal,
        PrioritizeVectors,
        GraphicsFirst,
    };
    struct TimerTask {
        i64 lastUpdate;
    };
    struct TimerEvent {
        TimerTask timer;
        TimerInvokable callback;
        i64 runAt;
        bool isActivated{true};
    };

    class Scheduler {
    public:
        struct MachineCycles {
            u32 highClock;
            u64 remain;
            u32 cycles;
        };
        enum VirtDeviceLTimer : u8 {
            Mips,
            IOP,
            Bus
        };

        Scheduler();
        void resetCycles();

        u32 getNextCycles(VirtDeviceLTimer high0);
        void updateCyclesCount();

        void postMakeTimer(u32 ofMask, u8 elPos, TimerInvokable invoke);
        void runEvents();

        u32 affinity{};
    private:
        MachineCycles eeCycles,
            busCycles,
            iopCycles;
        i64 nextEventCycle;

        std::vector<TimerEvent> events;
    };
}
