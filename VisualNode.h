#pragma once

#include "VisualNodeSocket.h"

namespace VisNodeSys
{
	enum NODE_STYLE
	{
		DEFAULT = 0,
		CIRCLE = 1
	};

#define NODE_NAME_MAX_LENGTH 1024
#define NODE_TITLE_HEIGHT 30.0f
#define NODE_DIAMETER 72.0f

	enum NODE_SOCKET_EVENT
	{
		CONNECTED = 0,
		DISCONNECTED = 1,
		DESTRUCTION = 2,
		UPDATE = 3,
		EXECUTE = 4
	};

	class VISUAL_NODE_SYSTEM_API Node
	{
	protected:
		friend class NodeFactory;
		friend class NodeSystem;
		friend class NodeArea;
		friend class LinkNode;

		virtual ~Node();

		NodeArea* ParentArea = nullptr;
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
		NodeSocket* GetSocketByIDInternal(std::string SocketID) const;

		ImVec2 LeftTop;
		ImVec2 RightBottom;

		ImColor TitleBackgroundColor = ImColor(120, 150, 25);
		ImColor TitleBackgroundColorHovered = ImColor(140, 190, 35);

		bool bHovered = false;
		void SetIsHovered(bool bNewValue);

		NODE_STYLE Style = DEFAULT;
		bool bRenderTitleBar = true;

		virtual void Draw();
		virtual bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser = nullptr);
		virtual void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);
		virtual bool OpenContextMenu();

		void UpdateClientRegion();

		static bool IsNodeWithIDInList(std::string ID, std::vector<Node*> List);

		void SetToDefaultState();
	public:
		Node(std::string ID = "");
		Node(const Node& Other);

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
		
		bool GetRenderTitleBar() const;
		void SetRenderTitleBar(bool bNewValue);

		virtual bool AddSocket(NodeSocket* Socket);
		virtual bool DeleteSocket(NodeSocket* Socket);
		virtual bool DeleteSocket(std::string SocketID);

		virtual Json::Value ToJson();
		virtual bool FromJson(Json::Value Json);

		const NodeSocket* GetSocketByID(std::string SocketID) const;
		size_t GetSocketIndexByID(std::string SocketID) const;

		const NodeSocket* GetSocketByIndex(size_t SocketIndex, bool bOutput) const;
		std::string GetSocketIDByIndex(size_t SocketIndex, bool bOutput) const;

		size_t GetInputSocketCount() const;
		std::vector<std::pair<size_t, std::vector<std::string>>> GetInputSocketTypes() const;
		std::vector<Node*> GetNodesConnectedToInput() const;

		size_t GetOutputSocketCount() const;
		std::vector<std::pair<size_t, std::vector<std::string>>> GetOutputSocketTypes() const;
		std::vector<Node*> GetNodesConnectedToOutput() const;

		NODE_STYLE GetStyle() const;
		void SetStyle(NODE_STYLE NewValue);

		bool IsHovered() const;

		bool CouldBeMoved() const;
		void SetCouldBeMoved(bool bNewValue);

		bool CouldBeDestroyed() const;

		NodeArea* GetParentArea() const;
	};
}