#include "NodeAreaGeneralTests.h"
using namespace VisNodeSys;

TEST(NodeAreaGeneralTests, GetNodeByID)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	Node* NewNode = new Node();
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);
	ASSERT_EQ(LocalNodeArea->GetNodeByID(NewNode->GetID()), nullptr);

	LocalNodeArea->AddNode(NewNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);
	ASSERT_EQ(LocalNodeArea->GetNodeByID(NewNode->GetID()), NewNode);
	
	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(NodeAreaGeneralTests, GetNodesByNameBasic)
{
	std::vector<Node*> FoundNodes;
	std::vector<Node*> NodesWithSameName;
	bool bFound = false;
	int FoundCounter = 0;

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	Node* NewNode = new Node();
	NewNode->SetName("TestNode");
	NodesWithSameName.push_back(NewNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);
	FoundNodes = LocalNodeArea->GetNodesByName("TestNode");
	ASSERT_EQ(FoundNodes.size(), 0);

	// Test that GetNodesByName returns correct node when there is one node with given name.
	LocalNodeArea->AddNode(NewNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);
	FoundNodes = LocalNodeArea->GetNodesByName("TestNode");
	ASSERT_EQ(FoundNodes.size(), 1);
	for (size_t i = 0; i < FoundNodes.size(); i++)
	{
		if (FoundNodes[i] == NewNode)
		{
			bFound = true;
			break;
		}
	}
	ASSERT_EQ(bFound, true);

	// Test that GetNodesByName returns empty vector if there is no node with given name.
	FoundNodes = LocalNodeArea->GetNodesByName("NotTestNode");
	ASSERT_EQ(FoundNodes.size(), 0);

	// Test that GetNodesByName returns all nodes with given name.
	NewNode = new Node();
	NewNode->SetName("TestNode");
	NodesWithSameName.push_back(NewNode);
	LocalNodeArea->AddNode(NewNode);

	FoundNodes = LocalNodeArea->GetNodesByName("TestNode");
	ASSERT_EQ(FoundNodes.size(), 2);

	bFound = false;
	if ((FoundNodes[0] == NodesWithSameName[0] || FoundNodes[1] == NodesWithSameName[0]) &&
		(FoundNodes[0] == NodesWithSameName[1] || FoundNodes[1] == NodesWithSameName[1]))
		bFound = true;
	
	ASSERT_EQ(bFound, true);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(NodeAreaGeneralTests, GetNodesByStringType)
{
	std::vector<std::string> NodesIDList;
	std::vector<std::string> GroupCommentsIDList;

	NodeArea* LocalNodeArea = TEST_TOOLS.CreateTinyPopulatedNodeArea(NodesIDList, GroupCommentsIDList);
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);

	auto CheckIDsInList = [&](std::vector<Node*> NodesToCheck) {
		std::vector<std::string> NodesToCheckIDList;
		for (size_t i = 0; i < NodesToCheck.size(); i++)
			NodesToCheckIDList.push_back(NodesToCheck[i]->GetID());

		ASSERT_EQ(TEST_TOOLS.IsFirstIDsListSubsetOfSecond(NodesToCheckIDList, NodesIDList), true);
	};

	std::vector<Node*> FoundNodes = LocalNodeArea->GetNodesByStringType("VisualNode");
	ASSERT_EQ(FoundNodes.size(), 5);
	CheckIDsInList(FoundNodes);

	FoundNodes = LocalNodeArea->GetNodesByStringType("CustomNode");
	ASSERT_EQ(FoundNodes.size(), 1);
	CheckIDsInList(FoundNodes);

	FoundNodes = LocalNodeArea->GetNodesByStringType("CustomNode2");
	ASSERT_EQ(FoundNodes.size(), 1);
	CheckIDsInList(FoundNodes);

	FoundNodes = LocalNodeArea->GetNodesByStringType("CustomNode3");
	ASSERT_EQ(FoundNodes.size(), 1);
	CheckIDsInList(FoundNodes);

	FoundNodes = LocalNodeArea->GetNodesByStringType("CustomNode4");
	ASSERT_EQ(FoundNodes.size(), 1);
	CheckIDsInList(FoundNodes);

	FoundNodes = LocalNodeArea->GetNodesByStringType("CustomNodeStyleDemonstration");
	ASSERT_EQ(FoundNodes.size(), 2);
	CheckIDsInList(FoundNodes);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(NodeAreaGeneralTests, GetNodesByType)
{
	std::vector<std::string> NodesIDList;
	std::vector<std::string> GroupCommentsIDList;

	NodeArea* LocalNodeArea = TEST_TOOLS.CreateTinyPopulatedNodeArea(NodesIDList, GroupCommentsIDList);
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);

	auto CheckIDsInList = [&](std::vector<Node*> NodesToCheck) {
		std::vector<std::string> NodesToCheckIDList;
		for (size_t i = 0; i < NodesToCheck.size(); i++)
			NodesToCheckIDList.push_back(NodesToCheck[i]->GetID());

		ASSERT_EQ(TEST_TOOLS.IsFirstIDsListSubsetOfSecond(NodesToCheckIDList, NodesIDList), true);
	};

	std::vector<Node*> FoundNodes = LocalNodeArea->GetNodesByType<Node>();
	ASSERT_EQ(FoundNodes.size(), 11);
	CheckIDsInList(FoundNodes);

	std::vector<CustomNode*> FoundCustomNodes = LocalNodeArea->GetNodesByType<CustomNode>();
	ASSERT_EQ(FoundCustomNodes.size(), 1);
	CheckIDsInList(std::vector<Node*>(FoundCustomNodes.begin(), FoundCustomNodes.end()));

	std::vector<CustomNode2*> FoundCustom2Nodes = LocalNodeArea->GetNodesByType<CustomNode2>();
	ASSERT_EQ(FoundCustom2Nodes.size(), 1);
	CheckIDsInList(std::vector<Node*>(FoundCustom2Nodes.begin(), FoundCustom2Nodes.end()));

	std::vector<CustomNode3*> FoundCustom3Nodes = LocalNodeArea->GetNodesByType<CustomNode3>();
	ASSERT_EQ(FoundCustom3Nodes.size(), 1);
	CheckIDsInList(std::vector<Node*>(FoundCustom3Nodes.begin(), FoundCustom3Nodes.end()));

	std::vector<CustomNode4*> FoundCustom4Nodes = LocalNodeArea->GetNodesByType<CustomNode4>();
	ASSERT_EQ(FoundCustom4Nodes.size(), 1);
	CheckIDsInList(std::vector<Node*>(FoundCustom4Nodes.begin(), FoundCustom4Nodes.end()));

	std::vector<CustomNodeStyleDemonstration*> FoundCustomNodeStyleDemonstrationNodes = LocalNodeArea->GetNodesByType<CustomNodeStyleDemonstration>();
	ASSERT_EQ(FoundCustomNodeStyleDemonstrationNodes.size(), 2);
	CheckIDsInList(std::vector<Node*>(FoundCustomNodeStyleDemonstrationNodes.begin(), FoundCustomNodeStyleDemonstrationNodes.end()));

	// Test that GetNodesByType returns empty vector if there is no node of given type.
	std::vector<CustomNode5*> FoundCustomNode5Nodes = LocalNodeArea->GetNodesByType<CustomNode5>();
	ASSERT_EQ(FoundCustomNode5Nodes.size(), 0);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}