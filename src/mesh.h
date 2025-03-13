#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "types.h"
#include "util.h"

/*
Class for managing mesh data used to make vulkan vertex/index buffers
Could provide some vertex processing here 
*/
class Mesh {
public:

	void init(MeshData data, GfxAccess access);


	void cleanup();

	const std::string name_ = "MESH";
private:
	MeshData data_;
	GfxAccess access_;

	VkBuffer vertexBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory_ = VK_NULL_HANDLE;
	VkBuffer indexBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory_ = VK_NULL_HANDLE;

	void createVertexBuffer();
	void createIndexBuffer();


};