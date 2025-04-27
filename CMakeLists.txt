cmake_minimum_required(VERSION 3.10)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# set the project name
project(Visual_Node_System_Tests LANGUAGES CXX)

# set different compiler flags depending on build type
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP")

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Force your path to Dear ImGui
set(DEAR_IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui CACHE PATH "Path to Dear ImGui directory." FORCE)
set(USE_STATIC_RUNTIME ON CACHE BOOL "Use static runtime (/MT) instead of dynamic (/MD)")

if(USE_STATIC_RUNTIME)
	set(VISUAL_NODE_SYSTEM_USE_STATIC_RUNTIME ON CACHE BOOL "Use static runtime (/MT) instead of dynamic (/MD) for VisualNodeSystem" FORCE)
	set(VISUAL_NODE_SYSTEM_GLFW_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/lib/x64/glfw3_mt.lib CACHE PATH "Path to GLFW library." FORCE)

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	set(VISUAL_NODE_SYSTEM_USE_STATIC_RUNTIME OFF CACHE BOOL "Use static runtime (/MT) instead of dynamic (/MD) for VisualNodeSystem" FORCE)
	set(VISUAL_NODE_SYSTEM_GLFW_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/lib/x64/glfw3.lib CACHE PATH "Path to GLFW library." FORCE)

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

if(NOT TARGET VisualNodeSystem)
	add_subdirectory(MasterBranchCode)
endif()

# *************** THIRD_PARTY ***************

file(GLOB imgui_SRC
	"ThirdParty/imgui/imgui.cpp"
	"ThirdParty/imgui/imgui_demo.cpp"
	"ThirdParty/imgui/imgui_draw.cpp"
	"ThirdParty/imgui/imgui_impl_glfw.cpp"
	"ThirdParty/imgui/imgui_impl_opengl3.cpp"
	"ThirdParty/imgui/imgui_tables.cpp"
	"ThirdParty/imgui/imgui_widgets.cpp"
)

# *************** THIRD_PARTY END ***************

file(GLOB CustomNodes_SRC
	"CustomNodes/CustomNode.h"
	"CustomNodes/CustomNode.cpp"
	"CustomNodes/CustomNode2.h"
	"CustomNodes/CustomNode2.cpp"
	"CustomNodes/CustomNode3.h"
	"CustomNodes/CustomNode3.cpp"
	"CustomNodes/CustomNode4.h"
	"CustomNodes/CustomNode4.cpp"
	"CustomNodes/CustomNode5.h"
	"CustomNodes/CustomNode5.cpp"
	"CustomNodes/CustomNodeStyleDemonstration.h"
	"CustomNodes/CustomNodeStyleDemonstration.cpp"
)

file(GLOB Basic_SRC
	"Basic/Basic.cpp"
	"Basic/Basic.h"
)

file(GLOB NodeAreaLoad_SRC
	"NodeAreaLoadTests.cpp"
	"NodeAreaLoadTests.h"
)

file(GLOB NodeFactory_SRC
	"NodeFactoryTests.cpp"
	"NodeFactoryTests.h"
)

file(GLOB GroupComment_SRC
	"GroupCommentTests.cpp"
	"GroupCommentTests.h"
)

file(GLOB source_SRC
	"main.cpp"
	"Core.h"
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/lib/$(PlatformTarget))
if(USE_STATIC_RUNTIME)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/googletest/lib/$(Configuration)/Static)
else()
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/googletest/lib/$(Configuration)/Dynamic)
endif()

# add the executable
add_executable(Visual_Node_System_Tests
			   ${source_SRC}
			   ${CustomNodes_SRC}
			   ${Basic_SRC}
			   ${NodeAreaLoad_SRC}
			   ${NodeFactory_SRC}
			   ${GroupComment_SRC}
			   # *************** THIRD_PARTY ***************
			   ${imgui_SRC}
)

target_link_libraries(Visual_Node_System_Tests 
    PRIVATE
        VisualNodeSystem
		${VISUAL_NODE_SYSTEM_GLFW_LIBRARY}
		gtest
		gtest_main
)

source_group("Source Files" FILES ${source_SRC})
source_group("Source Files/CustomNodes" FILES ${CustomNodes_SRC})
source_group("Source Files/Basic" FILES ${Basic_SRC})
source_group("Source Files/NodeAreaLoad" FILES ${NodeAreaLoad_SRC})
source_group("Source Files/NodeFactory" FILES ${NodeFactory_SRC})
source_group("Source Files/GroupComment" FILES ${GroupComment_SRC})
# *************** THIRD_PARTY ***************
source_group("Source Files/ThirdParty/imgui" FILES ${imgui_SRC})

# set the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Visual_Node_System_Tests)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui
	${VISUAL_NODE_SYSTEM_DIR}
	${VISUAL_NODE_SYSTEM_THIRDPARTY_DIR}
	"ThirdParty/googletest/include"
)