#include "overlay.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Overlay::init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass, VkExtent2D extent, Assets& assets) {
	util::log(name_, "initializing overlay");

	device_ = device;
	physicalDevice_ = physicalDevice;
	renderpass_ = renderpass;
    state_.extent = extent;
	assets_ = &assets;

    config_.init("../config/overlay.cfg");

    initTextures();
    initDescriptors();
    initPipeline();

    //state_.scale = 0.5f;

    // generate elements here
    generateElements();

    wireframeIndex_ = 0;

    // loading external stuff -----------------------------==================<
    vkCmdSetPolygonModeEXT = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(vkGetDeviceProcAddr(device_, "vkCmdSetPolygonModeEXT"));

    state_.initialized = true;
}

void Overlay::initTextures() {
    util::log(name_, "initializing overlay textures");
   
    textureCount_=0;

    // choose textures for font/ui
    textures_.push_back(&assets_->getTexture("green"));
    textureCount_++;
    textures_.push_back(&assets_->getTexture("fontgrid"));
    textureCount_++;
    textures_.push_back(&assets_->getTexture("button_background"));
    textureCount_++;
    textures_.push_back(&assets_->getTexture("bar"));
    textureCount_++;
    textures_.push_back(&assets_->getTexture("set"));
    textureCount_++;
}

void Overlay::initDescriptors() {
    util::log(name_, "initializing overlay descriptors");

    // Vertex buffer --------------------------------------------=========<
    util::log(name_, "creating overlay vertex buffer");
    VkDeviceSize vertexBufferSize = MAX_OVERLAY_QUADS * sizeof(UIVertex) * 4;
    util::createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer_, vertexBufferMemory_, device_, physicalDevice_);

    // Index buffer --------------------------------------------=========<
    util::log(name_, "creating overlay index buffer");
    VkDeviceSize indexBufferSize = MAX_OVERLAY_QUADS * sizeof(uint32_t) * 6;
    util::createBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        indexBuffer_, indexBufferMemory_, device_, physicalDevice_);

    // LINE buffers 
    // vertex
    util::log(name_, "creating overlay line buffer");
    VkDeviceSize lineVertexBufferSize = MAX_OVERLAY_LINES * sizeof(UIVertex) * 2;
    util::createBuffer(lineVertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        lineVertexBuffer_, lineVertexBufferMemory_, device_, physicalDevice_);

    // Descriptor ------------------------------------------=============<
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
    auto vertShaderCode = util::readFile("../shaders/compiled/overlay_vert.spv");
    auto fragShaderCode = util::readFile("../shaders/compiled/overlay_frag.spv");

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
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // IMPORTANT!!! VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; //
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT; // VK_CULL_MODE_NONE; // 
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

    std::vector<VkDynamicState> dynamicStateEnables = { 
        VK_DYNAMIC_STATE_VIEWPORT, 
        VK_DYNAMIC_STATE_SCISSOR, 
        VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY, 
        VK_DYNAMIC_STATE_POLYGON_MODE_EXT 
    };
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

    // loop over stuff in the config file
    // name : type
    std::unordered_map<std::string, std::string> configObjects = config_.getObjects();

    for (auto it = configObjects.begin(); it != configObjects.end(); it++) {
        // Crosshair
        if (it->second == "Crosshair") {
            crosshair_.init(state_, 
                { config_.getFloatAttribute(it->first, "pixelWidth"),
                config_.getFloatAttribute(it->first, "pixelHeight") },
                config_.getIntAttribute(it->first, "texIndex")
            );
        }
        // Rectangles
        else if (it->second == "Rectangle") {
            Rectangle r;
            r.init(state_, nullptr, it->first,
                { config_.getFloatAttribute(it->first, "positionX"), 
                config_.getFloatAttribute(it->first, "positionY") },
                { config_.getIntAttribute(it->first, "widthPixel"),
                config_.getIntAttribute(it->first, "heightPixel") },
                { config_.getFloatAttribute(it->first, "topleftX"),
                config_.getFloatAttribute(it->first, "topleftY"),
                config_.getFloatAttribute(it->first, "xOffset"),
                config_.getFloatAttribute(it->first, "yOffset") },
                config_.getIntAttribute(it->first, "texIndex")
            );
            // set mode
            r.setMode(strToMode(config_.getStringAttribute(it->first, "mode")));
            rectangles_.push_back(r);
        }
        // text boxes
        else if (it->second == "Text") {
            Text t;
            t.init(state_, nullptr, it->first,
                config_.getStringAttribute(it->first, "message"), // message
                { config_.getFloatAttribute(it->first, "positionX"),
                config_.getFloatAttribute(it->first, "positionY") }, // position
                { config_.getFloatAttribute(it->first, "boxWidthPixel"),
                config_.getFloatAttribute(it->first, "boxHeightPixel") }, // box size
                { config_.getFloatAttribute(it->first, "fontWidthPixel"),
                config_.getFloatAttribute(it->first, "fontHeightPixel") }, // font size
                config_.getIntAttribute(it->first, "texIndex")                
            );
            t.setBorder(config_.getBoolAttribute(it->first, "border"));
            // set mode
            t.setMode(strToMode(config_.getStringAttribute(it->first, "mode")));
            text_.push_back(t);
        }
        // button
        else if (it->second == "Button") {
            Button b;
            b.init(state_, nullptr, it->first,
                config_.getStringAttribute(it->first, "label"), // label
                { config_.getFloatAttribute(it->first, "positionX"),
                config_.getFloatAttribute(it->first, "positionY") }, // position
                { config_.getFloatAttribute(it->first, "widthPixel"),
                config_.getFloatAttribute(it->first, "heightPixel") }, // rectangle size
                { config_.getFloatAttribute(it->first, "fontWidthPixel"),
                config_.getFloatAttribute(it->first, "fontHeightPixel") }, // font size
                config_.getIntAttribute(it->first, "fontTexIndex"),
                config_.getIntAttribute(it->first, "backgroundTexIndex")
            );
            // need to set button function pointers
            if (it->first == "resume") {
                b.setAction(&resume);
            }
            else if (it->first == "quit") {
                b.setAction(&quitF);
            }
            // set mode
            b.setMode(strToMode(config_.getStringAttribute(it->first, "mode")));
            buttons_.push_back(b);
        }
        // sliders
        else if (it->second == "Slider") {
            Slider s;
            s.init(state_, nullptr, it->first,
                config_.getStringAttribute(it->first, "label"), // label
                { config_.getFloatAttribute(it->first, "minValue"),
                config_.getFloatAttribute(it->first, "maxValue") }, // limits
                config_.getFloatAttribute(it->first, "initValue"),
                { config_.getFloatAttribute(it->first, "positionX"),
                config_.getFloatAttribute(it->first, "positionY") }, // position
                { config_.getFloatAttribute(it->first, "widthPixel"),
                config_.getFloatAttribute(it->first, "heightPixel") }, // rectangle size
                config_.getIntAttribute(it->first, "fontTexIndex"),
                config_.getIntAttribute(it->first, "backgroundTexIndex"),
                config_.getIntAttribute(it->first, "knobTexIndex"),
                config_.getIntAttribute(it->first, "barTexIndex")
            );
            // set function pointers
            if (it->first == "FOV") {
                s.setAction(&setFOV);
            }
            // set mode & pushback
            s.setMode(strToMode(config_.getStringAttribute(it->first, "mode")));
            sliders_.push_back(s);
        }

        // TODO
        else {}
    }

}

