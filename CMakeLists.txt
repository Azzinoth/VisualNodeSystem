cmake_minimum_required(VERSION 3.10)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# set the project name
project(VisualNodeSystem LANGUAGES CXX)

option(VISUAL_NODE_SYSTEM_USE_EXTERNAL_IMGUI "Use an external ImGui library instead of compiling our own" OFF)
option(VISUAL_NODE_SYSTEM_BUILD_SHARED_LIBS "Build VisualNodeSystem as a shared library" OFF)
option(VISUAL_NODE_SYSTEM_USE_STATIC_RUNTIME "Use static runtime (/MT) instead of dynamic (/MD) for VisualNodeSystem" ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    if(VISUAL_NODE_SYSTEM_USE_STATIC_RUNTIME)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    endif()

    # Always add /MP for multi-processor compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB VisualNodeArea_SRC
	"SubSystems/VisualNodeArea/VisualNodeArea.cpp"
	"SubSystems/VisualNodeArea/VisualNodeArea.h"
	"SubSystems/VisualNodeArea/VisualNodeAreaLogic.cpp"
	"SubSystems/VisualNodeArea/VisualNodeAreaInput.cpp"
	"SubSystems/VisualNodeArea/VisualNodeAreaRendering.cpp"
)

file(GLOB VisualNodeSystem_SRC
	"VisualNodeSystem.cpp"
	"VisualNodeSystem.h"
	"GroupComment.cpp"
	"GroupComment.h"
	"VisualNodeFactory.cpp"
	"VisualNodeFactory.h"
	"VisualNode.cpp"
	"VisualNode.h"
	"VisualNodeSocket.cpp"
	"VisualNodeSocket.h"
	"VisualNodeCore.cpp"
	"VisualNodeCore.h"
	"VisualNodeSystemAPI.h"
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

set(ALL_SOURCE_FILES "")
list(APPEND ALL_SOURCE_FILES
            ${VisualNodeSystem_SRC}
			${VisualNodeArea_SRC}
			# *************** THIRD_PARTY ***************
			${jsoncpp_SRC}
)

if(VISUAL_NODE_SYSTEM_BUILD_SHARED_LIBS)
    add_library(VisualNodeSystem SHARED ${ALL_SOURCE_FILES})   
    target_compile_definitions(VisualNodeSystem PRIVATE VISUAL_NODE_SYSTEM_EXPORTS)
    target_compile_definitions(VisualNodeSystem PUBLIC VISUAL_NODE_SYSTEM_SHARED)

	# Sometimes /GL conflicts with WINDOWS_EXPORT_ALL_SYMBOLS
	set_target_properties(VisualNodeSystem PROPERTIES COMPILE_OPTIONS "/GL-")
	set_target_properties(VisualNodeSystem PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

	# Disable C4251 warning
	target_compile_options(VisualNodeSystem PRIVATE /wd4251)
else()
    add_library(VisualNodeSystem STATIC ${ALL_SOURCE_FILES})
endif()

source_group("Source Files" FILES ${VisualNodeSystem_SRC})
source_group("Source Files/SubSystems/VisualNodeArea/" FILES ${VisualNodeArea_SRC})
# *************** THIRD_PARTY ***************
source_group("Source Files/ThirdParty/jsoncpp" FILES ${jsoncpp_SRC})

set(VISUAL_NODE_SYSTEM_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

# set the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VisualNodeSystem)

# Prompt to give Dear ImGui path
set(DEAR_IMGUI_INCLUDE_DIR "" CACHE PATH "Path to Dear ImGui directory.")

if(VISUAL_NODE_SYSTEM_BUILD_SHARED_LIBS)
	if(VISUAL_NODE_SYSTEM_USE_EXTERNAL_IMGUI)
		# Use external ImGui library
		set(EXTERNAL_IMGUI_LIBRARY "" CACHE FILEPATH "Path to external ImGui library")
		if(NOT EXTERNAL_IMGUI_LIBRARY)
			message(FATAL_ERROR "EXTERNAL_IMGUI_LIBRARY must be set when VISUAL_NODE_SYSTEM_USE_EXTERNAL_IMGUI is ON")
		endif()
		target_link_libraries(VisualNodeSystem PRIVATE ${EXTERNAL_IMGUI_LIBRARY})
	else()
		# If we are building .dll we would need GLFW information.
		set(VISUAL_NODE_SYSTEM_GLFW_INCLUDE_DIR "" CACHE PATH "Path to GLFW include directory.")
		target_include_directories(VisualNodeSystem PRIVATE ${VISUAL_NODE_SYSTEM_GLFW_INCLUDE_DIR})
		set(VISUAL_NODE_SYSTEM_GLFW_LIBRARY "" CACHE PATH "Path to GLFW library.")
		target_link_libraries(VisualNodeSystem PRIVATE ${VISUAL_NODE_SYSTEM_GLFW_LIBRARY})

		# Add Dear ImGui source files
		file(GLOB IMGUI_SOURCES
			"${DEAR_IMGUI_INCLUDE_DIR}/*.cpp"
			"${DEAR_IMGUI_INCLUDE_DIR}/*.h"
		)
		target_sources(VisualNodeSystem PRIVATE ${IMGUI_SOURCES})
		source_group("Source Files/ThirdParty/ImGui" FILES ${IMGUI_SOURCES})
	endif()
endif()

include_directories(
	${DEAR_IMGUI_INCLUDE_DIR}
	${VISUAL_NODE_SYSTEM_THIRDPARTY_DIR}
)

set(VISUAL_NODE_SYSTEM_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty PARENT_SCOPE)
set(VISUAL_NODE_SYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)