#pragma once

#include <vulkan/vulkan.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <optional>
#include <vector>
#include <array>

const int MAX_MODELS = 512;

struct UniformBufferObject {
    alignas(16) glm::mat4 model[MAX_MODELS];
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    int texIndex;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32_SINT;
        attributeDescriptions[3].offset = offsetof(Vertex, texIndex);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

// used to remove duplicate vertices in .obj files
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

// used by mesh class
struct MeshData {
    std::vector<Vertex> vertices = {};
    std::vector<uint32_t> indices = {};
};


// Used for Vulkan device selection
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

// Texture stuff
struct TextureDetails {
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;
};

struct GfxAccess {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
};

// Camera config struct
struct CameraConfig {
    float fovy = 0;
    float near = 0;
    float far = 0;
    glm::mat4 perspectiveM = glm::mat4(1);
};

// for initializing a rigid body
struct RigidBodyProperties {
    glm::vec3 position = { 0.f, 0.f, 0.f };
    glm::vec3 linearVel = { 0.f, 0.f, 0.f };
    glm::mat3 orientation = glm::mat3(1);
    glm::vec3 angleVel = { 0.f, 0.f, 0.f };
};


/* helpers used for basic geometric meshs */
namespace Geometry {
    namespace Plane {
        static MeshData getMeshData() {
            MeshData data;
            data.vertices = {
                { { -0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 1.0f }, 0 },
                { {  0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 1.0f }, 0 },
                { {  0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 0.0f }, 0 },
                { { -0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 0.0f }, 0 },
            };
            data.indices = {
                0,1,2, 0,2,3
            };
            return data;
        }
    }

    namespace TexturedCube {
        static MeshData getMeshData() {
            MeshData data;
            data.vertices = {
                { { -0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 0.0f }, 0 },
                { {  0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 0.0f }, 0 },
                { {  0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 1.0f }, 0 },
                { { -0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 1.0f }, 0 },

                { {  0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 0.0f }, 0 },
                { {  0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 0.0f }, 0 },
                { {  0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 1.0f }, 0 },
                { {  0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 1.0f }, 0 },

                { { -0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 0.0f }, 0 },
                { {  0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 0.0f }, 0 },
                { {  0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 1.0f }, 0 },
                { { -0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 1.0f }, 0 },

                { { -0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 0.0f }, 0 },
                { { -0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 0.0f }, 0 },
                { { -0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 1.0f }, 0 },
                { { -0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 1.0f }, 0 },

                { {  0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 0.0f }, 0 },
                { { -0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 0.0f }, 0 },
                { { -0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 1.0f }, 0 },
                { {  0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 1.0f }, 0 },

                { { -0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 0.0f }, 0 },
                { {  0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 0.0f }, 0 },
                { {  0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 1.0f }, 0 },
                { { -0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 1.0f }, 0 },
            };
            data.indices = {
                0,1,2, 0,2,3, 6,5,4, 7,6,4, 10,9,8, 11,10,8, 12,13,14, 12,14,15, 18,17,16, 19,18,16, 20,21,22, 20,22,23
            };
            return data;
        }
    }

    /* BASED ON SPECIFIC SKYBOX FILE USED:
    total width: 4096px
    total height: 3072px
    letter width & height: 1024px
    rows: 3
    col: 4
    */
    namespace InvertedTexturedCube {
        static MeshData getMeshData() {
            MeshData data;
            data.vertices = {
                { { -0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 6.f / 9.f }, 0 }, // BACK FACE
                { {  0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.75f, 6.f / 9.f }, 0 },
                { {  0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.75f, 3.f / 9.f }, 0 },
                { { -0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 1.0f, 3.f / 9.f }, 0 },

                { {  0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.75f, 3.f / 9.f }, 0 }, // RIGHT FACE
                { {  0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.5f, 3.f / 9.f }, 0 },
                { {  0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.5f, 6.f / 9.f }, 0 },
                { {  0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.75f, 6.f / 9.f }, 0 },

                { { -0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.25f, 6.f / 9.f }, 0 }, // FRONT FACE
                { {  0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.5f, 6.f / 9.f }, 0 },
                { {  0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.5f, 3.f / 9.f }, 0 },
                { { -0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.25f, 3.f / 9.f }, 0 },

                { { -0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.25f, 6.f / 9.f }, 0 }, // LEFT FACE
                { { -0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 6.f / 9.f }, 0 },
                { { -0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.0f, 3.f / 9.f }, 0 },
                { { -0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.25f, 3.f / 9.f }, 0 },

                { {  0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.5f, 0.0f }, 0 }, // TOP FACE
                { { -0.5,  0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.25f, 0.0f }, 0 },
                { { -0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.25f, 3.f / 9.f }, 0 },
                { {  0.5,  0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.5f, 3.f / 9.f }, 0 },

                { { -0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.5f, 6.f / 9.f }, 0 }, // BOTTOM FACE
                { {  0.5, -0.5, -0.5 }, {1.0, 1.0, 1.0}, { 0.25f, 6.f / 9.f }, 0 },
                { {  0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.25f, 1.0f }, 0 },
                { { -0.5, -0.5,  0.5 }, {1.0, 1.0, 1.0}, { 0.5f, 1.0f }, 0 },
            };

            data.indices = {
                2,1,0, 3,2,0, 4,5,6, 4,6,7, 8,9,10, 8,10,11, 14,13,12, 15,14,12, 16,17,18, 16,18,19, 22,21,20, 23,22,20
            };
            return data;
        }
    }


};