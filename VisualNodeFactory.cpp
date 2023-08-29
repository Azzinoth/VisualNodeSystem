#include "VisualNodeFactory.h"
using namespace VisNodeSys;

NodeFactory* NodeFactory::Instance = nullptr;

NodeFactory::NodeFactory() {}
NodeFactory::~NodeFactory() {}

void NodeFactory::RegisterNodeType(const std::string& Type,
                                         std::function<Node* ()> Constructor,
                                         std::function<Node* (const Node&)> CopyConstructor)
{
    Constructors[Type] = Constructor;
    CopyConstructors[Type] = CopyConstructor;
}

Node* NodeFactory::CreateNode(const std::string& Type) const
{
    auto it = Constructors.find(Type);
    if (it == Constructors.end())
        return nullptr;

    return it->second();
}

Node* NodeFactory::CopyNode(const std::string& Type, const Node& Node) const
{
    auto it = CopyConstructors.find(Type);
    if (it == CopyConstructors.end())
        return nullptr;

    return it->second(Node);
}