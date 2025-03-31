#include "overlay.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Overlay::init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass, VkExtent2D extent, Assets& assets) {
	util::log(name_, "initializing overlay");

	device_ = device;
	physicalDevice_ = physicalDevice;
	renderpass_ = renderpass;
    swapChainExtent_ = extent;
	assets_ = &assets;

    initTextures();
    initDescriptors();
    initPipeline();

    generateElements();

    wireframeIndex_ = assets_->getTextureIndex("green");

    // loading external stuff -----------------------------==================<
    vkCmdSetPolygonModeEXT = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(vkGetDeviceProcAddr(device_, "vkCmdSetPolygonModeEXT"));
}

void Overlay::initTextures() {
    util::log(name_, "initializing overlay textures");
   
    textureCount_=0;

    // choose textures for font/ui
    textures_.push_back(&assets_->getTexture("green"));
    textureCount_++;
    textures_.push_back(&assets_->getTexture("font"));
    textureCount_++;
    textures_.push_back(&assets_->getTexture("set"));
    textureCount_++;
    
}

void Overlay::initDescriptors() {
    util::log(name_, "initializing overlay descriptors");

    // Vertex buffer --------------------------------------------=========<
    VkDeviceSize bufferSize = MAX_OVERLAY_ELEMENTS * sizeof(UIVertex);

    util::log(name_, "creating overlay vertex buffer");
    util::createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer_, vertexBufferMemory_, device_, physicalDevice_);

    // Descriptor ------------------------------------------=============<
    // Font uses a separate descriptor pool
    util::log(name_, "creating overlay descriptor pool");
    std::array<VkDescriptorPoolSize, 1> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = textureCount_;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(device_, &poolInfo, nullptr, &descriptorPool_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    // Descriptor set layout -------------------------------=========<
    util::log(name_, "creating text overlay descriptor set layout");
    std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings;
    setLayoutBindings[0].binding = 0;
    setLayoutBindings[0].descriptorCount = textureCount_;
    setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLayoutBindings[0].pImmutableSamplers = nullptr;
    setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    layoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr, &descriptorSetLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    // Descriptor set ---------------------------------------=============<
    util::log(name_, "creating overlay descriptor set");
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool_;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout_;

    if (vkAllocateDescriptorSets(device_, &allocInfo, &descriptorSet_) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }


    // Descriptors for the font images -------------------------=====<
    std::vector<VkDescriptorImageInfo> textureDescriptors(textureCount_);
    for (int i = 0; i < textureCount_; i++) {
        textureDescriptors[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        textureDescriptors[i].imageView = textures_[i]->getTextureImageView();
        textureDescriptors[i].sampler = textures_[i]->getTextureSampler();
    }

    std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSet_;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[0].descriptorCount = static_cast<uint32_t>(textureCount_);
    descriptorWrites[0].pImageInfo = textureDescriptors.data();

    vkUpdateDescriptorSets(device_, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Overlay::initPipeline() {
    util::log(name_, "initializing overlay pipeline");

    util::log(name_, "loading overlay shaders");
    auto vertShaderCode = util::readFile("../shaders/compiled/text_vert.spv");
    auto fragShaderCode = util::readFile("../shaders/compiled/text_frag.spv");

    VkShaderModule vertShaderModule = util::createShaderModule(vertShaderCode, device_);
    VkShaderModule fragShaderModule = util::createShaderModule(fragShaderCode, device_);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.push_back(vertShaderStageInfo);
    shaderStages.push_back(fragShaderStageInfo);

    // Pipeline cache
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (vkCreatePipelineCache(device_, &pipelineCacheCreateInfo, nullptr, &pipelineCache_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline cache!");
    }

    // Layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout_;

    if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // Enable blending, using alpha from red channel of the font texture (see text.frag)
    VkPipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; // IMPORTANT!!!
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    //rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.lineWidth = 1.0f;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthBiasEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_COPY;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &blendAttachmentState;

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_FALSE;
    depthStencilState.depthWriteEnable = VK_FALSE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.stencilTestEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;


    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.sampleShadingEnable = VK_FALSE; // remove?
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_POLYGON_MODE_EXT };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicState.pDynamicStates = dynamicStateEnables.data();


    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    auto bindingDescription = UIVertex::getBindingDescription();
    auto attributeDescriptions = UIVertex::getAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = pipelineLayout_;
    pipelineCreateInfo.renderPass = renderpass_;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    if (vkCreateGraphicsPipelines(device_, pipelineCache_, 1, &pipelineCreateInfo, nullptr, &pipeline_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(device_, fragShaderModule, nullptr);
    vkDestroyShaderModule(device_, vertShaderModule, nullptr);

}

void Overlay::generateElements() {
    util::log(name_, "generating overlay elements");

    // default elements
    Element e1;
    int crosshairIndex = assets_->getTextureIndex("set");
    std::vector<UIVertex> crosshairVertices = {
        { {-0.05f, -0.05f}, {0.f, 0.f}, crosshairIndex},
        { {-0.05f, 0.05f}, {0.f, 1.f}, crosshairIndex},
        { {0.05f, -0.05f}, {1.f, 0.f}, crosshairIndex},
        { {0.05f, 0.05f}, {1.f, 1.f}, crosshairIndex},
    };

    // get scale from swapchain extent
    glm::vec2 scale1 = {swapChainExtent_.width, swapChainExtent_.height};

    e1.init(crosshairVertices, scale1);

    defaultElements_.push_back(e1);



}

/*-----------------------------------------------------------------------------
------------------------------UPDATE-SHIT--------------------------------------
-----------------------------------------------------------------------------*/
void Overlay::updateExtent(VkExtent2D extent) {
    swapChainExtent_ = extent;

    // re-scale all elements here?
}

void Overlay::toggleWireframe() {
    if (currentPolygonMode_ == VK_POLYGON_MODE_FILL) {
        currentPolygonMode_ = VK_POLYGON_MODE_LINE;
        util::log(name_, "switching overlay to VK_POLYGON_MODE_LINE");
    }
    else {
        currentPolygonMode_ = VK_POLYGON_MODE_FILL;
        util::log(name_, "switching overlay to VK_POLYGON_MODE_FILL");
    }
}

void Overlay::tester() {
    if (vkMapMemory(device_, vertexBufferMemory_, 0, VK_WHOLE_SIZE, 0, (void**)&mapped_) != VK_SUCCESS) {
        throw std::runtime_error("failed to map memory for overlay update ");
    }

    assert(mapped_ != nullptr);

    // just default elements for now
    for (auto e : defaultElements_) {
        if (currentPolygonMode_ == VK_POLYGON_MODE_LINE) {
            e.map(mapped_, wireframeIndex_);
        }
        else {
            e.map(mapped_);
        }
    }


    vkUnmapMemory(device_, vertexBufferMemory_);
    mapped_ = nullptr;
}

void Overlay::beginTextUpdate() {
    if (vkMapMemory(device_, vertexBufferMemory_, 0, VK_WHOLE_SIZE, 0, (void**)&mapped_) != VK_SUCCESS) {
        throw std::runtime_error("failed to map memory for overlay update ");
    }
    //numLetters_ = 0;
}

// xPos & yPos --> -1.f - 1.f?
void Overlay::addText(const std::string& text, float xPos, float yPos) {
    assert(mapped_ != nullptr);

    // calc. letter width/height
    //const float charW = 25.f * scale / swapChainExtent_.width;
    //const float charH = 50.f * scale / swapChainExtent_.height;

    for (char letter : text) {

        // vertex 1: top left
        /*mapped->x = xPos; // position x
        mapped->y = yPos; // position y
        mapped->z = getXOffset(letter); // tex coord x
        mapped->w = getYOffset(letter); // tex coord y
        mapped++;

        // vertex 2: top right 
        mapped->x = xPos + charW;
        mapped->y = yPos;
        mapped->z = getXOffset(letter) + LETTER_OFFSET_X;
        mapped->w = getYOffset(letter);
        mapped++;

        // vertex 3: bottom left
        mapped->x = xPos;
        mapped->y = yPos + charH;
        mapped->z = getXOffset(letter);
        mapped->w = getYOffset(letter) + LETTER_OFFSET_Y;
        mapped++;

        // vertex 4: bottom right
        mapped->x = xPos + charW;
        mapped->y = yPos + charH;
        mapped->z = getXOffset(letter) + LETTER_OFFSET_X;
        mapped->w = getYOffset(letter) + LETTER_OFFSET_Y;
        mapped++;

        xPos += charW;

        numLetters_++;*/
    }
}

// unmap buffer
void Overlay::endTextUpdate() {
    vkUnmapMemory(device_, vertexBufferMemory_);
    mapped_ = nullptr;
}

/*-----------------------------------------------------------------------------
------------------------------DRAW---------------------------------------------
-----------------------------------------------------------------------------*/
void Overlay::draw(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

    // Set polygon mode and line width
    vkCmdSetPolygonModeEXT(commandBuffer, currentPolygonMode_);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &descriptorSet_, 0, NULL);

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer_, &offsets);

    vkCmdDraw(commandBuffer, 4, 1, 0, 0);

    //VkDeviceSize offsets = 0;
    //vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer_, &offsets);
    //vkCmdBindVertexBuffers(commandBuffer, 1, 1, &vertexBuffer_, &offsets);
    // One draw command for every character. This is okay for a debug overlay, but not optimal
    // In a real-world application one would try to batch draw commands
    //for (uint32_t j = 0; j < numLetters_; j++) {
    //    vkCmdDraw(commandBuffer, 4, 1, j * 4, 0);
    //}
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
