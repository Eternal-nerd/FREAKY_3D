#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "util.h"
#include "assets/mesh.h"
#include "assets/texture.h"
#include "phys/rigid_body.h"

/*
Class representing an entity on the game world
manipulated by the physics sim
has references to assets that need to be updated
each frame?
*/

class Entity {
public:
	void init(int id, Texture& texture, Mesh& mesh);

	// applies transforms to entity
	void scale(float x, float y, float z);

	// moves rigid body
	void setPosition(float x, float y, float z);

	// rotates it
	void setOrientation(float x, float y, float z);

	glm::mat4 getModelMat();

	void draw(VkCommandBuffer commandBuffer);

	// GETTERS
	RigidBody* getBodyPtr();

	const std::string name_ = "ENTITY";

private:
	int id_ = -1;

	// need a reference to mesh, texture, and rigid body
	Mesh* mesh_ = nullptr;
	Texture* texture_ = nullptr;
	RigidBody body_;

	// calculated for UBO
	glm::vec3 scale_{};


	// helpers
	glm::mat4 getRotationMat();

};