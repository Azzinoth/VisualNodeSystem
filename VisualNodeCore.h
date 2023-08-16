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
#include "glm/trigonometric.hpp"
#include "glm/geometric.hpp"

#include "jsoncpp/json/json.h"

namespace VisNodeSys
{
#define SINGLETON_PUBLIC_PART(CLASS_NAME)  \
static CLASS_NAME& getInstance()           \
{										   \
	if (!Instance)                         \
		Instance = new CLASS_NAME();       \
	return *Instance;				       \
}                                          \
										   \
~CLASS_NAME();

#define SINGLETON_PRIVATE_PART(CLASS_NAME) \
static CLASS_NAME* Instance;               \
CLASS_NAME();                              \
CLASS_NAME(const CLASS_NAME &);            \
void operator= (const CLASS_NAME &);

#define FE_MAP_TO_STR_VECTOR(map)          \
std::vector<std::string> result;           \
auto iterator = map.begin();               \
while (iterator != map.end())              \
{                                          \
	result.push_back(iterator->first);     \
	iterator++;                            \
}                                          \
                                           \
return result;

	static std::string GetUniqueID()
	{
		static std::random_device RandomDevice;
		static std::mt19937 mt(RandomDevice());
		static std::uniform_int_distribution<int> distribution(0, 128);

		static bool FirstInitialization = true;
		if (FirstInitialization)
		{
			srand(static_cast<unsigned>(time(nullptr)));
			FirstInitialization = false;
		}

		std::string ID;
		ID += static_cast<char>(distribution(mt));
		for (size_t j = 0; j < 11; j++)
		{
			ID.insert(rand() % ID.size(), 1, static_cast<char>(distribution(mt)));
		}

		return ID;
	}

	// This function can produce ID's that are "unique" with very rare collisions.
	// For most purposes it can be considered unique.
	// ID is a 24 long string.
	static std::string GetUniqueHexID()
	{
		const std::string ID = GetUniqueID();
		std::string IDinHex;

		for (size_t i = 0; i < ID.size(); i++)
		{
			IDinHex.push_back("0123456789ABCDEF"[(ID[i] >> 4) & 15]);
			IDinHex.push_back("0123456789ABCDEF"[ID[i] & 15]);
		}

		const std::string AdditionalRandomness = GetUniqueID();
		std::string AdditionalString;
		for (size_t i = 0; i < ID.size(); i++)
		{
			AdditionalString.push_back("0123456789ABCDEF"[(AdditionalRandomness[i] >> 4) & 15]);
			AdditionalString.push_back("0123456789ABCDEF"[AdditionalRandomness[i] & 15]);
		}
		std::string FinalID;

		for (size_t i = 0; i < ID.size() * 2; i++)
		{
			if (rand() % 2 - 1)
			{
				FinalID += IDinHex[i];
			}
			else
			{
				FinalID += AdditionalString[i];
			}
		}

		return FinalID;
	}

	static bool SetClipboardText(std::string Text)
	{
#ifdef _WIN32
		if (OpenClipboard(nullptr))
		{
			EmptyClipboard();

			const HGLOBAL HMem = GlobalAlloc(GMEM_MOVEABLE, Text.size() + 1);
			memcpy(GlobalLock(HMem), Text.c_str(), Text.size() + 1);
			GlobalUnlock(HMem);

			SetClipboardData(CF_TEXT, HMem);

			CloseClipboard();
			return true;
		}
#elif __linux__
		Display* display = XOpenDisplay(nullptr);
		Window root = DefaultRootWindow(display);

		Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
		Atom utf8 = XInternAtom(display, "UTF8_STRING", False);

		XSetSelectionOwner(display, clipboard, None, CurrentTime);

		if (XGetSelectionOwner(display, clipboard) != None)
		{
			XSetSelectionOwner(display, clipboard, None, CurrentTime);
			XCloseDisplay(display);
			return false;
		}

		XChangeProperty(display, root, clipboard, utf8, 8, PropModeReplace, (unsigned char*)Text.c_str(), Text.size());

		XCloseDisplay(display);
		return true;
#endif
		return false;
	}

	static std::string GetClipboardText()
	{
		std::string text;

#ifdef _WIN32
		if (OpenClipboard(nullptr))
		{
			HANDLE data = nullptr;
			data = GetClipboardData(CF_TEXT);
			if (data != nullptr)
			{
				const char* PszText = static_cast<char*>(GlobalLock(data));
				if (PszText != nullptr)
					text = PszText;
			}

			CloseClipboard();
		}
#elif __linux__
		Display* display = XOpenDisplay(nullptr);
		Window root = DefaultRootWindow(display);

		Atom clipboard = XInternAtom(display, "CLIPBOARD", False);

		Atom actualType;
		int actualFormat;
		unsigned long itemCount, bytesAfter;
		unsigned char* data = nullptr;

		XGetWindowProperty(display, root, clipboard, 0, 0, False, AnyPropertyType,
		                   &actualType, &actualFormat, &itemCount, &bytesAfter, &data);

		if(data != nullptr)
		{
			XFree(data);
			data = nullptr;

			XGetWindowProperty(display, root, clipboard, 0, bytesAfter, False, AnyPropertyType,
			                   &actualType, &actualFormat, &itemCount, &bytesAfter, &data);

			if(data != nullptr)
			{
				text = reinterpret_cast<char*>(data);
				XFree(data);
				return text;
			}
		}

		XCloseDisplay(display);
#endif
		return text;
	}
}