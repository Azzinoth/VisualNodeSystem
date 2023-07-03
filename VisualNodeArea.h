#pragma once

#include "VisualNodeFactory.h"

#define VISUAL_NODE_GRID_STEP 15.0f

class VisualNodeSystem;

// Type of events for general callbacks.
enum VISUAL_NODE_EVENT
{
	VISUAL_NODE_REMOVED = 0,
	VISUAL_NODE_DESTROYED = 1,
	VISUAL_NODE_BEFORE_CONNECTED = 2,
	VISUAL_NODE_AFTER_CONNECTED = 3,
	VISUAL_NODE_BEFORE_DISCONNECTED = 4,
	VISUAL_NODE_AFTER_DISCONNECTED = 5
};

class VisualNodeArea
{
	friend VisualNodeSystem;

	VisualNodeArea();
	~VisualNodeArea();

	bool bClearing = false;
	bool bFillWindow = false;
	ImDrawList* CurrentDrawList = nullptr;
	ImGuiWindow* NodeAreaWindow = nullptr;

	std::vector<VisualNode*> Nodes;
	VisualNode* Hovered;
	bool bMouseHovered = false;

	std::vector<VisualNode*> Selected;
	bool AddSelected(VisualNode* NewNode);
	bool IsSelected(const VisualNode* Node) const;

	ImVec2 MouseCursorPosition;
	ImVec2 MouseCursorSize = ImVec2(1, 1);
	ImVec2 MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	ImVec2 MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);
	ImGuiWindow* MouseDownIn = nullptr;

	void InputUpdateNode(VisualNode* Node);
	void InputUpdateSocket(NodeSocket* Socket);

	void RenderNode(VisualNode* Node) const;
	void RenderNodeSockets(const VisualNode* Node) const;
	void RenderNodeSocket(NodeSocket* Socket) const;
	bool bOpenMainContextMenu = false;

	void DrawHermiteLine(ImVec2 P1, ImVec2 P2, int Steps, ImColor Color, VisualNodeConnectionStyle* Style) const;
	void RenderConnection(const VisualNodeConnection* Connection) const;
	ImVec2 SocketToPosition(const NodeSocket* Socket) const;

	std::vector<VisualNodeConnection*> Connections;
	std::vector<VisualNodeConnection*> GetAllConnections(const NodeSocket* Socket) const;
	void Disconnect(VisualNodeConnection*& Connection);

	NodeSocket* SocketLookingForConnection = nullptr;
	NodeSocket* SocketHovered = nullptr;

	ImVec2 AreaPosition;
	ImVec2 AreaSize;
	ImU32 GridBackgroundColor = IM_COL32(32, 32, 32, 255);
	ImVec4 GridLinesColor = ImVec4(53.0f / 255.0f, 53.0f / 255.0f, 53.0f / 255.0f, 0.5f);
	ImVec4 GridBoldLinesColor = ImVec4(27.0f / 255.0f, 27.0f / 255.0f, 27.0f / 255.0f, 1.0f);
	ImVec2 RenderOffset = ImVec2(0.0, 0.0);

	void(*MainContextMenuFunc)() = nullptr;
	void InputUpdate();
	void Render();
	void RenderGrid(ImVec2 CurrentPosition) const;

	std::vector<void(*)(VisualNode*, VISUAL_NODE_EVENT)> NodeEventsCallbacks;
	void PropagateNodeEventsCallbacks(VisualNode* Node, VISUAL_NODE_EVENT EventToPropagate) const;

	static bool IsNodeIDInList(std::string ID, std::vector<VisualNode*> List);

	static bool EmptyOrFilledByNulls(const std::vector<VisualNode*> Vector)
	{
		for (size_t i = 0; i < Vector.size(); i++)
		{
			if (Vector[i] != nullptr)
				return false;
		}

		return true;
	}

	static ImVec2 NeededShift;
public:
	void Update();
	void Clear();
	void Reset();

	ImVec2 GetAreaPosition() const;
	void SetAreaPosition(ImVec2 NewValue);

	ImVec2 GetAreaSize() const;
	void SetAreaSize(ImVec2 NewValue);

	ImVec2 GetAreaRenderOffset() const;
	void SetAreaRenderOffset(ImVec2 Offset);

	void AddNode(VisualNode* NewNode);
	void DeleteNode(const VisualNode* Node);

	void SetMainContextMenuFunc(void(*Func)());

	void PropagateUpdateToConnectedNodes(const VisualNode* CallerNode) const;

	bool TryToConnect(const VisualNode* OutNode, size_t OutNodeSocketIndex, const VisualNode* InNode, size_t InNodeSocketIndex);
	bool TryToConnect(const VisualNode* OutNode, std::string OutSocketID, const VisualNode* InNode, std::string InSocketID);
	void SetNodeEventCallback(void(*Func)(VisualNode*, VISUAL_NODE_EVENT));

	std::string ToJson() const;
	void SaveToFile(const char* FileName) const;
	void LoadFromFile(const char* FileName);
	void SaveNodesToFile(const char* FileName, std::vector<VisualNode*> Nodes);

	void RunOnEachNode(void(*Func)(VisualNode*));
	void RunOnEachConnectedNode(VisualNode* StartNode, void(*Func)(VisualNode*));

	VisualNode* GetHovered() const;
	std::vector<VisualNode*> GetSelected();
	void ClearSelection();

	void GetAllNodesAABB(ImVec2& Min, ImVec2& Max) const;
	ImVec2 GetAllNodesAABBCenter() const;
	ImVec2 GetRenderedViewCenter() const;

	std::vector<VisualNode*> GetNodesByName(std::string NodeName) const;
	std::vector<VisualNode*> GetNodesByType(std::string NodeType) const;

	static VisualNodeArea* CreateNodeArea(std::vector<VisualNode*> Nodes);
	static VisualNodeArea* FromJson(std::string JsonText);
	static void CopyNodesTo(VisualNodeArea* SourceNodeArea, VisualNodeArea* TargetNodeArea);

	int GetNodeCount() const;

	bool IsMouseHovered() const;

	bool IsAreaFillingWindow();
	void SetIsAreaFillingWindow(bool NewValue);
};