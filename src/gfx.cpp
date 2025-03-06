#include "gfx.h"

#include <stdexcept>
#include <set>
#include <array>

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Gfx::init() {
	util::log(name, "initializing");

	util::log(name, "initializing SDL");
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		throw std::runtime_error("Failed to initialize SDL");
	}

    util::log(name, "creating SDL window");
    window_ = SDL_CreateWindow("FREAKY_3D", WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
    if (!window_) {
        throw std::runtime_error("Failed to create SDL window");
    }

    util::log(name, "setting SDL window to be resizable");
    if (!SDL_SetWindowResizable(window_, true)) {
        throw std::runtime_error("Failed to make window resizable");
    }

    util::log(name, "set mouse into relative mode (for FPS camera)");
    if (!SDL_SetWindowRelativeMouseMode(window_, true)) {
        throw std::runtime_error("Failed to put mouse into relative mode");
    }

    // Init Vulkan instance and logical device
    createDevice();

    // create Vulkan render pass
    createRenderPass();

    // swapchain and stuff
    createSwapchain();

    // MULTI THREADING INVOLVED HERE:
    // Assuming that the asset loading pipeline was kicked off prior to this point 
    // Assets class should know the texture image COUNT by this point (shared resource)
    // the following function will access that shared resource to see
    // how many texture sampler descriptors to specify in the descriptor set layout
    // 
    //createDescriptorSetLayout();
    

}

