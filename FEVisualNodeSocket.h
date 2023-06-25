#pragma once

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
	bool bOutput = false;
	std::string Type;
	std::string Name;
	FEVisualNode* Parent = nullptr;
	std::vector<FEVisualNodeSocket*> Connections;

	ImColor* ForceColor = nullptr;
public:
	FEVisualNodeSocket(FEVisualNode* Parent, std::string Type, std::string Name, bool bOutput = false);

	FEVisualNode* GetParent() const;
	std::vector<FEVisualNodeSocket*> GetConnections();

	std::string GetID();
	std::string GetName();

	std::string GetType() const;

	bool GetForcedConnectionColor(ImColor& Color) const;
	void SetForcedConnectionColor(ImColor* NewValue);

	bool isOutput() const { return bOutput; }
	bool isInput() const { return !bOutput; }
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