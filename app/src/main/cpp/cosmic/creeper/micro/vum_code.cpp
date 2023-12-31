#include <creeper/micro/vum_code.h>
#include <vu/vecu.h>
namespace cosmic::creeper::micro {
    u32 VuMicroInterpreter::executeCode() {
        VuMicroOperands ops[2];
        const auto [upper, lower] = fetchPcInst();

        u32 micro[2];
        micro[0] = upper;
        micro[1] = lower;
        // Check if we need to interlock the Q register until a previous FDIV instruction calculation result is finished
        // FDIV, SQRT
        if (((micro[1] & 0x800007fc) == 0x800003bc))
            waitq(ops[1]);
        if ((micro[1] & (static_cast<u32>(1 << 31))) && ((micro[1] >> 2) & 0x1fc) == 0x1fc)
            waitp(ops[1]);

        ops[0] = translateUpper(micro[0]);
        if (!(micro[0] & 0b1000000))
            ops[1] = translateLower(micro[1]);

        return {};
    }

    void VuMicroInterpreter::setCurrentProgram(u32 crc) {
    }

    std::pair<u32, u32> VuMicroInterpreter::fetchPcInst() {
        u32 u, l;
        l = vuMicro->fetchByPc();
        u = vuMicro->fetchByPc();
        return std::make_pair(u, l);
    }
    void VuMicroInterpreter::waitp(VuMicroOperands& ops) {
        if (!vuMicro->status.efu.isStarted)
            return;
        vuMicro->finishStallPipeTask(false);
    }
    void VuMicroInterpreter::waitq(VuMicroOperands& ops) {
        auto& div{vuMicro->status.div};
        if (!div.isStarted)
            return;
        vuMicro->finishStallPipeTask(true);
    }

    VuMicroOperands VuMicroInterpreter::translateUpper(u32 upper) {
        u8 decMi[1];
        decMi[0] = upper & 0x3f;
        VuMicroOperands ops{upper};
        ordered.fr.write[0] = ops.dest;
        ordered.fr.writeField[0] = ordered.fr.read0Field[0] = ops.dest;

        ordered.fr.read0[0] = ops.fs;
        ordered.fr.read1[0] = ops.ft;
        ordered.fr.read1Field[0] = static_cast<u8>(1 << (0x3 - (upper & 0x3)));

        switch (decMi[0]) {
        case 0x1d: ordered.upper = [&](VuMicroOperands& ops) { maxi(ops); }; break;
        }
        return ops;
    }
    VuMicroOperands VuMicroInterpreter::translateLower(u32 lower) {
        if (lower & (static_cast<u32>(1 << 31))) {
            return translateLower1(lower);
        } else {
            return translateLower2(lower);
        }
    }

    void VuMicroInterpreter::maxi(VuMicroOperands& ops) {
    }
    VuMicroOperands VuMicroInterpreter::translateLower1(u32 lower) {
        VuMicroOperands intOps{lower};
        ordered.ir.write = (lower >> 0x6) & 0xf;
        ordered.ir.read0 = ordered.ir.read1 = intOps.fs & 0xf;
        std::array<u32, 2> d2opc{lower & 0x3f, 0};

        switch (d2opc[0]) {
        case 0x32: ordered.lower = [&](VuMicroOperands& ops) { iddai(ops); }; break;
        case 0x3f:
            d2opc[1] = (lower & 0x3) | ((lower >> 4) & 0x7c);
            switch (d2opc[1]) {
            case 0x31: ordered.lower = [&](VuMicroOperands& ops) { mr32(ops); }; break;
            case 0x3c: ordered.lower = [&](VuMicroOperands& ops) { mtir(ops); }; break;
            }
        }
        return intOps;
    }
    VuMicroOperands VuMicroInterpreter::translateLower2(u32 lower) {
        switch ((lower >> 25) & 0x7f) {
        }
        return {};
    }
}
