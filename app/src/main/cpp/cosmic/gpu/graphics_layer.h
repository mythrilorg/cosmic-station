#pragma once
#include <variant>

#include <gpu/functions.h>
#include <gpu/renders.h>
#include <gpu/render_driver.h>
namespace cosmic::gpu {
    class GraphicsLayer : public GraphicsFunctionsRef {
    public:
        GraphicsLayer(RenderApi renderMode);
        std::variant<VkRender, Gl3Render> render;
        u32 reloadReferences();
        void updateLayer();

        std::unique_ptr<RenderDriver> hardware;
        std::optional<vk::raii::Context> app;
        std::optional<vk::raii::Instance> instance;

        std::optional<vk::raii::Device> vkDev;
        std::optional<vk::DeviceCreateInfo> deviceInfo;
        u32 queueFamilyId{};

        RenderApi graphicsApi{};
    };
}
namespace cosmic::gpu::vulcano {
    constexpr auto invQueueId{std::numeric_limits<u32>::max()};
    struct PhysicalDevice {
        PhysicalDevice() {}
        std::optional<vk::raii::PhysicalDevice> physicalDev;
        std::optional<vk::raii::Device> gpuUser;
        vk::DeviceCreateInfo info{};
        u32 desiredQueueId{invQueueId};
    };

    vk::raii::Instance createVulkanInstance(const vk::raii::Context& context);
    PhysicalDevice createPhysicalDevice(vk::raii::Instance& vki);
}