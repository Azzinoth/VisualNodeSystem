#pragma once

#include "VisualNode.h"

class VisualNode;

class VisualNodeFactory
{
    SINGLETON_PRIVATE_PART(VisualNodeFactory)

    std::unordered_map<std::string, std::function<VisualNode* ()>> Constructors;
    std::unordered_map<std::string, std::function<VisualNode* (const VisualNode&)>> CopyConstructors;

public:
    SINGLETON_PUBLIC_PART(VisualNodeFactory)

    void RegisterNodeType(const std::string& Type, std::function<VisualNode* ()> Constructor, std::function<VisualNode* (const VisualNode&)> CopyConstructor);
    VisualNode* CreateNode(const std::string& Type) const;
    VisualNode* CopyNode(const std::string& Type, const VisualNode& Node) const;
};

#define NODE_FACTORY VisualNodeFactory::getInstance()