#include "overlay.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Overlay::init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass, Assets& assets) {
	util::log(name_, "initializing overlay");

	device_ = device;
	physicalDevice_ = physicalDevice;
	renderpass_ = renderpass;
	assets_ = &assets;

    initTextures();
    initDescriptors();
    initPipeline();
}

void Overlay::initDescriptors() {
    util::log(name_, "initializing overlay descriptors");
}

void Overlay::initPipeline() {
    util::log(name_, "initializing overlay pipeline");
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Overlay::cleanup() {
    util::log(name_, "cleaning up overlay resources");

    // vertex buffer
    vkDestroyBuffer(device_, vertexBuffer_, nullptr);
    vkFreeMemory(device_, vertexBufferMemory_, nullptr);

    // descriptors/pipeline
    vkDestroyDescriptorSetLayout(device_, descriptorSetLayout_, nullptr);
    vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);
    vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
    vkDestroyPipelineCache(device_, pipelineCache_, nullptr);
    vkDestroyPipeline(device_, pipeline_, nullptr);

}