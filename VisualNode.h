#pragma once

#include "VisualNodeSocket.h"

enum VISUAL_NODE_STYLE
{
	VISUAL_NODE_STYLE_DEFAULT = 0,
	VISUAL_NODE_STYLE_CIRCLE = 1
};

#define VISUAL_NODE_NAME_MAX_LENGHT 1024
#define NODE_TITLE_HEIGHT 30.0f
#define NODE_DIAMETER 72.0f

enum VISUAL_NODE_SOCKET_EVENT
{
	VISUAL_NODE_SOCKET_CONNECTED = 0,
	VISUAL_NODE_SOCKET_DISCONNECTED = 1,
	VISUAL_NODE_SOCKET_DESTRUCTION = 2,
	VISUAL_NODE_SOCKET_UPDATE = 3
};

class VisualNodeSystem;
class VisualNodeArea;
class NodeSocket;

class VisualNode
{
protected:
	friend VisualNodeSystem;
	friend VisualNodeArea;

	virtual ~VisualNode();

	VisualNodeArea* ParentArea = nullptr;
	std::string ID;
	ImVec2 Position;
	ImVec2 Size;

	ImVec2 ClientRegionMin;
	ImVec2 ClientRegionMax;

	std::string Name;
	std::string Type;
	bool bShouldBeDestroyed = false;
	bool bCouldBeDestroyed = true;
	bool bCouldBeMoved = true;

	std::vector<NodeSocket*> Input;
	std::vector<NodeSocket*> Output;

	ImVec2 LeftTop;
	ImVec2 RightBottom;

	ImColor TitleBackgroundColor = ImColor(120, 150, 25);
	ImColor TitleBackgroundColorHovered = ImColor(140, 190, 35);

	bool bHovered = false;
	void SetIsHovered(bool NewValue);

	VISUAL_NODE_STYLE Style = VISUAL_NODE_STYLE_DEFAULT;

	virtual void Draw();
	virtual bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser = nullptr);
	virtual void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);
	virtual bool OpenContextMenu();

	void UpdateClientRegion();
public:
	VisualNode(std::string ID = "");
	VisualNode(const VisualNode& Src);

	std::string GetID();

	ImVec2 GetPosition() const;
	void SetPosition(ImVec2 NewValue);

	ImVec2 GetSize() const;
	void SetSize(ImVec2 NewValue);

	ImVec2 GetClientRegionSize();
	ImVec2 GetClientRegionPosition();

	std::string GetName();
	void SetName(std::string NewValue);

	std::string GetType() const;

	void AddSocket(NodeSocket* Socket);

	virtual Json::Value ToJson();
	virtual void FromJson(Json::Value Json);

	bool VisualNode::GetSocketStyle(bool bOutputSocket, size_t SocketIndex, VisualNodeConnectionStyle& Style) const;
	void SetSocketStyle(bool bOutputSocket, size_t SocketIndex, VisualNodeConnectionStyle NewStyle);

	size_t InputSocketCount() const;
	size_t OutSocketCount() const;

	std::vector<VisualNode*> GetNodesConnectedToInput() const;
	std::vector<VisualNode*> GetNodesConnectedToOutput() const;

	VISUAL_NODE_STYLE GetStyle() const;
	void SetStyle(VISUAL_NODE_STYLE NewValue);

	bool IsHovered() const;

	bool CouldBeMoved() const;
	void SetCouldBeMoved(bool NewValue);
};