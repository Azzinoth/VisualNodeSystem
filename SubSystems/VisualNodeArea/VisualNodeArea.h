#pragma once

#include "../../VisualNodeFactory.h"

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

public:
	VisualNodeArea();
	~VisualNodeArea();

	ImVec2 GetAreaPosition() const;
	void SetAreaPosition(ImVec2 NewValue);

	ImVec2 GetAreaSize() const;
	void SetAreaSize(ImVec2 NewValue);

	ImVec2 GetAreaRenderOffset() const;
	void SetAreaRenderOffset(ImVec2 Offset);

	float GetZoomFactor() const;
	void SetZoomFactor(float NewValue);

	VisualNode* GetHovered() const;
	std::vector<VisualNode*> GetSelected();

	std::vector<VisualNode*> GetNodesByName(std::string NodeName) const;
	std::vector<VisualNode*> GetNodesByType(std::string NodeType) const;

	bool IsMouseHovered() const;
	bool IsAreaFillingWindow();
	void SetIsAreaFillingWindow(bool NewValue);
	int GetNodeCount() const;

	void Update();
	void Clear();
	void Reset();
	void AddNode(VisualNode* NewNode);
	void DeleteNode(const VisualNode* Node);
	void SetMainContextMenuFunc(void(*Func)());
	void PropagateUpdateToConnectedNodes(const VisualNode* CallerNode) const;
	void SetNodeEventCallback(void(*Func)(VisualNode*, VISUAL_NODE_EVENT));
	std::string ToJson() const;
	void SaveToFile(const char* FileName) const;
	void LoadFromFile(const char* FileName);
	void SaveNodesToFile(const char* FileName, std::vector<VisualNode*> Nodes);
	void RunOnEachNode(void(*Func)(VisualNode*));
	void RunOnEachConnectedNode(VisualNode* StartNode, void(*Func)(VisualNode*));
	void ClearSelection();
	void GetAllNodesAABB(ImVec2& Min, ImVec2& Max) const;
	ImVec2 GetAllNodesAABBCenter() const;
	ImVec2 GetRenderedViewCenter() const;
	bool TryToConnect(const VisualNode* OutNode, size_t OutNodeSocketIndex, const VisualNode* InNode, size_t InNodeSocketIndex);
	bool TryToConnect(const VisualNode* OutNode, std::string OutSocketID, const VisualNode* InNode, std::string InSocketID);
	bool TriggerSocketEvent(NodeSocket* CallerNodeSocket, NodeSocket* TriggeredNodeSocket, VISUAL_NODE_SOCKET_EVENT EventType);
	bool TriggerOrphanSocketEvent(VisualNode* Node, VISUAL_NODE_SOCKET_EVENT EventType);

	static bool IsAlreadyConnected(NodeSocket* FirstSocket, NodeSocket* SecondSocket, const std::vector<VisualNodeConnection*>& Connections);
	static void VisualNodeArea::ProcessConnections(const std::vector<NodeSocket*>& Sockets,
												   std::unordered_map<NodeSocket*, NodeSocket*>& OldToNewSocket,
												   VisualNodeArea* TargetArea, size_t NodeShift, const std::vector<VisualNode*>& SourceNodes);
	static void CopyNodesInternal(const std::vector<VisualNode*>& SourceNodes, VisualNodeArea* TargetArea, const size_t NodeShift = 0);

	static VisualNodeArea* CreateNodeArea(std::vector<VisualNode*> Nodes);
	static VisualNodeArea* FromJson(std::string JsonText);
	static void CopyNodesTo(VisualNodeArea* SourceNodeArea, VisualNodeArea* TargetNodeArea);
	static bool IsNodeIDInList(std::string ID, std::vector<VisualNode*> List);
	static bool EmptyOrFilledByNulls(const std::vector<VisualNode*> Vector);

	bool VisualNodeArea::GetConnectionStyle(VisualNode* Node, bool bOutputSocket, size_t SocketIndex, VisualNodeConnectionStyle& Style) const;
	void VisualNodeArea::SetConnectionStyle(VisualNode* Node, bool bOutputSocket, size_t SocketIndex, VisualNodeConnectionStyle NewStyle);

