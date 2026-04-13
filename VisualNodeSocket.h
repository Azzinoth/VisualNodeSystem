#pragma once

#include "VisualNodeCore.h"

namespace VisNodeSys
{
#define NODE_SOCKET_SIZE 5.0f
#define DEFAULT_NODE_SOCKET_COLOR ImColor(150, 150, 150)
#define DEFAULT_CONNECTION_COLOR ImColor(200, 200, 200)
#define DEFAULT_NODE_SOCKET_MOUSE_HOVERED_CONNECTION_COLOR ImColor(220, 220, 220)

	class Node;

	class VISUAL_NODE_SYSTEM_API NodeSocket
	{
	public:
		enum class SocketFlow : uint8_t
		{
			Input = 0,
			Output = 1
		};
	private:
		friend class NodeSystem;
		friend class NodeArea;
		friend Node;

		~NodeSocket() = default;

		std::string ID;
		SocketFlow Flow;
		std::vector<std::string> AllowedTypes;
		std::string Name;
		std::vector<NodeSocket*> ConnectedSockets;

		static std::unordered_map<std::string, ImColor> SocketTypeToColorAssociations;

		std::function<void* ()> OutputData = []() { return nullptr; };
	protected:
		Node* Parent = nullptr;
	public:
		NodeSocket(Node* Parent, std::string Type, std::string Name, SocketFlow Flow = SocketFlow::Input, std::function<void* ()> OutputDataFunction = []() { return nullptr; });
		NodeSocket(Node* Parent, std::vector<std::string> Types, std::string Name, SocketFlow Flow = SocketFlow::Input, std::function<void* ()> OutputDataFunction = []() { return nullptr; });
		
		Node* GetParent() const;
		const std::vector<NodeSocket*>& GetConnectedSockets() const;

		std::string GetID() const;

		std::string GetName() const;
		void SetName(std::string NewValue);

		SocketFlow GetFlowDirection() const;

		const std::vector<std::string>& GetAllowedTypes() const;
		// Returns true if change did not trigger disconnection of already connected sockets.
		bool SetAllowedTypes(std::vector<std::string> NewTypes);

		void SetFunctionToOutputData(std::function<void* ()> NewFunction);
		void* GetData();
	};

	inline NodeSocket::SocketFlow operator!(NodeSocket::SocketFlow Direction)
	{
		return Direction == NodeSocket::SocketFlow::Input ? NodeSocket::SocketFlow::Output : NodeSocket::SocketFlow::Input;
	}

	struct ConnectionStyle
	{
		ImColor ForceColor = DEFAULT_CONNECTION_COLOR;

		bool bMarchingAntsEffect = false;
		float MarchingAntsSpeed = 1.0f;
		bool bMarchingAntsReverseDirection = false;

		bool bPulseEffect = false;
		float PulseSpeed = 1.0f;
		float PulseMin = 0.1f;
		float PulseMax = 1.0f;
	};

	class NodeArea;
	class Connection;

	class RerouteNode
	{
		friend class NodeSystem;
		friend class NodeArea;
		friend class Connection;
		friend Node;

		std::string ID;
		Connection* Parent = nullptr;
		ImVec2 Position;

		NodeSocket* BeginSocket = nullptr;
		RerouteNode* BeginReroute = nullptr;

		NodeSocket* EndSocket = nullptr;
		RerouteNode* EndReroute = nullptr;

		bool bHovered = false;
		bool bSelected = false;

		RerouteNode() {};
	};

	struct ConnectionSegment
	{
		ImVec2 Begin;
		ImVec2 End;

		NodeSocket* BeginSocket = nullptr;
		RerouteNode* BeginReroute = nullptr;

		NodeSocket* EndSocket = nullptr;
		RerouteNode* EndReroute = nullptr;
	};

	class Connection
	{
		friend class NodeSystem;
		friend class NodeArea;
		friend Node;

		NodeSocket* Out = nullptr;
		NodeSocket* In = nullptr;

		bool bHovered = false;
		bool bSelected = false;
		ConnectionStyle Style;

		std::vector<RerouteNode*> RerouteNodes;

		Connection(NodeSocket* Out, NodeSocket* In);
		~Connection();
	};
}