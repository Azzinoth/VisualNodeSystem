#pragma once

#include "VisualNode.h"

namespace VisNodeSys
{
    class VISUAL_NODE_SYSTEM_API NodeFactory
    {
        SINGLETON_PRIVATE_PART(NodeFactory)

        friend class Node;

        std::unordered_map<std::string, std::function<Node* ()>> Constructors;
        std::unordered_map<std::string, std::function<Node* (const Node&)>> CopyConstructors;

        std::unordered_map<std::string, std::pair<size_t, size_t>> NodeClassNameToSocketCount;
        // Retrieves the expected number of input and output sockets for a given node type,
        // as defined by its current C++ class implementation.
        // It is used to validate loaded node data (e.g., from a JSON file)
        // If a node class's socket structure(number of inputs / outputs) has changed since a file was saved,
        // loading that file could lead to inconsistencies or crashes.
		// TO-DO: Implement a more robust NodeClass integrity check. With socket type validation and more.
        std::pair<size_t, size_t> GetSocketCount(std::string NodeClassName);
    public:
        SINGLETON_PUBLIC_PART(NodeFactory)

        bool RegisterNodeType(const std::string& Type, std::function<Node* ()> Constructor, std::function<Node* (const Node&)> CopyConstructor);
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