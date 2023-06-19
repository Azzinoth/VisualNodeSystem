#pragma once

#include "FEVisualNode.h"

#define FE_VISUAL_NODE_GRID_STEP 15.0f

class FEVisualNodeSystem;

// Type of events for general callbacks.
enum FE_VISUAL_NODE_EVENT
{
	FE_VISUAL_NODE_REMOVED = 0,
	FE_VISUAL_NODE_DESTROYED = 1,
	FE_VISUAL_NODE_BEFORE_CONNECTED = 2,
	FE_VISUAL_NODE_AFTER_CONNECTED = 3,
	FE_VISUAL_NODE_BEFORE_DISCONNECTED = 4,
	FE_VISUAL_NODE_AFTER_DISCONNECTED = 5
};

class FEVisualNodeArea
{
	friend FEVisualNodeSystem;

	FEVisualNodeArea();
	~FEVisualNodeArea();

	bool bClearing = false;
	bool bFillWindow = false;
	ImDrawList* CurrentDrawList = nullptr;
	ImGuiWindow* NodeAreaWindow = nullptr;

	std::vector<FEVisualNode*> Nodes;
	FEVisualNode* Hovered;
	bool bMouseHovered = false;

	std::vector<FEVisualNode*> Selected;
	bool AddSelected(FEVisualNode* NewNode);
	bool IsSelected(const FEVisualNode* Node) const;

	ImVec2 MouseCursorPosition;
	ImVec2 MouseCursorSize = ImVec2(1, 1);
	ImVec2 MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	ImVec2 MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);
	ImGuiWindow* MouseDownIn = nullptr;

	void InputUpdateNode(FEVisualNode* Node);
	void InputUpdateSocket(FEVisualNodeSocket* Socket);

	void RenderNode(FEVisualNode* Node) const;
	void RenderNodeSockets(const FEVisualNode* Node) const;
	void RenderNodeSocket(FEVisualNodeSocket* Socket) const;
	bool bOpenMainContextMenu = false;

	void DrawHermiteLine(ImVec2 P1, ImVec2 P2, int Steps, ImColor Color) const;
	void RenderConnection(const FEVisualNodeConnection* Connection) const;
	ImVec2 SocketToPosition(const FEVisualNodeSocket* Socket) const;

	std::vector<FEVisualNodeConnection*> Connections;
	std::vector<FEVisualNodeConnection*> GetAllConnections(const FEVisualNodeSocket* Socket) const;
	void Disconnect(FEVisualNodeConnection*& Connection);

	FEVisualNodeSocket* SocketLookingForConnection = nullptr;
	FEVisualNodeSocket* SocketHovered = nullptr;

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

	std::vector<void(*)(FEVisualNode*, FE_VISUAL_NODE_EVENT)> NodeEventsCallbacks;
	void PropagateNodeEventsCallbacks(FEVisualNode* Node, FE_VISUAL_NODE_EVENT EventToPropagate) const;

	static bool IsNodeIDInList(std::string ID, std::vector<FEVisualNode*> List);

	static bool EmptyOrFilledByNulls(const std::vector<FEVisualNode*> Vector)
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

	void AddNode(FEVisualNode* NewNode);
	void DeleteNode(const FEVisualNode* Node);

	void SetMainContextMenuFunc(void(*Func)());

	void PropagateUpdateToConnectedNodes(const FEVisualNode* CallerNode) const;

	bool TryToConnect(const FEVisualNode* OutNode, size_t OutNodeSocketIndex, const FEVisualNode* InNode, size_t InNodeSocketIndex);
	bool TryToConnect(const FEVisualNode* OutNode, std::string OutSocketID, const FEVisualNode* InNode, std::string InSocketID);
	void SetNodeEventCallback(void(*Func)(FEVisualNode*, FE_VISUAL_NODE_EVENT));

	std::string ToJson() const;
	void SaveToFile(const char* FileName) const;
	void LoadFromFile(const char* FileName);
	void SaveNodesToFile(const char* FileName, std::vector<FEVisualNode*> Nodes);

	void RunOnEachNode(void(*Func)(FEVisualNode*));
	void RunOnEachConnectedNode(FEVisualNode* StartNode, void(*Func)(FEVisualNode*));

	FEVisualNode* GetHovered() const;
	std::vector<FEVisualNode*> GetSelected();
	void ClearSelection();

	void GetAllNodesAABB(ImVec2& Min, ImVec2& Max) const;
	ImVec2 GetAllNodesAABBCenter() const;
	ImVec2 GetRenderedViewCenter() const;

	std::vector<FEVisualNode*> GetNodesByName(std::string NodeName) const;
	std::vector<FEVisualNode*> GetNodesByType(std::string NodeType) const;

	static FEVisualNodeArea* CreateNodeArea(std::vector<FEVisualNode*> Nodes);
	static FEVisualNodeArea* FromJson(std::string JsonText);
	static void CopyNodesTo(FEVisualNodeArea* SourceNodeArea, FEVisualNodeArea* TargetNodeArea);

	int GetNodeCount() const;

	bool IsMouseHovered() const;

	bool IsAreaFillingWindow();
	void SetIsAreaFillingWindow(bool NewValue);
};