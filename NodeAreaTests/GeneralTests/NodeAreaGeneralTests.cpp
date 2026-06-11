#include "NodeAreaGeneralTests.h"
using namespace VisNodeSys;

TEST(NodeAreaGeneralTests, GetNodeByID)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	Node* NewNode = new Node();
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);
	ASSERT_EQ(LocalNodeArea->GetNodeByID(NewNode->GetID()), nullptr);

	LocalNodeArea->AddNode(NewNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);
	ASSERT_EQ(LocalNodeArea->GetNodeByID(NewNode->GetID()), NewNode);
	
	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, GetNodesByNameBasic)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, GetNodesByStringType)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, GetNodesByType)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, TryToDisconnect_WithNonExistentSocketIDs)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "TYPE_A", "in", NodeSocket::SocketFlow::Input));
	Area->AddNode(NodeB);
	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_EQ(Area->GetConnectionCount(), 1);

	EXPECT_FALSE(Area->TryToDisconnect(NodeA, std::string("nonexistent_out"), NodeB, std::string("nonexistent_in")));
	EXPECT_EQ(Area->GetConnectionCount(), 1);
	EXPECT_TRUE(Area->IsConnected(NodeA, NodeB));
	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, IsConnected_WithNonExistentSocketIDs)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "TYPE_A", "in", NodeSocket::SocketFlow::Input));
	Area->AddNode(NodeB);
	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_EQ(Area->GetConnectionCount(), 1);

	EXPECT_FALSE(Area->IsConnected(NodeA, std::string("nonexistent_out"), NodeB, std::string("nonexistent_in")));
	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, AddRerouteNodeToConnection_WithNonExistentSocketIDs)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "TYPE_A", "in", NodeSocket::SocketFlow::Input));
	Area->AddNode(NodeB);
	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_EQ(Area->GetConnectionCount(), 1);

	EXPECT_FALSE(Area->AddRerouteNodeToConnection(NodeA, std::string("nonexistent_out"), NodeB, std::string("nonexistent_in"), 0, ImVec2(50.0f, 50.0f)));
	EXPECT_EQ(Area->GetRerouteConnectionCount(), 0);
	NODE_SYSTEM.Clear();
}


