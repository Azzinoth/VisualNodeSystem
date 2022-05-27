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

enum FEEditorNodeSocketType
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

class FEEditorNode;
class FEEditorNodeSocket;
class FEEditorNodeArea;
class FEEditorNodeSystem;

class FEEditorNodeSocket
{
	friend FEEditorNodeSystem;
	friend FEEditorNodeArea;
	friend FEEditorNode;

	std::string ID;
	std::string name;
	FEEditorNode* parent = nullptr;
	std::vector<FEEditorNodeSocket*> connections;
	FEEditorNodeSocketType type;

	ImColor* forceColor = nullptr;
public:
	FEEditorNodeSocket(FEEditorNode* parent, FEEditorNodeSocketType type, std::string name);

	FEEditorNode* getParent();
	std::vector<FEEditorNodeSocket*> getConnections();

	std::string getID();
	std::string getName();

	FEEditorNodeSocketType getType();

	bool getForcedConnectionColor(ImColor& color);
	void setForcedConnectionColor(ImColor* newValue);
};

class FEEditorNodeConnection
{
	friend FEEditorNodeSystem;
	friend FEEditorNodeArea;
	friend FEEditorNode;

	FEEditorNodeSocket* out = nullptr;
	FEEditorNodeSocket* in = nullptr;

	FEEditorNodeConnection(FEEditorNodeSocket* out, FEEditorNodeSocket* in);
};