/*-----------------------------------------------------------------------------
------------------------------UPDATE-SHIT--------------------------------------
-----------------------------------------------------------------------------*/
void Overlay::updateExtent(VkExtent2D extent) {
    state_.extent = extent;
    
    if (state_.initialized) {
        crosshair_.scale();

        for (Rectangle& r : rectangles_) {
            r.scale();
        }

        for (Text& t : text_) {
            t.scale();
        }

        for (Button& b : buttons_) {
            b.scale();
        }

        for (Slider& s : sliders_) {
            s.scale();
        }
    }

    // tell elements to re-map
    state_.updatedLine = true;
    state_.updatedTri = true;
}

void Overlay::resetUpdates() {
    updates_.unpause = false;
    updates_.quit = false; // unused lol
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

    // tell elements to re-map
    state_.updatedLine = true;
    state_.updatedTri = true;
}

void Overlay::toggleMenu() {
    util::log(name_, "toggling menu overlay");
    state_.menuShown = !state_.menuShown;

    // reset mouse position
    state_.mousePos = {0.f, 0.f};

    if (!state_.menuShown) {
        // reset interactions
        for (Rectangle& r : rectangles_) {
            r.resetInteraction();
        }

        for (Text& t : text_) {
            t.resetInteraction();
        }

        for (Button& b : buttons_) {
            b.resetInteraction();
        }

        for (Slider& s : sliders_) {
            s.resetInteraction();
        }
    }

    // tell elements to re-map
    state_.updatedLine = true;
    state_.updatedTri = true;
}