TEST(NodeAreaGeneralTests, IsConnected_WithReversedDirectionSocketIDs_ReturnsFalse)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "IN_A", NodeSocket::SocketFlow::Input));
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "OUT_A", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "IN_B", NodeSocket::SocketFlow::Input));
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "OUT_B", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeB);

	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_EQ(Area->GetConnectionCount(), 1);

	std::string NodeAInSocketID = NodeA->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	std::string NodeAOutSocketID = NodeA->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output);
	std::string NodeBInSocketID = NodeB->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	std::string NodeBOutSocketID = NodeB->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output);

	EXPECT_TRUE(Area->IsConnected(NodeA, NodeAOutSocketID, NodeB, NodeBInSocketID));
	// Socket IDs are passed in reversed direction, it should return false.
	EXPECT_FALSE(Area->IsConnected(NodeA, NodeAInSocketID, NodeB, NodeBOutSocketID));

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, TryToDisconnect_WithReversedDirectionSocketIDs_DoesNotDisconnect)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "in_A", NodeSocket::SocketFlow::Input));
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "out_A", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "in_B", NodeSocket::SocketFlow::Input));
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "out_B", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeB);

	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_EQ(Area->GetConnectionCount(), 1);

	std::string NodeAInSocketID = NodeA->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	std::string NodeBOutSocketID = NodeB->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output);

	// Socket IDs are passed in reversed direction, it should return false.
	EXPECT_FALSE(Area->TryToDisconnect(NodeA, NodeAInSocketID, NodeB, NodeBOutSocketID));
	EXPECT_EQ(Area->GetConnectionCount(), 1);
	EXPECT_TRUE(Area->IsConnected(NodeA, NodeB));

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, TryToConnect_WithReversedDirectionSocketIDs_ReturnsFalse)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "in_A", NodeSocket::SocketFlow::Input));
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "out_A", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "in_B", NodeSocket::SocketFlow::Input));
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "out_B", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeB);

	ASSERT_EQ(Area->GetConnectionCount(), 0);

	std::string NodeAInSocketID = NodeA->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	std::string NodeBOutSocketID = NodeB->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output);

	// Socket IDs are passed in reversed direction, it should return false.
	EXPECT_FALSE(Area->TryToConnect(NodeA, NodeAInSocketID, NodeB, NodeBOutSocketID));
	EXPECT_EQ(Area->GetConnectionCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, AddObjects_Twice_IsRejected)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	EXPECT_TRUE(Area->AddNode(NodeA));
	EXPECT_FALSE(Area->AddNode(NodeA));
	ASSERT_EQ(Area->GetNodeCount(), 1);

	GroupComment* GroupCommentA = new GroupComment();
	EXPECT_TRUE(Area->AddGroupComment(GroupCommentA));
	EXPECT_FALSE(Area->AddGroupComment(GroupCommentA));
	ASSERT_EQ(Area->GetGroupCommentCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, AddSelected_Reject_InAppropriateNode)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	AreaA->AddNode(NodeA);

	EXPECT_FALSE(AreaB->AddSelected(NodeA));
	EXPECT_EQ(AreaB->GetSelected().size(), 0);

	Node* NodeB = new Node();
	EXPECT_FALSE(AreaB->AddSelected(NodeB));
	EXPECT_EQ(AreaB->GetSelected().size(), 0);
	
	// Destructor is private, so we need to add nodes to an area to be able to delete them.
	AreaB->AddNode(NodeB);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, Delete_SelectedNode_IsRemovedFromSelection)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	Node* NodeA = new Node();
	Area->AddNode(NodeA);

	ASSERT_TRUE(Area->AddSelected(NodeA));
	ASSERT_EQ(Area->GetSelected().size(), 1);

	Area->Delete(NodeA);
	EXPECT_EQ(Area->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, AddSelected_Reject_InAppropriateGroupComment)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();

	GroupComment* NewGroupComment = new GroupComment();
	AreaA->AddGroupComment(NewGroupComment);
	ASSERT_EQ(AreaA->GetGroupCommentCount(), 1);
	ASSERT_EQ(AreaB->GetGroupCommentCount(), 0);

	EXPECT_FALSE(AreaB->AddSelected(NewGroupComment));
	EXPECT_FALSE(AreaB->IsSelected(NewGroupComment));

	GroupComment* AnotherNewGroupComment = new GroupComment();
	EXPECT_FALSE(AreaA->AddSelected(AnotherNewGroupComment));
	EXPECT_FALSE(AreaB->IsSelected(AnotherNewGroupComment));

	delete AnotherNewGroupComment;
	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, Delete_GroupComment_IsRemovedFromSelection)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	GroupComment* NewGroupComment = new GroupComment();
	Area->AddGroupComment(NewGroupComment);

	ASSERT_TRUE(Area->AddSelected(NewGroupComment));
	EXPECT_TRUE(Area->IsSelected(NewGroupComment));

	Area->Delete(NewGroupComment);
	EXPECT_FALSE(Area->IsSelected(NewGroupComment));

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, AddNode_RejectNode_AlreadyInAnotherArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(AreaA, nullptr);
	ASSERT_NE(AreaB, nullptr);

	Node* TestNode = new Node();
	AreaA->AddNode(TestNode);
	ASSERT_EQ(AreaA->GetNodeCount(), 1);
	ASSERT_EQ(AreaB->GetNodeCount(), 0);
	ASSERT_EQ(TestNode->GetParentArea(), AreaA);

	EXPECT_FALSE(AreaB->AddNode(TestNode));

	ASSERT_EQ(AreaA->GetNodeByID(TestNode->GetID()), TestNode);
	ASSERT_EQ(AreaB->GetNodeByID(TestNode->GetID()), nullptr);

	ASSERT_EQ(TestNode->GetParentArea(), AreaA);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, AddGroupComment_RejectGroupComment_AlreadyInAnotherArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(AreaA, nullptr);
	ASSERT_NE(AreaB, nullptr);

	GroupComment* TestGroupComment = new GroupComment();
	AreaA->AddGroupComment(TestGroupComment);
	ASSERT_EQ(AreaA->GetGroupCommentCount(), 1);
	ASSERT_EQ(AreaB->GetGroupCommentCount(), 0);
	ASSERT_EQ(TestGroupComment->GetParentArea(), AreaA);

	EXPECT_FALSE(AreaB->AddGroupComment(TestGroupComment));

	ASSERT_EQ(AreaA->GetGroupCommentByID(TestGroupComment->GetID()), TestGroupComment);
	ASSERT_EQ(AreaB->GetGroupCommentByID(TestGroupComment->GetID()), nullptr);

	ASSERT_EQ(TestGroupComment->GetParentArea(), AreaA);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, AddSelected_Reject_InAppropriateRerouteNode)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(AreaA, nullptr);
	ASSERT_NE(AreaB, nullptr);

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	AreaA->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "TYPE_A", "in", NodeSocket::SocketFlow::Input));
	AreaA->AddNode(NodeB);

	ASSERT_TRUE(AreaA->TryToConnect(NodeA, 0, NodeB, 0));
	RerouteNode* CrossAreaRerouteNode = AreaA->AddRerouteNodeToConnection(NodeA, 0, NodeB, 0, 0, ImVec2(100.0f, 100.0f));
	ASSERT_NE(CrossAreaRerouteNode, nullptr);
	ASSERT_EQ(AreaA->GetRerouteConnectionCount(), 1);

	EXPECT_FALSE(AreaB->AddSelected(CrossAreaRerouteNode));
	EXPECT_FALSE(AreaB->IsSelected(CrossAreaRerouteNode));
	EXPECT_FALSE(AreaA->IsSelected(CrossAreaRerouteNode));

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, Delete_Reject_InAppropriateRerouteNode)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(AreaA, nullptr);
	ASSERT_NE(AreaB, nullptr);

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	AreaA->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "TYPE_A", "in", NodeSocket::SocketFlow::Input));
	AreaA->AddNode(NodeB);

	ASSERT_TRUE(AreaA->TryToConnect(NodeA, 0, NodeB, 0));
	RerouteNode* CrossAreaRerouteNode = AreaA->AddRerouteNodeToConnection(NodeA, 0, NodeB, 0, 0, ImVec2(100.0f, 100.0f));
	ASSERT_NE(CrossAreaRerouteNode, nullptr);
	ASSERT_EQ(AreaA->GetRerouteConnectionCount(), 1);

	EXPECT_FALSE(AreaB->DeleteRerouteNodeByID(CrossAreaRerouteNode->GetID()));
	EXPECT_EQ(AreaA->GetRerouteConnectionCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, DeleteRerouteNodeByID_ValidStaleAndUnknownIDs)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "TYPE_A", "in", NodeSocket::SocketFlow::Input));
	Area->AddNode(NodeB);

	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0));

	// Three reroutes so the deletion exercises the middle-of-chain relink.
	RerouteNode* FirstReroute = Area->AddRerouteNodeToConnection(NodeA, 0, NodeB, 0, 0, ImVec2(100.0f, 100.0f));
	ASSERT_NE(FirstReroute, nullptr);
	RerouteNode* MiddleReroute = Area->AddRerouteNodeToConnection(NodeA, 0, NodeB, 0, 1, ImVec2(200.0f, 100.0f));
	ASSERT_NE(MiddleReroute, nullptr);
	RerouteNode* LastReroute = Area->AddRerouteNodeToConnection(NodeA, 0, NodeB, 0, 2, ImVec2(300.0f, 100.0f));
	ASSERT_NE(LastReroute, nullptr);
	ASSERT_EQ(Area->GetRerouteConnectionCount(), 3);

	const std::string MiddleRerouteID = MiddleReroute->GetID();

	EXPECT_TRUE(Area->DeleteRerouteNodeByID(MiddleRerouteID));
	EXPECT_EQ(Area->GetRerouteConnectionCount(), 2);

	// The same ID is now stale, the lookup fails and nothing is freed or relinked twice.
	EXPECT_FALSE(Area->DeleteRerouteNodeByID(MiddleRerouteID));
	EXPECT_EQ(Area->GetRerouteConnectionCount(), 2);

	// Unknown IDs are rejected the same way.
	EXPECT_FALSE(Area->DeleteRerouteNodeByID("DOES_NOT_EXIST"));
	EXPECT_EQ(Area->GetRerouteConnectionCount(), 2);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, SocketToPosition_InvalidInputs_DoNotCrash)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Area->SocketToPosition(nullptr, "any");
	Area->SocketToPosition(NodeA, "nonexistent_socket_id");
	Area->SocketToPosition(NodeA, "");

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGeneralTests, SaveNodesToFile_OpenFailure_DoesNotLeakArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);
	Node* NodeToSave = new Node();
	ASSERT_TRUE(Area->AddNode(NodeToSave));

	const size_t AreaCountBefore = NODE_SYSTEM.GetNodeAreaCount();

	// Path inside a non-existent directory, so std::ofstream::open fails.
	const std::string UnwritablePath = "__no_such_dir__/SaveNodesToFile_OpenFailure.json";
	EXPECT_FALSE(Area->SaveNodesToFile(UnwritablePath, std::vector<Node*>{ NodeToSave }));

	// No phantom area should remain registered after the failed save.
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), AreaCountBefore);

	NODE_SYSTEM.Clear();
}