#include "mesh.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Mesh::init(MeshData data, GfxAccess access) {
	util::log(name_, "initializing mesh");

	data_ = data;
	access_ = access;

	createVertexBuffer();
	createIndexBuffer();
}

/*-----------------------------------------------------------------------------
------------------------------CHANGE-TEXTURE-----------------------------------
-----------------------------------------------------------------------------*/
void Mesh::setTextureIndex(int texIndex) {
	// recreate buffers (destroy them)
	cleanup();

	for (int i = 0; i < data_.vertices.size(); i++) {
		data_.vertices[i].texIndex = texIndex;
	}
	createVertexBuffer();
	createIndexBuffer();
}

/*-----------------------------------------------------------------------------
------------------------------BINDING------------------------------------------
-----------------------------------------------------------------------------*/
void Mesh::bindVertexBuffer(VkCommandBuffer commandBuffer) {
	VkDeviceSize offsets = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer_, &offsets);
}

void Mesh::bindIndexBuffer(VkCommandBuffer commandBuffer) {
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer_, 0, VK_INDEX_TYPE_UINT32);
}

/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
int Mesh::getIndexCount() {	return static_cast<int>(data_.indices.size()); }


/*-----------------------------------------------------------------------------
------------------------------BUFFERS------------------------------------------
-----------------------------------------------------------------------------*/
void Mesh::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(data_.vertices[0]) * data_.vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	util::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory, access_.device,
		access_.physicalDevice);

	void* data;
	vkMapMemory(access_.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, data_.vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(access_.device, stagingBufferMemory);

	util::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_, vertexBufferMemory_,
		access_.device, access_.physicalDevice);

	util::copyBuffer(stagingBuffer, vertexBuffer_, bufferSize, access_);

	vkDestroyBuffer(access_.device, stagingBuffer, nullptr);
	vkFreeMemory(access_.device, stagingBufferMemory, nullptr);
}

void Mesh::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(data_.indices[0]) * data_.indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	util::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory, access_.device,
		access_.physicalDevice);

	void* data;
	vkMapMemory(access_.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, data_.indices.data(), (size_t)bufferSize);
	vkUnmapMemory(access_.device, stagingBufferMemory);

	util::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer_, indexBufferMemory_,
		access_.device, access_.physicalDevice);

	util::copyBuffer(stagingBuffer, indexBuffer_, bufferSize, access_);

	vkDestroyBuffer(access_.device, stagingBuffer, nullptr);
	vkFreeMemory(access_.device, stagingBufferMemory, nullptr);
}


/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Mesh::cleanup() {
	util::log(name_, "cleaning up mesh");

	vkDestroyBuffer(access_.device, indexBuffer_, nullptr);
	vkFreeMemory(access_.device, indexBufferMemory_, nullptr);
	vkDestroyBuffer(access_.device, vertexBuffer_, nullptr);
	vkFreeMemory(access_.device, vertexBufferMemory_, nullptr);
}


