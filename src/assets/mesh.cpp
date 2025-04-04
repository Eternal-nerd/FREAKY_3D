#include "mesh.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Mesh::init(MeshData data, VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue) {
	util::log(name_, "initializing mesh");

	data_ = data;
	
	physicalDevice_ = physicalDevice;
	device_ = device;
	commandPool_ = commandPool;
	graphicsQueue_ = graphicsQueue;

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
		stagingBuffer, stagingBufferMemory, device_,
		physicalDevice_);

	void* data;
	vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, data_.vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device_, stagingBufferMemory);

	util::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_, vertexBufferMemory_,
		device_, physicalDevice_);

	util::copyBuffer(stagingBuffer, vertexBuffer_, bufferSize, physicalDevice_, device_, commandPool_, graphicsQueue_);

	vkDestroyBuffer(device_, stagingBuffer, nullptr);
	vkFreeMemory(device_, stagingBufferMemory, nullptr);
}

void Mesh::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(data_.indices[0]) * data_.indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	util::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory, device_,
		physicalDevice_);

	void* data;
	vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, data_.indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device_, stagingBufferMemory);

	util::createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer_, indexBufferMemory_,
		device_, physicalDevice_);

	util::copyBuffer(stagingBuffer, indexBuffer_, bufferSize, physicalDevice_, device_, commandPool_, graphicsQueue_);

	vkDestroyBuffer(device_, stagingBuffer, nullptr);
	vkFreeMemory(device_, stagingBufferMemory, nullptr);
}


/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Mesh::cleanup() {
	util::log(name_, "cleaning up mesh");

	vkDestroyBuffer(device_, indexBuffer_, nullptr);
	vkFreeMemory(device_, indexBufferMemory_, nullptr);
	vkDestroyBuffer(device_, vertexBuffer_, nullptr);
	vkFreeMemory(device_, vertexBufferMemory_, nullptr);
}


