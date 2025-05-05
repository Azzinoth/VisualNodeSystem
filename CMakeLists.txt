cmake_minimum_required(VERSION 3.10)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# set the project name
project(VisualNodeSystem LANGUAGES CXX)

option(VISUAL_NODE_SYSTEM_USE_EXTERNAL_IMGUI "Use an external ImGui library instead of compiling our own" OFF)
option(VISUAL_NODE_SYSTEM_BUILD_SHARED_LIBS "Build VisualNodeSystem as a shared library" OFF)
option(VISUAL_NODE_SYSTEM_USE_STATIC_RUNTIME "Use static runtime (/MT) instead of dynamic (/MD) for VisualNodeSystem" ON)
option(VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES "Build the Visual Node System with standard nodes module" OFF)
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


set(BaseExecutionFlowNodes_SOURCE_FILES)
set(LogicalOperatorNodes_SOURCE_FILES)
set(ComparisonOperatorNodes_SOURCE_FILES)
set(ArithmeticOperatorNodes_SOURCE_FILES)
set(LiteralsNodes_SOURCE_FILES)
set(VariablesNodes_SOURCE_FILES)
set(ControlFlowNodes_SOURCE_FILES)

if(VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DVISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES")

	# *************** STANDARD_NODES ***************
	list(APPEND BaseExecutionFlowNodes_SOURCE_FILES
		"StandardNodes/ExecutionFlowNodes/BaseExecutionFlowNode.h"
		"StandardNodes/ExecutionFlowNodes/BaseExecutionFlowNode.cpp"
	)

	list(APPEND LogicalOperatorNodes_SOURCE_FILES
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/BaseLogicalOperatorNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/BaseLogicalOperatorNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/LogicalANDOperatorNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/LogicalANDOperatorNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/LogicalOROperatorNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/LogicalOROperatorNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/LogicalXOROperatorNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/LogicalXOROperatorNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/LogicalNOTOperatorNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical/LogicalNOTOperatorNode.cpp"
	)

	list(APPEND ComparisonOperatorNodes_SOURCE_FILES
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/BaseComparisonOperatorNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/BaseComparisonOperatorNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/EqualNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/EqualNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/NotEqualNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/NotEqualNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/LessThanOrEqualNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/LessThanOrEqualNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/GreaterThanOrEqualNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/GreaterThanOrEqualNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/GreaterThanNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/GreaterThanNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/LessThanNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison/LessThanNode.cpp"
	)

	list(APPEND ArithmeticOperatorNodes_SOURCE_FILES
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/BaseArithmeticOperatorNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/BaseArithmeticOperatorNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticAddNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticAddNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticSubtractNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticSubtractNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticMultiplyNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticMultiplyNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticDivideNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticDivideNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticModulusNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticModulusNode.cpp"

		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticPowerNode.h"
		"StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic/ArithmeticPowerNode.cpp"
	)

	list(APPEND LiteralsNodes_SOURCE_FILES
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/BoolLiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/BoolLiteralNode.cpp"

		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/IntegerLiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/IntegerLiteralNode.cpp"

		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/FloatLiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/FloatLiteralNode.cpp"

		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/Vec2LiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/Vec2LiteralNode.cpp"

		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/BoolVec2LiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/BoolVec2LiteralNode.cpp"

		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/Vec3LiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/Vec3LiteralNode.cpp"

		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/BoolVec3LiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/BoolVec3LiteralNode.cpp"

		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/Vec4LiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/Vec4LiteralNode.cpp"

		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/BoolVec4LiteralNode.h"
		"StandardNodes/ExecutionFlowNodes/LiteralsNodes/BoolVec4LiteralNode.cpp"
	)

	list(APPEND VariablesNodes_SOURCE_FILES
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/BoolVariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/BoolVariableNode.cpp"

		"StandardNodes/ExecutionFlowNodes/VariablesNodes/IntegerVariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/IntegerVariableNode.cpp"

		"StandardNodes/ExecutionFlowNodes/VariablesNodes/FloatVariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/FloatVariableNode.cpp"

		"StandardNodes/ExecutionFlowNodes/VariablesNodes/Vec2VariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/Vec2VariableNode.cpp"

		"StandardNodes/ExecutionFlowNodes/VariablesNodes/BoolVec2VariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/BoolVec2VariableNode.cpp"

		"StandardNodes/ExecutionFlowNodes/VariablesNodes/Vec3VariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/Vec3VariableNode.cpp"

		"StandardNodes/ExecutionFlowNodes/VariablesNodes/BoolVec3VariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/BoolVec3VariableNode.cpp"

		"StandardNodes/ExecutionFlowNodes/VariablesNodes/Vec4VariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/Vec4VariableNode.cpp"

		"StandardNodes/ExecutionFlowNodes/VariablesNodes/BoolVec4VariableNode.h"
		"StandardNodes/ExecutionFlowNodes/VariablesNodes/BoolVec4VariableNode.cpp"
	)

	list(APPEND ControlFlowNodes_SOURCE_FILES
		"StandardNodes/ExecutionFlowNodes/ControlFlowNodes/BranchNode.h"
		"StandardNodes/ExecutionFlowNodes/ControlFlowNodes/BranchNode.cpp"

		"StandardNodes/ExecutionFlowNodes/ControlFlowNodes/SequenceNode.h"
		"StandardNodes/ExecutionFlowNodes/ControlFlowNodes/SequenceNode.cpp"

		"StandardNodes/ExecutionFlowNodes/ControlFlowNodes/LoopNode.h"
		"StandardNodes/ExecutionFlowNodes/ControlFlowNodes/LoopNode.cpp"

		"StandardNodes/ExecutionFlowNodes/ControlFlowNodes/WhileLoopNode.h"
		"StandardNodes/ExecutionFlowNodes/ControlFlowNodes/WhileLoopNode.cpp"
	)
	# *************** STANDARD_NODES END ***************
endif()

set(ALL_SOURCE_FILES "")
list(APPEND ALL_SOURCE_FILES
            ${VisualNodeSystem_SRC}
			${VisualNodeArea_SRC}
			${BaseExecutionFlowNodes_SOURCE_FILES}
			${LiteralsNodes_SOURCE_FILES}
			${VariablesNodes_SOURCE_FILES}
			${LogicalOperatorNodes_SOURCE_FILES}
			${ComparisonOperatorNodes_SOURCE_FILES}
			${ArithmeticOperatorNodes_SOURCE_FILES}
			${ControlFlowNodes_SOURCE_FILES}
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
if(VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES)
	source_group("Source Files/StandardNodes/ExecutionFlowNodes" FILES ${BaseExecutionFlowNodes_SOURCE_FILES})
	source_group("Source Files/StandardNodes/ExecutionFlowNodes/LiteralsNodes" FILES ${LiteralsNodes_SOURCE_FILES})
	source_group("Source Files/StandardNodes/ExecutionFlowNodes/VariablesNodes" FILES ${VariablesNodes_SOURCE_FILES})
	source_group("Source Files/StandardNodes/ExecutionFlowNodes/OperatorNodes/Logical" FILES ${LogicalOperatorNodes_SOURCE_FILES})
	source_group("Source Files/StandardNodes/ExecutionFlowNodes/OperatorNodes/Comparison" FILES ${ComparisonOperatorNodes_SOURCE_FILES})
	source_group("Source Files/StandardNodes/ExecutionFlowNodes/OperatorNodes/Arithmetic" FILES ${ArithmeticOperatorNodes_SOURCE_FILES})
	source_group("Source Files/StandardNodes/ExecutionFlowNodes/ControlFlowNodes" FILES ${ControlFlowNodes_SOURCE_FILES})
endif()
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