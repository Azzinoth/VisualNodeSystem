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

enum FEVisualNodeSocketType
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
	std::string name;
	FEVisualNode* parent = nullptr;
	std::vector<FEVisualNodeSocket*> connections;
	FEVisualNodeSocketType type;

	ImColor* forceColor = nullptr;
public:
	FEVisualNodeSocket(FEVisualNode* parent, FEVisualNodeSocketType type, std::string name);

	FEVisualNode* getParent();
	std::vector<FEVisualNodeSocket*> getConnections();

	std::string getID();
	std::string getName();

	FEVisualNodeSocketType getType();

	bool getForcedConnectionColor(ImColor& color);
	void setForcedConnectionColor(ImColor* newValue);
};

class FEVisualNodeConnection
{
	friend FEVisualNodeSystem;
	friend FEVisualNodeArea;
	friend FEVisualNode;

	FEVisualNodeSocket* out = nullptr;
	FEVisualNodeSocket* in = nullptr;

	FEVisualNodeConnection(FEVisualNodeSocket* out, FEVisualNodeSocket* in);
};