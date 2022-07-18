#pragma once

// Part of https://github.com/Azzinoth/FEBasicApplication/
#include "FEBasicApplication.h"
using namespace FocalEngine;

#include <fstream>
#include <functional>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/trigonometric.hpp"
#include "glm/geometric.hpp"

#include "jsoncpp/json/json.h"

#define NODE_SOCKET_SIZE 5.0f

enum FE_VISUAL_NODE_SOCKET_TYPE
{
	FE_NODE_SOCKET_COLOR_CHANNEL_OUT = 0,
	FE_NODE_SOCKET_COLOR_CHANNEL_IN = 1,
	FE_NODE_SOCKET_FLOAT_CHANNEL_OUT = 2,
	FE_NODE_SOCKET_FLOAT_CHANNEL_IN = 3,
	FE_NODE_SOCKET_COLOR_RGB_CHANNEL_OUT = 4,
	FE_NODE_SOCKET_COLOR_RGB_CHANNEL_IN = 5,
	FE_NODE_SOCKET_COLOR_RGBA_CHANNEL_OUT = 6,
	FE_NODE_SOCKET_COLOR_RGBA_CHANNEL_IN = 7
};

class FEVisualNode;
class FEVisualNodeSocket;
class FEVisualNodeArea;
class FEVisualNodeSystem;

class FEVisualNodeSocket
{
	friend FEVisualNodeSystem;
	friend FEVisualNodeArea;
	friend FEVisualNode;

	std::string ID;
	std::string Name;
	FEVisualNode* Parent = nullptr;
	std::vector<FEVisualNodeSocket*> Connections;
	FE_VISUAL_NODE_SOCKET_TYPE Type;

	ImColor* ForceColor = nullptr;
public:
	FEVisualNodeSocket(FEVisualNode* Parent, FE_VISUAL_NODE_SOCKET_TYPE Type, std::string Name);

	FEVisualNode* GetParent() const;
	std::vector<FEVisualNodeSocket*> GetConnections();

	std::string GetID();
	std::string GetName();

	FE_VISUAL_NODE_SOCKET_TYPE GetType() const;

	bool GetForcedConnectionColor(ImColor& Color) const;
	void SetForcedConnectionColor(ImColor* NewValue);
};

class FEVisualNodeConnection
{
	friend FEVisualNodeSystem;
	friend FEVisualNodeArea;
	friend FEVisualNode;

	FEVisualNodeSocket* Out = nullptr;
	FEVisualNodeSocket* In = nullptr;

	FEVisualNodeConnection(FEVisualNodeSocket* Out, FEVisualNodeSocket* In);
};