/*-----------------------------------------------------------------------------
------------------------------DEVICE-CREATION----------------------------------
-----------------------------------------------------------------------------*/
void Gfx::createDevice() {

    // Vulkan instance --------------------====<
    util::log(name, "creating Vulkan instance");
    if (enableValidationLayers && !util::checkValidationLayerSupport(validationLayers)) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "FREAKY_3D";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    auto extensions = util::getRequiredExtensions();
    if (enableValidationLayers) {
        // need to add debug util to extensions
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        util::populateDebugMessengerCreateInfo(debugCreateInfo);
        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

    // Vulkan debug layer --------------------====<
    // setup debug messager if in debug mode
    if (enableValidationLayers) {
        util::log(name, "setting up debug messenger");
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        util::populateDebugMessengerCreateInfo(createInfo);

        if (util::CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger_) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    // Vulkan/SDL surface --------------------====<
    util::log(name, "creating surface");
    if (!SDL_Vulkan_CreateSurface(window_, instance_, nullptr, &surface_)) {
        throw std::runtime_error("failed to create SDL window surface!");
    }

    // Vulkan physical device (GPU) --------------------====<
    util::log(name, "selecting physical device");
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (util::isDeviceSuitable(device, surface_, deviceExtensions)) {
            physicalDevice_ = device;
            break;
        }
    }

    if (physicalDevice_ == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    // Vulkan LOGICAL Device --------------------====<
    util::log(name, "creating logical device");
    QueueFamilyIndices indices = util::findQueueFamilies(physicalDevice_, surface_);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { 
        indices.graphicsFamily.value(),
        indices.presentFamily.value() 
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &device_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    util::log(name, "getting device queues");
    vkGetDeviceQueue(device_, indices.graphicsFamily.value(), 0, &graphicsQueue_);
    vkGetDeviceQueue(device_, indices.presentFamily.value(), 0, &presentQueue_);

}

/*-----------------------------------------------------------------------------
------------------------------RENDER-PASS--------------------------------------
-----------------------------------------------------------------------------*/
void Gfx::createRenderPass() {
    util::log(name, "creating renderpass");

    // get some swapchain details here:
    SwapChainSupportDetails swapChainSupport = util::querySwapChainSupport(physicalDevice_, surface_);

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = util::chooseSwapSurfaceFormat(swapChainSupport.formats).format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = util::findDepthFormat(physicalDevice_);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { 
        colorAttachment,
        depthAttachment 
    };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device_, &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

/*-----------------------------------------------------------------------------
------------------------------SWAPCHAIN----------------------------------------
-----------------------------------------------------------------------------*/
void Gfx::createSwapchain() {
    // SWAPCHAIN ----------------------------------====<
    util::log(name, "creating swapchain");
    SwapChainSupportDetails swapChainSupport = util::querySwapChainSupport(physicalDevice_, surface_);
    VkSurfaceFormatKHR surfaceFormat = util::chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = util::chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = util::chooseSwapExtent(swapChainSupport.capabilities, window_);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapCreateInfo{};
    swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapCreateInfo.surface = surface_;

    swapCreateInfo.minImageCount = imageCount;
    swapCreateInfo.imageFormat = surfaceFormat.format;
    swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapCreateInfo.imageExtent = extent;
    swapCreateInfo.imageArrayLayers = 1;
    swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = util::findQueueFamilies(physicalDevice_, surface_);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapCreateInfo.queueFamilyIndexCount = 2;
        swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapCreateInfo.presentMode = presentMode;
    swapCreateInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device_, &swapCreateInfo, nullptr, &swapChain_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, nullptr);
    swapChainImages_.resize(imageCount);
    vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, swapChainImages_.data());

    swapChainImageFormat_ = surfaceFormat.format;
    swapChainExtent_ = extent;

    // IMAGE VIEWS ----------------------------=====<
    util::log(name, "creating swapchain image views");
    swapChainImageViews_.resize(swapChainImages_.size());

    for (uint32_t i = 0; i < swapChainImages_.size(); i++) {
        swapChainImageViews_[i] = util::createImageView(
            swapChainImages_[i], 
            swapChainImageFormat_, 
            VK_IMAGE_ASPECT_COLOR_BIT,
            device_
        );
    }

    // DEPTH RESOURCES --------------------================<
    util::log(name, "creating depth resources");
    VkFormat depthFormat = util::findDepthFormat(physicalDevice_);

    util::createImage(
        swapChainExtent_.width, 
        swapChainExtent_.height, 
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        depthImage_, 
        depthImageMemory_,
        device_, 
        physicalDevice_
    );

    depthImageView_ = util::createImageView(
        depthImage_, 
        depthFormat, 
        VK_IMAGE_ASPECT_DEPTH_BIT,
        device_
    );

    // FRAMEBUFFERS -------------------------===<
    util::log(name, "creating framebuffers");
    swapChainFramebuffers_.resize(swapChainImageViews_.size());

    for (size_t i = 0; i < swapChainImageViews_.size(); i++) {
        std::array<VkImageView, 2> attachments = { swapChainImageViews_[i], depthImageView_ };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass_;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent_.width;
        framebufferInfo.height = swapChainExtent_.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device_, &framebufferInfo, nullptr, &swapChainFramebuffers_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void Gfx::recreateSwapchain() {
    util::log(name, "recreating swapchain");
    vkDeviceWaitIdle(device_);
    cleanupSwapchain();
    createSwapchain();
}

void Gfx::cleanupSwapchain() {
    util::log(name, "cleaning up swapchain");

    util::log(name, "destroying swapchain depth resources");
    vkDestroyImageView(device_, depthImageView_, nullptr);
    vkDestroyImage(device_, depthImage_, nullptr);
    vkFreeMemory(device_, depthImageMemory_, nullptr);

    util::log(name, "destroying swapchain frame buffers");
    for (auto framebuffer : swapChainFramebuffers_) {
        vkDestroyFramebuffer(device_, framebuffer, nullptr);
    }

    util::log(name, "destroying swapchain image views");
    for (auto imageView : swapChainImageViews_) {
        vkDestroyImageView(device_, imageView, nullptr);
    }

    util::log(name, "destroying swapchain");
    vkDestroySwapchainKHR(device_, swapChain_, nullptr);
}

/*-----------------------------------------------------------------------------
-----------------------------DESCRIPTOR----------------------------------------
-----------------------------------------------------------------------------*/
/*void Gfx::createDescriptorSetLayout() {
    util::log("Creating descriptor set layout...");
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = static_cast<uint32_t>(textureNames_.size());
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding,
                                                            samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr,
        &descriptorSetLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}*/

/*-----------------------------------------------------------------------------
------------------------------DRAW-SHIT----------------------------------------
-----------------------------------------------------------------------------*/
void Gfx::render() {

}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Gfx::cleanup() {
	util::log(name, "cleaning up");

    // very first, swapchain
    cleanupSwapchain();








    // render pass
    util::log(name, "destroying render pass");
    vkDestroyRenderPass(device_, renderPass_, nullptr);

    // Devices/instance
    util::log(name, "destroying logical device");
    vkDestroyDevice(device_, nullptr);

    util::log(name, "destroying surface");
    vkDestroySurfaceKHR(instance_, surface_, nullptr);

    if (enableValidationLayers) {
        util::log(name, "destroying debug messenger");
        util::DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
    }

    util::log(name, "destroying Vulkan instance");
    vkDestroyInstance(instance_, nullptr);

    // SDL
    util::log(name, "cleaning up SDL");
    SDL_DestroyWindow(window_);
    SDL_Quit();
}