#include "overlay.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Overlay::init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass) {
	device_ = device;
	physicalDevice_ = physicalDevice;
	renderpass_ = renderpass;
}
