#pragma once
#include "../../VisualNode.h"

namespace VisNodeSys
{
    class VISUAL_NODE_SYSTEM_API SocketMirrorNode : public Node
    {
        friend class NodeSystem;

        bool AddSocketInternal(std::vector<std::string> AllowedTypes, std::string Name = "", NodeSocket::SocketFlow FlowDirection = NodeSocket::SocketFlow::Input);
    protected:
        // Prevents cycle when working with socket modification, when NODE_SYSTEM will trigger modification of partner node's socket, which would trigger modification of this node's socket again.
        std::string SocketIDBeingModified = "";

		bool bHaveInput = false;
		bool bHaveOutput = false;

        virtual std::vector<Node*> GetMirrorPartners() const = 0;

        std::function<void* ()> CreateCrossAreaDataGetter(int SocketIndex);

        Json::Value ToJson();
        bool FromJson(Json::Value Json);

        SocketMirrorNode(const std::string ID = "");
        SocketMirrorNode(const SocketMirrorNode& Other);

        void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);

        virtual bool IsDangling() const;

        bool bInEditMode = false;
        static ImTextureID LinkIconTextureID;
        static ImTextureID BrokenLinkIconTextureID;
        static ImTextureID PlusIconTextureID;
        static ImTextureID EditIconTextureID;
        static ImTextureID TrashBinIconTextureID;
        static ImTextureID RenameIconTextureID;
        static ImTextureID ChangeAllowedTypesIconTextureID;
		static ImTextureID SubAreaIconTextureID;

        virtual void SetCorrectSize();
        void Draw();
    public:
        bool AddSocket(NodeSocket* Socket);
        bool AddSocket(std::vector<std::string> AllowedTypes, std::string Name = "", NodeSocket::SocketFlow FlowDirection = NodeSocket::SocketFlow::Input);

        bool DeleteSocket(std::string SocketID);
        bool DeleteSocket(NodeSocket* Socket);
    };
}