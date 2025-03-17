#include "entity.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Entity::init(int id, Texture& texture, Mesh& mesh, RigidBodyProperties properties) {
	util::log(name_, "initializing entity");

	id_ = id;

	texture_ = &texture;
	mesh_ = &mesh;

	// update the mesh to the given texture
	util::log(name_, "updating mesh data's texture index");
	mesh_->setTextureIndex(texture_->getIndex());

	// init rigid body
	body_.init(properties);
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
-----------------------------GETTERS-------------------------------------------
-----------------------------------------------------------------------------*/
RigidBody* Entity::getBodyPtr() { return &body_; }