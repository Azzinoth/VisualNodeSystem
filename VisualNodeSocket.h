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

class VisualNode;
class NodeSocket;
class VisualNodeArea;
class VisualNodeSystem;

struct VisualNodeConnectionStyle
{
	ImColor* ForceColor = nullptr;

	bool bMarchingAntsEffect = false;
	float MarchingAntsSpeed = 1.0f;
	bool bMarchingAntsReverseDirection = false;

	bool bPulseEffect = false;
	float PulseSpeed = 1.0f;
	float PulseMin = 0.1f;
	float PulseMax = 1.0f;
};

class NodeSocket
{
	friend VisualNodeSystem;
	friend VisualNodeArea;
	friend VisualNode;

	std::string ID;
	bool bOutput = false;
	std::string Type;
	std::string Name;
	std::vector<NodeSocket*> Connections;

	VisualNodeConnectionStyle ConnectionStyle;
	static std::unordered_map<std::string, ImColor> SocketTypeToColorAssosiations;

	std::function<void* ()> OutputData = []() { return nullptr; };
protected:
	VisualNode* Parent = nullptr;
public:
	NodeSocket(VisualNode* Parent, std::string Type, std::string Name, bool bOutput = false, std::function<void* ()> OutputDataFunction = []() { return nullptr; });

	VisualNode* GetParent() const;
	std::vector<NodeSocket*> GetConnections();

	std::string GetID() const;
	std::string GetName() const;

	std::string GetType() const;

	bool GetForcedConnectionColor(ImColor& Color) const;
	void SetForcedConnectionColor(ImColor* NewValue);

	bool isOutput() const { return bOutput; }
	bool isInput() const { return !bOutput; }
	
	void SetFunctionToOutputData(std::function<void* ()> NewFunction);
	void* GetData() { return OutputData(); }
};

class VisualNodeConnection
{
	friend VisualNodeSystem;
	friend VisualNodeArea;
	friend VisualNode;

	NodeSocket* Out = nullptr;
	NodeSocket* In = nullptr;

	VisualNodeConnection(NodeSocket* Out, NodeSocket* In);
};