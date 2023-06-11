cmake_minimum_required(VERSION 3.1)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# set the project name
project(FEVisualNodeSystem LANGUAGES CXX)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT TARGET FEBasicApplication)
  add_subdirectory(SubSystems/FEBasicApplication)
endif()

file(GLOB FEVisualNodeSystem_SRC
	"FEVisualNodeSystem.cpp"
	"FEVisualNodeSystem.h"
	"FEVisualNodeArea.cpp"
	"FEVisualNodeArea.h"
	"FEVisualNode.cpp"
	"FEVisualNode.h"
	"FEVisualNodeSocket.cpp"
	"FEVisualNodeSocket.h"
)

# *************** THIRD_PARTY ***************

file(GLOB jsoncpp_SRC
	"ThirdParty/jsoncpp/json_reader.cpp"
	"ThirdParty/jsoncpp/json_tool.h"
	"ThirdParty/jsoncpp/json_value.cpp"
	"ThirdParty/jsoncpp/json_valueiterator.inl"
	"ThirdParty/jsoncpp/json_writer.cpp"
)

# *************** THIRD_PARTY END ***************

add_library(FEVisualNodeSystem
			${FEVisualNodeSystem_SRC}
			# *************** THIRD_PARTY ***************
			${jsoncpp_SRC}
)

target_link_libraries(FEVisualNodeSystem FEBasicApplication)

source_group("Source Files" FILES ${FEVisualNodeSystem_SRC})
# *************** THIRD_PARTY ***************
source_group("Source Files/ThirdParty/jsoncpp" FILES ${jsoncpp_SRC})

set(VISUAL_NODE_SYSTEM_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

# set the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FEVisualNodeSystem)

include_directories(
	${GLEW_INCLUDE_DIR}
	${BASICAPP_THIRDPARTY_DIR}
	${BASICAPP_DIR}
	${VISUAL_NODE_SYSTEM_THIRDPARTY_DIR}
)

set(VISUAL_NODE_SYSTEM_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty PARENT_SCOPE)
set(VISUAL_NODE_SYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# Re-export of the variables from child projects
set(GLEW_LIB_DIR ${GLEW_LIB_DIR} PARENT_SCOPE)
set(GLFW_LIB_DIR ${GLFW_LIB_DIR} PARENT_SCOPE)

set(GLEW_INCLUDE_DIR ${GLEW_INCLUDE_DIR} PARENT_SCOPE)
set(BASICAPP_THIRDPARTY_DIR ${BASICAPP_THIRDPARTY_DIR} PARENT_SCOPE)
set(BASICAPP_DIR ${BASICAPP_DIR} PARENT_SCOPE)
