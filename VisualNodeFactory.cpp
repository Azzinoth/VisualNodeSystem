#include "VisualNodeFactory.h"
using namespace VisNodeSys;

#ifdef VISUAL_NODE_SYSTEM_SHARED
extern "C" __declspec(dllexport) void* GetNodeFactory()
{
    return NodeFactory::GetInstancePointer();
}
#endif

NodeFactory::NodeFactory() {}
NodeFactory::~NodeFactory() {}

bool NodeFactory::RegisterNodeType(const std::string& Type,
                                         std::function<Node* ()> Constructor,
                                         std::function<Node* (const Node&)> CopyConstructor)
{
	if (Type.empty() || Constructor == nullptr || CopyConstructor == nullptr)
		return false;

	auto ConstructorIterator = Constructors.find(Type);
    if (ConstructorIterator != Constructors.end())
		return false;
	
    Constructors[Type] = Constructor;
    CopyConstructors[Type] = CopyConstructor;

	return true;
}

Node* NodeFactory::CreateNode(const std::string& Type) const
{
    auto ConstructorIterator = Constructors.find(Type);
    if (ConstructorIterator == Constructors.end())
        return nullptr;

    return ConstructorIterator->second();
}

Node* NodeFactory::CopyNode(const std::string& Type, const Node& Node) const
{
    auto CopyConstructorIterator = CopyConstructors.find(Type);
    if (CopyConstructorIterator == CopyConstructors.end())
        return nullptr;

    return CopyConstructorIterator->second(Node);
}