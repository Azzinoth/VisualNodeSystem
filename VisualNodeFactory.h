#pragma once

#include "VisualNode.h"

namespace VisNodeSys
{
    class VISUAL_NODE_SYSTEM_API NodeFactory
    {
        SINGLETON_PRIVATE_PART(NodeFactory)

        std::unordered_map<std::string, std::function<Node* ()>> Constructors;
        std::unordered_map<std::string, std::function<Node* (const Node&)>> CopyConstructors;

    public:
        SINGLETON_PUBLIC_PART(NodeFactory)

        void RegisterNodeType(const std::string& Type, std::function<Node* ()> Constructor, std::function<Node* (const Node&)> CopyConstructor);
        Node* CreateNode(const std::string& Type) const;
        Node* CopyNode(const std::string& Type, const Node& Node) const;
    };

#ifdef VISUAL_NODE_SYSTEM_SHARED
    extern "C" __declspec(dllexport) void* GetNodeFactory();
    #define NODE_FACTORY (*static_cast<VisNodeSys::NodeFactory*>(VisNodeSys::GetNodeFactory()))
#else
    #define NODE_FACTORY VisNodeSys::NodeFactory::GetInstance()
#endif
}