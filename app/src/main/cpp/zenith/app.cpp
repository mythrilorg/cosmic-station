#include <range/v3/algorithm.hpp>
#include <sys/system_properties.h>

#include <cassert>
#include <app.h>

namespace zenith {
    std::unique_ptr<java::JvmManager> device;
    std::shared_ptr<GlobalLogger> userLog;
    std::shared_ptr<CoreApplication> zenithApp;

    CoreApplication::CoreApplication()
        : virBlocks(std::make_shared<link::GlobalMemory>()),
          simulated(std::make_shared<console::VirtualDevices>(virBlocks)) {
        apiLevel = android_get_device_api_level();

        std::array<bool, 1> feats{
            riscFeatures.haveCrc32C()
        };
        auto failed = ranges::find_if(feats, [](auto test) { return !test; });
        if (failed != feats.end()) {
            throw fatalError("Some of the required ARM ISA sets aren't available on your host processor");
        }

        userLog->success("Device {} accepted as the host device, Android API {}", getDeviceName(), apiLevel);

        vm = std::make_unique<console::EmuVM>(device->android, virBlocks, simulated);
        vm->resetVM();
    }

    std::shared_ptr<kernel::KernelsGroup> CoreApplication::getKernelsGroup() {
        auto group{vm->biosHLE->group};
        return group;
    }

    const std::string& CoreApplication::getDeviceName() {
        if (artDeviceName.empty()) {
            std::array<char, 40> model;
            __system_property_get("android.provider.Settings.Global.device_name", model.data());
            artDeviceName = std::string(model.data());
        }
        return artDeviceName;
    }
}
