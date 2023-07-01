#include "VisualNodeFactory.h"

VisualNodeFactory* VisualNodeFactory::Instance = nullptr;

VisualNodeFactory::VisualNodeFactory() {}
VisualNodeFactory::~VisualNodeFactory() {}

void VisualNodeFactory::RegisterNodeType(const std::string& Type,
                                         std::function<VisualNode* ()> Constructor,
                                         std::function<VisualNode* (const VisualNode&)> CopyConstructor)
{
    Constructors[Type] = Constructor;
    CopyConstructors[Type] = CopyConstructor;
}

VisualNode* VisualNodeFactory::CreateNode(const std::string& Type) const
{
    auto it = Constructors.find(Type);
    if (it == Constructors.end())
        return nullptr;

    return it->second();
}

VisualNode* VisualNodeFactory::CopyNode(const std::string& Type, const VisualNode& Node) const
{
    auto it = CopyConstructors.find(Type);
    if (it == CopyConstructors.end())
        return nullptr;

    return it->second(Node);

    //auto it = CopyConstructors.find(Node.GetType());
    //if (it != CopyConstructors.end())
    //{
    //    return it->second(Node);
    //}
    ///*else
    //{
    //    throw std::runtime_error("Unknown node type: " + Node.GetType());
    //}*/

    //return nullptr;
}