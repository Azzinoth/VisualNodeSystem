#include "NodeFactoryTests.h"
using namespace VisNodeSys;

TEST(NodeFactory, CreateKnownNodes)
{
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	std::vector<std::string> KnownNodesTypes = {
		"CustomNode",
		"CustomNode2",
		"CustomNode3",
		"CustomNode4",
		"CustomNodeStyleDemonstration",
	};

	for (int i = 0; i < KnownNodesTypes.size(); i++)
	{
		VisNodeSys::Node* CreatedNode = NODE_FACTORY.CreateNode(KnownNodesTypes[i]);
		ASSERT_NE(CreatedNode, nullptr);
		NodeArea->AddNode(CreatedNode);
	}

	ASSERT_EQ(NodeArea->GetNodeCount(), KnownNodesTypes.size());
	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeFactory, CreateUnknownNodes)
{
	VisNodeSys::Node* UnknownNode = NODE_FACTORY.CreateNode("UnknownNode");
	ASSERT_EQ(UnknownNode, nullptr);

	VisNodeSys::Node* EmptyNode = NODE_FACTORY.CreateNode("");
	ASSERT_EQ(EmptyNode, nullptr);
}

TEST(NodeFactory, CopyKnownNodes)
{
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	VisNodeSys::Node* CreatedNode = NODE_FACTORY.CreateNode("CustomNode");
	NodeArea->AddNode(CreatedNode);
	ASSERT_NE(CreatedNode, nullptr);
	CustomNode* CustomNodeInstance = dynamic_cast<CustomNode*>(CreatedNode);
	ASSERT_EQ(CustomNodeInstance->GetData(), 0.0f);
	CustomNodeInstance->SetData(1.0f);
	ASSERT_EQ(CustomNodeInstance->GetData(), 1.0f);

	VisNodeSys::Node* CopiedNode = NODE_FACTORY.CopyNode("CustomNode", *CreatedNode);
	NodeArea->AddNode(CopiedNode);
	ASSERT_NE(CopiedNode, nullptr);
	CustomNode* CustomNodeCopiedInstance = dynamic_cast<CustomNode*>(CopiedNode);
	ASSERT_EQ(CustomNodeCopiedInstance->GetData(), 1.0f);

	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	NodeArea->DeleteNode(CreatedNode);
	NodeArea->DeleteNode(CopiedNode);
	// Here it is same count because CustomNode is non-destructable.
	ASSERT_EQ(NodeArea->GetNodeCount(), 2);

	CreatedNode = NODE_FACTORY.CreateNode("CustomNode2");
	ASSERT_NE(CreatedNode, nullptr);
	NodeArea->AddNode(CreatedNode);
	CustomNode2* CustomNode2Instance = dynamic_cast<CustomNode2*>(CreatedNode);
	ASSERT_EQ(CustomNode2Instance->GetData(), 0.0f);
	CustomNode2Instance->SetData(1.0f);
	ASSERT_EQ(CustomNode2Instance->GetData(), 1.0f);
	CustomNode2Instance->SetStringData("Test");
	ASSERT_EQ(CustomNode2Instance->GetStringData(), "Test");

	CopiedNode = NODE_FACTORY.CopyNode("CustomNode2", *CreatedNode);
	ASSERT_NE(CopiedNode, nullptr);
	NodeArea->AddNode(CopiedNode);
	CustomNode2* CustomNode2CopiedInstance = dynamic_cast<CustomNode2*>(CopiedNode);
	ASSERT_EQ(CustomNode2CopiedInstance->GetData(), 1.0f);
	ASSERT_EQ(CustomNode2CopiedInstance->GetStringData(), "Test");

	ASSERT_EQ(NodeArea->GetNodeCount(), 4);
	NodeArea->DeleteNode(CreatedNode);
	NodeArea->DeleteNode(CopiedNode);
	ASSERT_EQ(NodeArea->GetNodeCount(), 2);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeFactory, CopyUnknownNodes)
{
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	VisNodeSys::Node* CreatedNode = NODE_FACTORY.CreateNode("CustomNode");
	ASSERT_NE(CreatedNode, nullptr);
	NodeArea->AddNode(CreatedNode);

	VisNodeSys::Node* CopiedNode = NODE_FACTORY.CopyNode("UnknownNode", *CreatedNode);
	ASSERT_EQ(CopiedNode, nullptr);

	CopiedNode = NODE_FACTORY.CopyNode("", *CreatedNode);
	ASSERT_EQ(CopiedNode, nullptr);

	NodeArea->DeleteNode(CreatedNode);
}

TEST(NodeFactory, IncorrectRegistration)
{
	bool Result = NODE_FACTORY.RegisterNodeType("CustomNode",
		[]() -> Node* {
			return new CustomNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const CustomNode& NodeToCopy = static_cast<const CustomNode&>(CurrentNode);
			return new CustomNode(NodeToCopy);
		}
	);
	ASSERT_FALSE(Result);

	Result = NODE_FACTORY.RegisterNodeType("CustomNode",
		nullptr,
		[](const Node& CurrentNode) -> Node* {
			const CustomNode& NodeToCopy = static_cast<const CustomNode&>(CurrentNode);
			return new CustomNode(NodeToCopy);
		}
	);
	ASSERT_FALSE(Result);

	Result = NODE_FACTORY.RegisterNodeType("CustomNode",
		[]() -> Node* {
			return new CustomNode();
		},
		nullptr
	);
	ASSERT_FALSE(Result);

	Result = NODE_FACTORY.RegisterNodeType("CustomNode",
		nullptr,
		nullptr
	);
	ASSERT_FALSE(Result);
}