cmake_minimum_required(VERSION 3.5)
project(FREAKY_3D)

# set standard:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

# SDL3 
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
include_directories(${SDL3_INCLUDE_DIRS})

# add source files:
set(SOURCES
	src/main.cpp
	src/engine.cpp
	src/gfx.cpp
	src/util.cpp
	src/clock.cpp
	src/entity.cpp
	src/camera.cpp
	src/ui/overlay.cpp
	src/ui/element.cpp
	src/ui/text_box.cpp
	src/ui/slider.cpp
	src/assets/assets.cpp
	src/assets/texture.cpp
	src/assets/mesh.cpp
	src/phys/rigid_body.cpp
	src/phys/simulation.cpp
)

# add headers
set(HEADERS
	src/engine.h
	src/gfx.h
	src/util.h
	src/types.h
	src/clock.h
	src/entity.h
	src/camera.h
	src/ui/overlay.h
	src/ui/element.h
	src/ui/text_box.h
	src/ui/slider.h
	src/assets/assets.h
	src/assets/texture.h
	src/assets/mesh.h
	src/phys/rigid_body.h
	src/phys/simulation.h

	lib/stb_image.h
	lib/tiny_obj_loader.h
)

# add executable:
add_executable(FREAKY_3D ${SOURCES} ${HEADERS})

# add libs
target_link_libraries(FREAKY_3D PRIVATE Vulkan::Vulkan SDL3::SDL3)

# Specify the output directory for the binary
set_target_properties(FREAKY_3D PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if (WIN32)
	# Windows-specific stuff

	# compile shaders on windows
	add_custom_target(compile_shaders
		COMMENT "Compiling shaders..."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
		COMMAND "compile.bat"
	)
	add_dependencies(FREAKY_3D compile_shaders)

else()
	# Linux/other (only Linux for now)
	
	# custom target for run script on Linux (run.sh)
	# command to run in root dir:
	add_custom_target(run
		COMMAND FREAKY_3D
		DEPENDS FREAKY_3D
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
		COMMENT "Running FREAKY_3D"
	)

endif()