void Overlay::mouseButtonTrigger(bool state) {
    state_.mouseDown = state;

    if (state_.menuShown) {
        // call mousebutton for elements
        for (Rectangle& r : rectangles_) {
            r.onMouseButton();
        }

        for (Text& t : text_) {
            t.onMouseButton();
        }

        for (Button& b : buttons_) {
            b.onMouseButton();
        }

        for (Slider& s : sliders_) {
            s.onMouseButton();
        }

        // tell elements to re-map
        state_.updatedLine = true;
        state_.updatedTri = true;
    }
}

void Overlay::updateMousePosition(float xPos, float yPos) {
    
    if (state_.menuShown) { 
        // update old position
        state_.oldMousePos.x = state_.mousePos.x;
        state_.oldMousePos.y = state_.mousePos.y;

        // update current position
        state_.mousePos.x = -1 + (2 * (xPos / state_.extent.width));
        state_.mousePos.y = -1 + (2 * (yPos / state_.extent.height));


        // call elements on mouse move
        for (Rectangle& r : rectangles_) {
            r.onMouseMove();
        }

        for (Text& t : text_) {
            t.onMouseMove();
        }
        
        for (Button& b : buttons_) {
            b.onMouseMove();
        }

        for (Slider& s : sliders_) {
            s.onMouseMove();
        }

        // tell elements to re-map
        state_.updatedLine = true;
        state_.updatedTri = true;
    }
}

// API: call start update, then can call update textbox/element methods any amt of times, then, call end update
void Overlay::update() {
    // used to move the buffer access pointers along..
    int offset = 0;
    
    // LINES ---------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    if (state_.updatedLine) {
        // map line buffer
        if (vkMapMemory(device_, lineVertexBufferMemory_, 0, VK_WHOLE_SIZE, 0, (void**)&lineVertexMapped_) != VK_SUCCESS) {
            throw std::runtime_error("failed to map vertex buffer memory for overlay update ");
        }

        assert(lineVertexMapped_ != nullptr);

        // map crosshair
        offset = crosshair_.mapLines(lineVertexMapped_);
        lineVertexMapped_ += offset;
        linePointCount_ += offset;
        
        // Text boxes
        for (Text& t : text_) {
            if (modeMapCheck(t.getMode())) {
                offset = t.mapLines(lineVertexMapped_);
                lineVertexMapped_ += offset;
                linePointCount_ += offset;
            }
        }

        // maybe add lines to others?

        // line buffer
        vkUnmapMemory(device_, lineVertexBufferMemory_);
        lineVertexMapped_ = nullptr;

        state_.updatedLine = false;
    }

    // TRIANGLES ---------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    if (state_.updatedTri) {
        indexCount_ = 0;
        int pointCount = 0;

        // triangle buffer
        if (vkMapMemory(device_, vertexBufferMemory_, 0, VK_WHOLE_SIZE, 0, (void**)&vertexMapped_) != VK_SUCCESS) {
            throw std::runtime_error("failed to map vertex buffer memory for overlay update ");
        }

        assert(vertexMapped_ != nullptr);

        int wired = (currentPolygonMode_ == VK_POLYGON_MODE_LINE) ? wireframeIndex_ : -1;

        // map rectangles
        for (Rectangle& r : rectangles_) {
            if (modeMapCheck(r.getMode())) {
                offset = r.map(vertexMapped_, wired);
                vertexMapped_ += offset;
                pointCount += offset;
            }
        }

        // Text boxes
        for (Text& t : text_) {
            if (modeMapCheck(t.getMode())) {
                offset = t.map(vertexMapped_, wired);
                vertexMapped_ += offset;
                pointCount += offset;
            }
        }

        // Buttons
        for (Button& b : buttons_) {
            if (modeMapCheck(b.getMode())) {
                offset = b.map(vertexMapped_, wired);
                vertexMapped_ += offset;
                pointCount += offset;
            }
        }

        // Sliders
        for (Slider& s : sliders_) {
            if (modeMapCheck(s.getMode())) {
                offset = s.map(vertexMapped_, wired);
                vertexMapped_ += offset;
                pointCount += offset;
            }
        }

        // points should be divisible by 4 no remainder
        if (pointCount % 4 != 0) {
            throw std::runtime_error("overlay pointCount not divisible by 4, pointCount % 4 = " + std::to_string(pointCount % 4));
        }

        // vertex buffer
        vkUnmapMemory(device_, vertexBufferMemory_);
        vertexMapped_ = nullptr;

        // INDEX MAPPING ---------------------------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        // populate index buffer
        if (vkMapMemory(device_, indexBufferMemory_, 0, VK_WHOLE_SIZE, 0, (void**)&indexMapped_) != VK_SUCCESS) {
            throw std::runtime_error("failed to map index buffer memory for overlay update ");
        }

        assert(indexMapped_ != nullptr);

        std::vector<int> indices = { 0,1,2,2,1,3 };
        for (int i = 0; i < pointCount/4; i++) {
            for (int j = 0; j < indices.size(); j++) {
                *indexMapped_ = (indices[j] + (4 * i));
                indexMapped_++;
                indexCount_++;
            }
        }

        vkUnmapMemory(device_, indexBufferMemory_);
        indexMapped_ = nullptr;

        state_.updatedTri = false;
    }
}

