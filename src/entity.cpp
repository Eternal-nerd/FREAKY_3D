#include "entity.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Entity::init(int id, Texture& texture, Mesh& mesh) {
	util::log(name_, "initializing entity");

	id_ = id;

	texture_ = &texture;
	mesh_ = &mesh;

	// update the mesh to the given texture
	util::log(name_, "updating mesh data's texture index");
	mesh_->setTextureIndex(texture_->getIndex());

	// init transforms
	scale_ = { 1.f,1.f,1.f };
}

/*-----------------------------------------------------------------------------
------------------------------DRAWING------------------------------------------
-----------------------------------------------------------------------------*/
void Entity::draw(VkCommandBuffer commandBuffer) {
	mesh_->bindVertexBuffer(commandBuffer);
	mesh_->bindIndexBuffer(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh_->getIndexCount()), 1, 0, 0, id_);
}

/*-----------------------------------------------------------------------------
-----------------------------TRANSFORMS----------------------------------------
-----------------------------------------------------------------------------*/
void Entity::scale(float x, float y, float z) {
	scale_ = {x, y, z};
}

void Entity::setPosition(float x, float y, float z) {
	body_.setPosition(glm::vec3(x, y, z));
}

void Entity::setOrientation(float x, float y, float z) {
	body_.setOrientation(glm::vec3(x, y, z));
}

/*-----------------------------------------------------------------------------
-----------------------------GETTERS-------------------------------------------
-----------------------------------------------------------------------------*/
RigidBody* Entity::getBodyPtr() { return &body_; }

glm::mat4 Entity::getModelMat() {
	glm::mat4 modelMat(1.f);

	// move to position and orient
	modelMat = glm::translate(modelMat, body_.getPosition());

	// ORIENT
	glm::vec3 orientation = body_.getOrientation();
	modelMat = glm::rotate(modelMat, glm::radians(orientation.x), glm::vec3(1.f, 0.f, 0.f)); // x
	modelMat = glm::rotate(modelMat, glm::radians(orientation.y), glm::vec3(0.f, 1.f, 0.f)); // y
	modelMat = glm::rotate(modelMat, glm::radians(orientation.z), glm::vec3(0.f, 0.f, 1.f)); // z

	// SCALE
	modelMat = glm::scale(modelMat, scale_);

	return modelMat;
}