private:
	struct SocketEvent
	{
		NodeSocket* TriggeredNodeSocket;
		NodeSocket* CallerNodeSocket;
		VISUAL_NODE_SOCKET_EVENT EventType;
	};

	// Background grid variables.
	float GRID_SIZE = 10000.0f;
	int BOLD_LINE_FREQUENCY = 10;
	float DEFAULT_LINE_WIDTH = 1;
	float BOLD_LINE_WIDTH = 3;

	float Zoom = 1.0f;
	void ApplyZoom(float Delta);
	float MAX_ZOOM_LEVEL = 5.0f;  // Max zoom 500%
	float MIN_ZOOM_LEVEL = 0.2f;  // Min zoom 20%
	float GetNodeSocketSize() const { return NODE_SOCKET_SIZE * Zoom; }
	float GetRerouteNodeSize() const { return NODE_SOCKET_SIZE * Zoom * 1.5f; }
	float GetNodeTitleHeight() const { return NODE_TITLE_HEIGHT * Zoom; }
	ImVec2 GetMouseDragDelta() const { return ImGui::GetMouseDragDelta(0) * Zoom; }
	ImVec2 GetMouseDelta() const { return ImGui::GetIO().MouseDelta / Zoom; }
	float GetConnectionThickness() const { return 3.0f * Zoom; }

	static ImVec2 NeededShift;

	bool bClearing = false;
	bool bFillWindow = false;
	bool bMouseHovered = false;
	ImDrawList* CurrentDrawList = nullptr;
	ImGuiWindow* NodeAreaWindow = nullptr;
	std::vector<VisualNode*> Nodes;

	VisualNode* HoveredNode = nullptr;
	NodeSocket* SocketLookingForConnection = nullptr;
	VisualNodeConnection* HoveredConnection = nullptr;
	NodeSocket* SocketHovered = nullptr;
	VisualNodeRerouteNode* RerouteNodeHovered = nullptr;
	
	std::vector<VisualNode*> SelectedNodes;
	std::vector<VisualNodeConnection*> SelectedConnections;
	std::vector<VisualNodeRerouteNode*> SelectedRerouteNodes;

	ImVec2 MouseCursorPosition;
	ImVec2 MouseCursorSize = ImVec2(1, 1);
	ImVec2 MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	ImVec2 MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);
	ImGuiWindow* MouseDownIn = nullptr;
	bool bOpenMainContextMenu = false;
	std::vector<VisualNodeConnection*> Connections;

	ImVec2 AreaPosition;
	ImVec2 AreaSize;
	ImU32 GridBackgroundColor = IM_COL32(32, 32, 32, 255);
	ImVec4 GridLinesColor = ImVec4(53.0f / 255.0f, 53.0f / 255.0f, 53.0f / 255.0f, 0.5f);
	ImVec4 GridBoldLinesColor = ImVec4(27.0f / 255.0f, 27.0f / 255.0f, 27.0f / 255.0f, 1.0f);
	ImVec2 RenderOffset = ImVec2(0.0, 0.0);
	int LineSegments = 16;
	float LineXTangentMagnitude = 80.0f * 2.0f;
	float LineYTangentMagnitude = 0.0f;
	void(*MainContextMenuFunc)() = nullptr;
	std::vector<void(*)(VisualNode*, VISUAL_NODE_EVENT)> NodeEventsCallbacks;
	std::queue<SocketEvent> SocketEventQueue;
	
	void PropagateNodeEventsCallbacks(VisualNode* Node, VISUAL_NODE_EVENT EventToPropagate) const;
	void ProcessSocketEventQueue();
	ImVec2 SocketToPosition(const NodeSocket* Socket) const;
	std::vector<VisualNodeConnection*> GetAllConnections(const NodeSocket* Socket) const;
	VisualNodeConnection* GetConnection(const NodeSocket* FirstSocket, const NodeSocket* SecondSocket) const;

	void Delete(VisualNodeConnection* Connection);
	void Delete(VisualNodeRerouteNode* RerouteNode);

	void InputUpdate();
	void MouseInputUpdate();

	void LeftMouseClick();
	void LeftMouseClickNodesUpdate();
	void LeftMouseClickConnectionsUpdate();
	void LeftMouseClickRerouteUpdate();

	void RightMouseClick();
	void RightMouseClickNodesUpdate();
	void RightMouseClickConnectionsUpdate();
	void RightMouseClickRerouteUpdate();

	void MouseDragging();
	void MouseDraggingNodesUpdate();
	void MouseDraggingConnectionsUpdate();
	void MouseDraggingRerouteUpdate();

	void KeyboardInputUpdate();
	void InputUpdateNode(VisualNode* Node);
	void InputUpdateSocket(NodeSocket* Socket);
	void MouseInputUpdateConnections();
	void InputUpdateReroute(VisualNodeRerouteNode* Reroute);

	bool AddSelected(VisualNode* Node);
	bool IsSelected(const VisualNode* Node) const;
	bool AddSelected(VisualNodeConnection* Connection);
	bool IsSelected(const VisualNodeConnection* Connection) const;
	bool UnSelect(const VisualNodeConnection* Connection);
	void UnSelectAllConnections();

	bool AddSelected(VisualNodeRerouteNode* RerouteNode);
	bool IsSelected(const VisualNodeRerouteNode* RerouteNode) const;
	bool UnSelect(const VisualNodeRerouteNode* RerouteNode);
	void UnSelectAllRerouteNodes();

	void ConnectionsDoubleMouseClick();
	std::vector<VisualNodeConnectionSegment> GetConnectionSegments(const VisualNodeConnection* Connection) const;
	bool IsMouseOverConnection(VisualNodeConnection* Connection, const int Steps, const float MaxDistance, ImVec2& CollisionPoint = ImVec2());
	bool IsMouseOverSegment(ImVec2 Begin, ImVec2 End, const int Steps, const float MaxDistance, ImVec2& CollisionPoint = ImVec2());
	bool IsPointInRegion(const ImVec2& Point, const ImVec2& RegionMin, const ImVec2& RegionMax);
	bool IsSegmentInRegion(ImVec2 Begin, ImVec2 End, const int Steps);
	bool IsConnectionInRegion(VisualNodeConnection* Connection, const int Steps);

	void Render();
	void RenderGrid(ImVec2 CurrentPosition) const;
	void RenderNode(VisualNode* Node) const;
	void RenderNodeSockets(const VisualNode* Node) const;
	void RenderNodeSocket(NodeSocket* Socket) const;
	std::vector<ImVec2> GetTangentsForLine(const ImVec2 P1, const ImVec2 P2) const;
	void DrawHermiteLine(ImVec2 P1, ImVec2 P2, int Steps, ImColor Color, const VisualNodeConnectionStyle* Style) const;
	void DrawHermiteLine(const ImVec2 P1, const ImVec2 P2, const int Steps, const ImColor Color, const float Thickness) const;
	void RenderConnection(const VisualNodeConnection* Connection) const;
	void RenderReroute(const VisualNodeRerouteNode* RerouteNode) const;
	VisualNodeConnectionStyle* GetConnectionStyle(const NodeSocket* ParticipantOfConnection) const;

	bool IsMouseRegionSelectionActive() const;

	ImVec2 ScreenToLocal(ImVec2 ScreenPosition) const;
	ImVec2 LocalToScreen(ImVec2 LocalPosition) const;
};