#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#elif __linux__
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include <random>
#include <queue>
#include <chrono>

#include <fstream>
#include <functional>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/trigonometric.hpp"
#include "glm/geometric.hpp"

#include "jsoncpp/json/json.h"

#include "VisualNodeSystemAPI.h"

namespace VisNodeSys
{

#define SINGLETON_PUBLIC_PART(CLASS_NAME)		\
    static CLASS_NAME& GetInstance()			\
    {											\
		static CLASS_NAME* Instance = nullptr;	\
        if (!Instance)							\
            Instance = new CLASS_NAME();		\
        return *Instance;						\
    }											\
												\
	static CLASS_NAME* GetInstancePointer()		\
	{											\
		return &GetInstance();					\
	}

#define SINGLETON_PRIVATE_PART(CLASS_NAME)	\
    CLASS_NAME();							\
	~CLASS_NAME();							\
    CLASS_NAME(const CLASS_NAME &);			\
    void operator= (const CLASS_NAME &);


#define FE_MAP_TO_STR_VECTOR(map)          \
	std::vector<std::string> result;       \
	auto iterator = map.begin();           \
	while (iterator != map.end())          \
	{                                      \
		result.push_back(iterator->first); \
		iterator++;                        \
	}                                      \
										   \
	return result;

#define VISUAL_NODE_SYSTEM_VERSION "0.1.0"

	class VISUAL_NODE_SYSTEM_API NodeCore
	{
		SINGLETON_PRIVATE_PART(NodeCore)

		friend class NodeArea;
		friend class NodeSystem;

		bool bIsInTestMode = false;

		std::string GetUniqueID();

		bool bIsFontsInitialized = false;
		std::vector<ImFont*> Fonts;
		void InitializeFonts();
		
		std::string Base64Characters =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
	public:
		SINGLETON_PUBLIC_PART(NodeCore)

		// This function can produce ID's that are "unique" with very rare collisions.
		// For most purposes it can be considered unique.
		// ID is a 24 long string.
		std::string GetUniqueHexID();

		bool SetClipboardText(std::string Text);
		std::string GetClipboardText();

		std::string Base64Encode(unsigned char const* BytesToEncode, unsigned int Length);
		bool IsBase64(unsigned char Character);
		std::string Base64Decode(std::string const& EncodedString);
	};

#ifdef VISUAL_NODE_SYSTEM_SHARED
	extern "C" __declspec(dllexport) void* GetNodeCore();
	#define NODE_CORE (*static_cast<VisNodeSys::NodeCore*>(VisNodeSys::GetNodeCore()))
#else
	#define NODE_CORE VisNodeSys::NodeCore::GetInstance()
#endif
}