void Overlay::updateTextBox(const std::string& label, const std::string& newText) {
    for (Text& t : text_) {
        if (t.id_ == label) {
            t.updateMessage(newText);
            return;
        }
    }

    throw std::runtime_error("failed to find text with label: " + label);

    // IMPORTANT: text class will determine if an overlay update needs to occur
    // - if new text is same as whats already in the box, no update
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
    
    // DRAW TRIANGLES
    vkCmdSetPrimitiveTopology(commandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer_, &offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer_, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexCount_), 1, 0, 0, 0);

    // DRAW LINES
    vkCmdSetPrimitiveTopology(commandBuffer, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &lineVertexBuffer_, &offsets);
    vkCmdDraw(commandBuffer, static_cast<uint32_t>(linePointCount_), 1, 0, 0);
}

/*-----------------------------------------------------------------------------
------------------------------UTILITY------------------------------------------
-----------------------------------------------------------------------------*/
OverlayMode Overlay::strToMode(const std::string& modeString) {
    if (modeString == "OVERLAY_DEFAULT") {
        return OVERLAY_DEFAULT;
    }
    if (modeString == "OVERLAY_MENU") {
        return OVERLAY_MENU;
    }
    if (modeString == "OVERLAY_STATS") {
        return OVERLAY_STATS;
    }
    if (modeString == "OVERLAY_INVENTORY") {
        return OVERLAY_INVENTORY;
    }
    throw std::runtime_error("invalid mode: " + modeString);
}

bool Overlay::modeMapCheck(OverlayMode mode) {
    bool retVal = false;
    switch (mode) {
    case OVERLAY_MENU:
        if (state_.menuShown) {
            retVal = true;
        }
        break;
    case OVERLAY_DEFAULT:
        retVal = true;
        break;
    default:
        break;
    } 
    return retVal;
}

/*-----------------------------------------------------------------------------
------------------------------BUTTON-AND-SLIDER-FUNCTIONS----------------------
-----------------------------------------------------------------------------*/
void Overlay::resume() {
    updates_.unpause = true;
}

void Overlay::quitF() {
    updates_.quit = true;
}

void Overlay::setFOV(float fov) {
    updates_.fov = fov;
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Overlay::cleanup() {
    util::log(name_, "cleaning up overlay resources");

    // WRITE IMPORTANT CHANGES TO CONFIG FILE
    
    // FIXME
    for (Rectangle& r : rectangles_) {
        config_.setAttributeString(r.id_, "positionX", std::to_string(r.getPosition().x));
        config_.setAttributeString(r.id_, "positionY", std::to_string(r.getPosition().y));
        r.cleanup();
    }

    for (Text& t : text_) {
        config_.setAttributeString(t.id_, "positionX", std::to_string(t.getPosition().x));
        config_.setAttributeString(t.id_, "positionY", std::to_string(t.getPosition().y));
        t.cleanup();
    }

    for (Button& b : buttons_) {
        config_.setAttributeString(b.id_, "positionX", std::to_string(b.getPosition().x));
        config_.setAttributeString(b.id_, "positionY", std::to_string(b.getPosition().y));
        b.cleanup();
    }

    for (Slider& s : sliders_) {
        config_.setAttributeString(s.id_, "positionX", std::to_string(s.getPosition().x));
        config_.setAttributeString(s.id_, "positionY", std::to_string(s.getPosition().y));
        config_.setAttributeString(s.id_, "initValue", std::to_string(s.getValue()));
        s.cleanup();
    }

    // vertex buffer
    vkDestroyBuffer(device_, vertexBuffer_, nullptr);
    vkFreeMemory(device_, vertexBufferMemory_, nullptr);

    //index
    vkDestroyBuffer(device_, indexBuffer_, nullptr);
    vkFreeMemory(device_, indexBufferMemory_, nullptr);

    // line vertex
    vkDestroyBuffer(device_, lineVertexBuffer_, nullptr);
    vkFreeMemory(device_, lineVertexBufferMemory_, nullptr);

    // descriptors/pipeline
    vkDestroyDescriptorSetLayout(device_, descriptorSetLayout_, nullptr);
    vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);
    vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
    vkDestroyPipelineCache(device_, pipelineCache_, nullptr);
    vkDestroyPipeline(device_, pipeline_, nullptr);

}
