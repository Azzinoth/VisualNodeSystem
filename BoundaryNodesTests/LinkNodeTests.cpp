#include "LinkNodeTests.h"
using namespace VisNodeSys;

TEST(LinkNodeTests, Basic_EstablishConnection)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	ASSERT_NE(UpstreamArea, nullptr);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	ASSERT_NE(DownstreamArea, nullptr);

	// Invalid inputs should not cause a link to be created, and should return false.
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas("", ""), false);
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, ""), false);
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas("", UpstreamAreaID), false);
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, UpstreamAreaID), false);
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(DownstreamAreaID, DownstreamAreaID), false);

	// Valid inputs should create a link and return true, and output the IDs of the nodes involved in the link.
	std::pair<std::string, std::string> LinkResult;
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult), true);
	ASSERT_NE(LinkResult.first, "");
	ASSERT_NE(LinkResult.second, "");

	std::vector<std::pair<std::string, std::string>> LinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(UpstreamAreaID, DownstreamAreaID);
	EXPECT_EQ(LinkingNodes.size(), 1);

	// GetLinkingNodesForAreas outputs the IDs of the nodes involved in the link, so they should match the IDs returned by LinkNodeAreas.
	EXPECT_EQ(LinkResult.first, LinkingNodes[0].first);
	EXPECT_EQ(LinkResult.second, LinkingNodes[0].second);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	EXPECT_EQ(DownstreamArea->GetNodeByID(LinkResult.second), nullptr);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, Basic_AddSockets)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	ASSERT_NE(UpstreamArea, nullptr);

	Node* ExecutionBeginNode = new BeginNode();
	ASSERT_NE(ExecutionBeginNode, nullptr);
	UpstreamArea->AddNode(ExecutionBeginNode);
	UpstreamArea->SetExecutionEntryNode(ExecutionBeginNode);
	UpstreamArea->SetSaveExecutedNodes(true);

	BoolLiteralNode* UpstreamBoolNode = new BoolLiteralNode();
	UpstreamBoolNode->SetName("UpstreamBoolNode");
	EXPECT_FALSE(UpstreamBoolNode->GetData());
	UpstreamBoolNode->SetData(true);
	EXPECT_TRUE(UpstreamBoolNode->GetData());
	UpstreamArea->AddNode(UpstreamBoolNode);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	ASSERT_NE(DownstreamArea, nullptr);
	DownstreamArea->SetSaveExecutedNodes(true);

	BoolVariableNode* DownstreamBoolNode = new BoolVariableNode();
	DownstreamBoolNode->SetName("DownstreamBoolNode");
	EXPECT_FALSE(DownstreamBoolNode->GetData());
	DownstreamArea->AddNode(DownstreamBoolNode);

	std::pair<std::string, std::string> LinkResult;
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult), true);
	ASSERT_NE(LinkResult.first, "");
	ASSERT_NE(LinkResult.second, "");
	LinkNode* UpstreamLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	EXPECT_TRUE(UpstreamLinkNode->AddSocket({ "BOOL" }));

	// Connect node to a parent link node.
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_EQ(UpstreamArea->TryToConnect(ExecutionBeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamBoolNode, 0, UpstreamLinkNode, 1), true);

	// Connect node to a child link node.
	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);
	ASSERT_NE(DownstreamLinkNode, nullptr);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamBoolNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamBoolNode, 1), true);

	EXPECT_FALSE(DownstreamBoolNode->GetData());
	ASSERT_EQ(UpstreamArea->ExecuteNodeNetwork(), true);
	EXPECT_TRUE(DownstreamBoolNode->GetData());
	ASSERT_EQ(UpstreamArea->GetLastExecutedNodes().size(), 2);
	ASSERT_EQ(DownstreamArea->GetLastExecutedNodes().size(), 2);

	// Try to add incompatible socket type to the link, should fail.
	EXPECT_FALSE(UpstreamLinkNode->AddSocket(new NodeSocket(UpstreamLinkNode, { std::string("FLOAT") }, "IncompatibleSocket", NodeSocket::SocketFlow::Output)));

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, MultipleSockets)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	BoolLiteralNode* UpstreamBoolNode = new BoolLiteralNode();
	UpstreamBoolNode->SetData(true);
	UpstreamArea->AddNode(UpstreamBoolNode);

	FloatLiteralNode* UpstreamFloatNode = new FloatLiteralNode();
	UpstreamFloatNode->SetData(4.2f);
	UpstreamArea->AddNode(UpstreamFloatNode);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	BoolVariableNode* DownstreamBoolNode = new BoolVariableNode();
	DownstreamBoolNode->SetData(false);
	DownstreamArea->AddNode(DownstreamBoolNode);

	FloatVariableNode* DownstreamFloatNode = new FloatVariableNode();
	DownstreamFloatNode->SetData(0.0f);
	DownstreamArea->AddNode(DownstreamFloatNode);

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));

	LinkNode* CurrentLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_TRUE(CurrentLinkNode->AddSocket({ "BOOL" }));
	ASSERT_TRUE(CurrentLinkNode->AddSocket({ "FLOAT" }));

	Node* UpstreamLinkNode = UpstreamArea->GetNodeByID(LinkResult.first);
	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);

	// Execution + Bool + Float on parent side.
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamBoolNode, 0, UpstreamLinkNode, 1), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamFloatNode, 0, UpstreamLinkNode, 2), true);

	// Execution to both child nodes, data from link.
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamBoolNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamBoolNode, 1), true);
	// Chain execution: BoolVar => FloatVar.
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamBoolNode, 0, DownstreamFloatNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 2, DownstreamFloatNode, 1), true);

	EXPECT_FALSE(DownstreamBoolNode->GetData());
	EXPECT_EQ(DownstreamFloatNode->GetData(), 0.0f);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_TRUE(DownstreamBoolNode->GetData());
	EXPECT_EQ(DownstreamFloatNode->GetData(), 4.2f);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, ArithmeticAcrossLink)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	IntegerLiteralNode* IntegerA = new IntegerLiteralNode();
	IntegerA->SetData(10);
	UpstreamArea->AddNode(IntegerA);

	IntegerLiteralNode* IntegerB = new IntegerLiteralNode();
	IntegerB->SetData(20);
	UpstreamArea->AddNode(IntegerB);

	ArithmeticAddNode* AddNode = new ArithmeticAddNode();
	UpstreamArea->AddNode(AddNode);

	// Wire arithmetic: IntegerA + IntegerB
	ASSERT_EQ(UpstreamArea->TryToConnect(IntegerA, 0, AddNode, 1), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(IntegerB, 0, AddNode, 2), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, AddNode, 0), true);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	IntegerVariableNode* ResultNode = new IntegerVariableNode();
	ResultNode->SetData(0);
	DownstreamArea->AddNode(ResultNode);

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* CurrentLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_TRUE(CurrentLinkNode->AddSocket({ "INT" }));

	Node* UpstreamLinkNode = UpstreamArea->GetNodeByID(LinkResult.first);
	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);

	// Chain: AddNode execution => LinkNode, AddNode result => LinkNode data.
	ASSERT_EQ(UpstreamArea->TryToConnect(AddNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(AddNode, 1, UpstreamLinkNode, 1), true);

	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, ResultNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, ResultNode, 1), true);

	EXPECT_EQ(ResultNode->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(ResultNode->GetData(), 30);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, SocketIndexRoutesDataCorrectly)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	IntegerVariableNode* IntegerA = new IntegerVariableNode();
	IntegerA->SetData(1);
	UpstreamArea->AddNode(IntegerA);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	IntegerVariableNode* IntegerB = new IntegerVariableNode();
	IntegerB->SetData(0);
	DownstreamArea->AddNode(IntegerB);

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* CurrentLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_TRUE(CurrentLinkNode->AddSocket({ "INT" }, "INT_0"));
	ASSERT_TRUE(CurrentLinkNode->AddSocket({ "INT" }, "INT_1"));

	Node* UpstreamLinkNode = UpstreamArea->GetNodeByID(LinkResult.first);
	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);

	// Connecting execution sockets.
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, IntegerB, 0), true);

	// Config 1: Mismatched indices. Upstream socket 2, downstream socket 1.
	ASSERT_EQ(UpstreamArea->TryToConnect(IntegerA, 1, UpstreamLinkNode, 2), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, IntegerB, 1), true);

	EXPECT_EQ(IntegerB->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntegerB->GetData(), 0);

	// Config 2: Fix upstream to match downstream at socket 1.
	IntegerA->SetData(1);
	IntegerB->SetData(0);

	ASSERT_EQ(UpstreamArea->TryToDisconnect(IntegerA, 1, UpstreamLinkNode, 2), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(IntegerA, 1, UpstreamLinkNode, 1), true);

	EXPECT_EQ(IntegerB->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntegerB->GetData(), 1);

	// Config 3: Fix downstream to match upstream at socket 2 instead.
	IntegerA->SetData(1);
	IntegerB->SetData(0);

	ASSERT_EQ(UpstreamArea->TryToDisconnect(IntegerA, 1, UpstreamLinkNode, 1), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(IntegerA, 1, UpstreamLinkNode, 2), true);
	ASSERT_EQ(DownstreamArea->TryToDisconnect(DownstreamLinkNode, 1, IntegerB, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 2, IntegerB, 1), true);

	EXPECT_EQ(IntegerB->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntegerB->GetData(), 1);

	// Config 4: Mismatch again. Upstream stays at socket 2, move downstream back to socket 1.
	IntegerA->SetData(1);
	IntegerB->SetData(0);

	ASSERT_EQ(DownstreamArea->TryToDisconnect(DownstreamLinkNode, 2, IntegerB, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, IntegerB, 1), true);

	EXPECT_EQ(IntegerB->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntegerB->GetData(), 0);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, SocketIndexRoutesDataCorrectly_AfterDeletion)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	IntegerVariableNode* IntegerA = new IntegerVariableNode();
	IntegerA->SetData(1);
	UpstreamArea->AddNode(IntegerA);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	IntegerVariableNode* IntegerB = new IntegerVariableNode();
	IntegerB->SetData(0);
	DownstreamArea->AddNode(IntegerB);

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* CurrentLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_TRUE(CurrentLinkNode->AddSocket({ "INT" }, "INT_0"));
	ASSERT_TRUE(CurrentLinkNode->AddSocket({ "INT" }, "INT_1"));

	Node* UpstreamLinkNode = UpstreamArea->GetNodeByID(LinkResult.first);
	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);

	// Connecting execution sockets.
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, IntegerB, 0), true);

	// Mismatched indices. Upstream socket 2, downstream socket 1.
	ASSERT_EQ(UpstreamArea->TryToConnect(IntegerA, 1, UpstreamLinkNode, 2), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, IntegerB, 1), true);

	// No data should flow due to mismatch.
	EXPECT_EQ(IntegerB->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntegerB->GetData(), 0);

	// Now delete first data socket in link node.
	std::string SocketIDToDelete = UpstreamLinkNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	ASSERT_EQ(UpstreamLinkNode->DeleteSocket(SocketIDToDelete), true);
	// Connection on downstream area should also be deleted since the socket was removed.
	ASSERT_EQ(DownstreamArea->IsConnected(DownstreamLinkNode, 1, IntegerB, 1), false);
	
	IntegerA->SetData(1);
	IntegerB->SetData(0);

	// Still no data flow.
	EXPECT_EQ(IntegerB->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntegerB->GetData(), 0);

	// Connect downstream int socket.
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, IntegerB, 1), true);

	IntegerA->SetData(1);
	IntegerB->SetData(0);

	// Finally data should flow.
	EXPECT_EQ(IntegerB->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntegerB->GetData(), 1);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, ReExecuteWithChangedData)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	FloatLiteralNode* UpstreamFloatNode = new FloatLiteralNode();
	UpstreamFloatNode->SetData(1.0f);
	UpstreamArea->AddNode(UpstreamFloatNode);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	FloatVariableNode* DownstreamFloatNode = new FloatVariableNode();
	DownstreamFloatNode->SetData(0.0f);
	DownstreamArea->AddNode(DownstreamFloatNode);

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* CurrentLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_TRUE(CurrentLinkNode->AddSocket({ "FLOAT" }));

	Node* UpstreamLinkNode = UpstreamArea->GetNodeByID(LinkResult.first);
	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);

	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamFloatNode, 0, UpstreamLinkNode, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamFloatNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamFloatNode, 1), true);

	// First execution.
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(DownstreamFloatNode->GetData(), 1.0f);

	// Change data and re-execute.
	UpstreamFloatNode->SetData(999.0f);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(DownstreamFloatNode->GetData(), 999.0f);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, Deletion_Basic)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	ASSERT_NE(UpstreamArea, nullptr);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	ASSERT_NE(DownstreamArea, nullptr);

	std::pair<std::string, std::string > LinkResult;
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult), true);
	ASSERT_NE(LinkResult.first, "");
	ASSERT_NE(LinkResult.second, "");

	UpstreamArea->Delete(UpstreamArea->GetNodeByID(LinkResult.first));

	// Both nodes involved in the link should be deleted.
	EXPECT_EQ(UpstreamArea->GetNodeByID(LinkResult.first), nullptr);
	EXPECT_EQ(DownstreamArea->GetNodeByID(LinkResult.second), nullptr);

	std::vector<std::pair<std::string, std::string>> LinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(UpstreamAreaID, DownstreamAreaID);
	EXPECT_EQ(LinkingNodes.size(), 0);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, DeletingDownstreamArea_RemovesUpstreamLinkNode)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));

	Node* UpstreamLinkNode = UpstreamArea->GetNodeByID(LinkResult.first);
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);

	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
	UpstreamLinkNode = UpstreamArea->GetNodeByID(LinkResult.first);
	EXPECT_EQ(UpstreamLinkNode, nullptr);

	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
}

TEST(LinkNodeTests, Deletion_FiresDestroyedCallbackOncePerNode)
{
	NODE_SYSTEM.Clear();

	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(UpstreamArea, nullptr);
	ASSERT_NE(DownstreamArea, nullptr);

	std::pair<std::string, std::string> LinkIDs;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamArea->GetID(), DownstreamArea->GetID(), &LinkIDs));

	const std::string UpstreamLinkID = LinkIDs.first;
	const std::string DownstreamLinkID = LinkIDs.second;

	int UpstreamDestroyedCount = 0;
	int DownstreamDestroyedCount = 0;
	UpstreamArea->AddNodeEventCallback([&UpstreamDestroyedCount, UpstreamLinkID](Node* CurrentNode, NODE_EVENT Event)
	{
		if (Event == DESTROYED && CurrentNode != nullptr && CurrentNode->GetID() == UpstreamLinkID)
			UpstreamDestroyedCount++;
	});

	DownstreamArea->AddNodeEventCallback([&DownstreamDestroyedCount, DownstreamLinkID](Node* CurrentNode, NODE_EVENT Event)
	{
		if (Event == DESTROYED && CurrentNode != nullptr && CurrentNode->GetID() == DownstreamLinkID)
			DownstreamDestroyedCount++;
	});

	// Deleting one half cascades to its partner, each node's DESTROYED callback must fire exactly once.
	ASSERT_TRUE(UpstreamArea->Delete(UpstreamArea->GetNodeByID(UpstreamLinkID)));

	EXPECT_EQ(UpstreamDestroyedCount, 1);
	EXPECT_EQ(DownstreamDestroyedCount, 1);

	EXPECT_EQ(UpstreamArea->GetNodeByID(UpstreamLinkID), nullptr);
	EXPECT_EQ(DownstreamArea->GetNodeByID(DownstreamLinkID), nullptr);
	EXPECT_FALSE(NODE_SYSTEM.IsLinked(UpstreamArea->GetID(), DownstreamArea->GetID()));

	NODE_SYSTEM.Clear();
}

TEST(LinkNodeTests, Basic_Tiny_Graph)
{
	// Create a 3-level hierarchy (Parent => Child => Grandchild).
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	ASSERT_NE(UpstreamArea, nullptr);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	ASSERT_NE(DownstreamArea, nullptr);

	NodeArea* GrandDownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string GrandDownstreamAreaID = GrandDownstreamArea->GetID();
	ASSERT_NE(GrandDownstreamArea, nullptr);

	NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID);

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(UpstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(UpstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(UpstreamArea, { DownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(UpstreamArea, { DownstreamArea }));

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(DownstreamArea, { UpstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(DownstreamArea, { UpstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(DownstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(DownstreamArea, {}));

	NODE_SYSTEM.LinkNodeAreas(DownstreamAreaID, GrandDownstreamAreaID);

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(UpstreamArea, { DownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(UpstreamArea, { DownstreamArea, GrandDownstreamArea }));

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(DownstreamArea, { UpstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(DownstreamArea, { GrandDownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(DownstreamArea, { GrandDownstreamArea }));

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(GrandDownstreamArea, { DownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(GrandDownstreamArea, { DownstreamArea, UpstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(GrandDownstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(GrandDownstreamArea, {}));

	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(UpstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(UpstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(GrandDownstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(GrandDownstreamArea, {}));

	EXPECT_TRUE(TEST_TOOLS.VerifyNoLinkNodes(UpstreamArea));
	EXPECT_TRUE(TEST_TOOLS.VerifyNoLinkNodes(GrandDownstreamArea));

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(GrandDownstreamArea);
}

TEST(LinkNodeTests, InfiniteLoop)
{
	// Create a loop (Parent => Child => Grandchild => Parent).
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	ASSERT_NE(UpstreamArea, nullptr);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	ASSERT_NE(DownstreamArea, nullptr);

	NodeArea* GrandDownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string GrandDownstreamAreaID = GrandDownstreamArea->GetID();
	ASSERT_NE(GrandDownstreamArea, nullptr);

	// Link Parent => Child.
	std::pair<std::string, std::string > LinkResult;
	EXPECT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));

	// Link Child => Grandchild.
	EXPECT_TRUE(NODE_SYSTEM.LinkNodeAreas(DownstreamAreaID, GrandDownstreamAreaID, &LinkResult));

	// Link Grandchild => Parent.
	EXPECT_TRUE(NODE_SYSTEM.LinkNodeAreas(GrandDownstreamAreaID, UpstreamAreaID, &LinkResult));

	// Test different levels of upstream/downstream retrieval to ensure no infinite loops and correct results.
	// Parent.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(UpstreamArea, { GrandDownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(UpstreamArea, { DownstreamArea, GrandDownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(UpstreamArea, { DownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(UpstreamArea, { DownstreamArea, GrandDownstreamArea }));

	// Child.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(DownstreamArea, { UpstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(DownstreamArea, { UpstreamArea, GrandDownstreamArea }));

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(DownstreamArea, { GrandDownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(DownstreamArea, { GrandDownstreamArea, UpstreamArea }));

	// Grandchild.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(GrandDownstreamArea, { DownstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(GrandDownstreamArea, { DownstreamArea, UpstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(GrandDownstreamArea, { UpstreamArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(GrandDownstreamArea, { UpstreamArea, DownstreamArea }));

	// Removing Child should sever links.
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);

	NODE_SYSTEM.IsLinked(UpstreamAreaID, DownstreamAreaID);
	EXPECT_FALSE(NODE_SYSTEM.IsLinked(UpstreamAreaID, DownstreamAreaID));
	EXPECT_FALSE(NODE_SYSTEM.IsLinked(DownstreamAreaID, GrandDownstreamAreaID));
	EXPECT_TRUE(NODE_SYSTEM.IsLinked(UpstreamAreaID, GrandDownstreamAreaID));

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(UpstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(UpstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(GrandDownstreamArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(GrandDownstreamArea, {}));

	// No dangling link nodes should remain.
	EXPECT_TRUE(TEST_TOOLS.VerifyNoDanglingLinkNodes(UpstreamArea));
	EXPECT_TRUE(TEST_TOOLS.VerifyNoDanglingLinkNodes(GrandDownstreamArea));

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(GrandDownstreamArea);
}

TEST(LinkNodeTests, Small_Graph)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallLinkedNodeAreaGraph();
	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());

	// After verification of the graph structure, unlink some areas and verify the structure updates correctly.
	EXPECT_TRUE(NODE_SYSTEM.UnlinkNodeAreas(Areas[0]->GetID(), Areas[1]->GetID()));
	// Try to unlink again to ensure it fails gracefully and doesn't cause issues.
	EXPECT_FALSE(NODE_SYSTEM.UnlinkNodeAreas(Areas[0]->GetID(), Areas[1]->GetID()));

	EXPECT_FALSE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());

	// After unlinking Areas[0] => Areas[1]:
	// Areas[0] should no longer have Areas[1] as immediate downstream.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[0], {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[0], {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[0], { Areas[2], Areas[3] }));

	// All downstream from root should now exclude Areas[1]'s entire subtree.
	std::vector<NodeArea*> AllExceptRootAndBranch1 = {
		Areas[2], Areas[3],
		Areas[7], Areas[8], Areas[9],
		Areas[10], Areas[11], Areas[12],
		Areas[16], Areas[17], Areas[18],
		Areas[19], Areas[20], Areas[21], Areas[22],
		Areas[25], Areas[26],
		Areas[27], Areas[28], Areas[29]
	};
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[0], AllExceptRootAndBranch1));

	// Areas[1] is now a root of its own subtree, no upstream at all.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[1], {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[1], {}));
	// Its immediate downstream remains unchanged.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[1], { Areas[4], Areas[5], Areas[6] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[1], { Areas[4], Areas[5], Areas[6], Areas[13], Areas[14], Areas[15], Areas[23], Areas[24] }));

	// Depth Level 2 under old branch 1, Areas[0] removed from all upstream chains.
	// Node 4.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[4], { Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[4], { Areas[1] }));
	// Node 5.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[5], { Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[5], { Areas[1] }));
	// Node 6.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[6], { Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[6], { Areas[1] }));

	// Depth Level 3 under old branch 1.
	// Node 13.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[13], { Areas[4] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[13], { Areas[1], Areas[4] }));
	// Node 14.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[14], { Areas[4] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[14], { Areas[1], Areas[4] }));
	// Node 15.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[15], { Areas[5] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[15], { Areas[1], Areas[5] }));

	// Depth Level 4 under old branch 1.
	// Node 23.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[23], { Areas[13] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[23], { Areas[1], Areas[4], Areas[13] }));
	// Node 24.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[24], { Areas[15] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[24], { Areas[1], Areas[5], Areas[15] }));

	// Branches 2 and 3 should be completely unaffected.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[2], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[2], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[3], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[3], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[25], { Areas[0], Areas[2], Areas[7], Areas[16] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[29], { Areas[0], Areas[3], Areas[12], Areas[22] }));
	
	NODE_SYSTEM.Clear();
}

void IntCalculationInSmallLinkedNodeAreaGraph()
{
	std::vector<NodeArea*> Areas = TEST_TOOLS.GetOrderedAreasFromSmallLinkedNodeAreaGraph();
	for (size_t i = 0; i < Areas.size(); i++)
	{
		if (Areas[i] == nullptr)
			return;
	}

	if (Areas.size() != 30)
		return;

	auto ConnectFromUpstream = [&](NodeArea* UpstreamArea, NodeArea* CurrentArea, std::vector<NodeArea*> DownstreamAreas) {
		size_t CurrentAreaIndex = std::find(Areas.begin(), Areas.end(), CurrentArea) - Areas.begin();

		IntegerVariableNode* IndexNode = new IntegerVariableNode();
		IndexNode->SetData(static_cast<int>(CurrentAreaIndex));
		CurrentArea->AddNode(IndexNode);

		std::vector<std::pair<std::string, std::string>> UpstreamLinkingNodeIDs;
		LinkNode* UpstreamLinkNode = nullptr;
		Node* BeginNode = nullptr;
		if (UpstreamArea != nullptr)
		{
			UpstreamLinkingNodeIDs = NODE_SYSTEM.GetLinkingNodesForAreas(UpstreamArea->GetID(), CurrentArea->GetID());
			UpstreamLinkNode = reinterpret_cast<LinkNode*>(CurrentArea->GetNodeByID(UpstreamLinkingNodeIDs[0].second));
		}
		else
		{
			BeginNode = NODE_FACTORY.CreateNode("BeginNode");
			CurrentArea->AddNode(BeginNode);
			CurrentArea->SetExecutionEntryNode(BeginNode);
		}

		if (DownstreamAreas.empty())
		{
			ArithmeticAddNode* AddNode = new ArithmeticAddNode();
			CurrentArea->AddNode(AddNode);

			IntegerVariableNode* ResultNode = new IntegerVariableNode();
			ResultNode->SetName("ResultNode_" + CurrentArea->GetName());
			CurrentArea->AddNode(ResultNode);

			// Connect execution from upstream link node to AddNode.
			ASSERT_EQ(CurrentArea->TryToConnect(UpstreamLinkNode, 0, AddNode, 0), true);
			// Connect area index integer output to AddNode.
			ASSERT_EQ(CurrentArea->TryToConnect(IndexNode, 1, AddNode, 1), true);
			// Connect upstream link node integer output to AddNode.
			ASSERT_EQ(CurrentArea->TryToConnect(UpstreamLinkNode, 1, AddNode, 2), true);

			// Connect AddNode execution output to result node.
			ASSERT_EQ(CurrentArea->TryToConnect(AddNode, 0, ResultNode, 0), true);
			// Connect AddNode integer output to result node.
			ASSERT_EQ(CurrentArea->TryToConnect(AddNode, 1, ResultNode, 1), true);

			return;
		}

		for (size_t i = 0; i < DownstreamAreas.size(); i++)
		{
			ArithmeticAddNode* AddNode = new ArithmeticAddNode();
			CurrentArea->AddNode(AddNode);

			std::vector<std::pair<std::string, std::string>> DownstreamLinkingNodeIDs =
				NODE_SYSTEM.GetLinkingNodesForAreas(CurrentArea->GetID(), DownstreamAreas[i]->GetID());
			LinkNode* DownstreamLinkNode = reinterpret_cast<LinkNode*>(CurrentArea->GetNodeByID(DownstreamLinkingNodeIDs[0].first));

			// Root area will not have UpstreamLinkNode.
			if (UpstreamArea != nullptr)
			{
				// Connect execution from upstream link node to AddNode.
				ASSERT_EQ(CurrentArea->TryToConnect(UpstreamLinkNode, 0, AddNode, 0), true);
				// Connect upstream link node integer output to AddNode.
				ASSERT_EQ(CurrentArea->TryToConnect(UpstreamLinkNode, 1, AddNode, 2), true);
			}
			else
			{
				// Connect execution from begin node to AddNode.
				ASSERT_EQ(CurrentArea->TryToConnect(BeginNode, 0, AddNode, 0), true);
			}

			// Connect area index integer output to AddNode.
			ASSERT_EQ(CurrentArea->TryToConnect(IndexNode, 1, AddNode, 1), true);

			// Connect AddNode execution output to downstream link node.
			ASSERT_EQ(CurrentArea->TryToConnect(AddNode, 0, DownstreamLinkNode, 0), true);
			// Create new socket on link for integer data.
			ASSERT_TRUE(DownstreamLinkNode->AddSocket({ "INT" }));
			// Connect AddNode integer output to downstream link node.
			ASSERT_EQ(CurrentArea->TryToConnect(AddNode, 1, DownstreamLinkNode, 1), true);
		}
	};

	// Depth Level 0.
	ConnectFromUpstream(nullptr, Areas[0], { Areas[1], Areas[2], Areas[3] });
	// Depth Level 1.
	ConnectFromUpstream(Areas[0], Areas[1], { Areas[4], Areas[5], Areas[6] });
	ConnectFromUpstream(Areas[0], Areas[2], { Areas[7], Areas[8], Areas[9] });
	ConnectFromUpstream(Areas[0], Areas[3], { Areas[10], Areas[11], Areas[12] });
	// Depth Level 2.
	ConnectFromUpstream(Areas[1], Areas[4], { Areas[13], Areas[14] });
	ConnectFromUpstream(Areas[1], Areas[5], { Areas[15]});
	ConnectFromUpstream(Areas[1], Areas[6], {  });
	ConnectFromUpstream(Areas[2], Areas[7], { Areas[16] });
	ConnectFromUpstream(Areas[2], Areas[8], { Areas[17] });
	ConnectFromUpstream(Areas[2], Areas[9], { Areas[18] });
	ConnectFromUpstream(Areas[3], Areas[10], { Areas[19] });
	ConnectFromUpstream(Areas[3], Areas[11], { Areas[20] });
	ConnectFromUpstream(Areas[3], Areas[12], { Areas[21], Areas[22] });
	// Depth Level 3.
	ConnectFromUpstream(Areas[4], Areas[13], { Areas[23] });
	ConnectFromUpstream(Areas[4], Areas[14], {  });
	ConnectFromUpstream(Areas[5], Areas[15], { Areas[24] });
	ConnectFromUpstream(Areas[7], Areas[16], { Areas[25] });
	ConnectFromUpstream(Areas[8], Areas[17], {  });
	ConnectFromUpstream(Areas[9], Areas[18], { Areas[26] });
	ConnectFromUpstream(Areas[10], Areas[19], {  });
	ConnectFromUpstream(Areas[11], Areas[20], { Areas[27] });
	ConnectFromUpstream(Areas[12], Areas[21], { Areas[28] });
	ConnectFromUpstream(Areas[12], Areas[22], { Areas[29] });
	// Depth Level 4.
	ConnectFromUpstream(Areas[13], Areas[23], {  });
	ConnectFromUpstream(Areas[15], Areas[24], {  });
	ConnectFromUpstream(Areas[16], Areas[25], {  });
	ConnectFromUpstream(Areas[18], Areas[26], {  });
	ConnectFromUpstream(Areas[20], Areas[27], {  });
	ConnectFromUpstream(Areas[21], Areas[28], {  });
	ConnectFromUpstream(Areas[22], Areas[29], {  });
}

TEST(LinkNodeTests, SaveLoad_With_Execute_Connections_Small)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallLinkedNodeAreaGraph();
	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());
	IntCalculationInSmallLinkedNodeAreaGraph();

	size_t TotalConnections = NODE_SYSTEM.GetTotalConnectionCount();
	std::vector<size_t> PerAreaConnectionCounts;
	for (auto Area : Areas)
	{
		PerAreaConnectionCounts.push_back(Area->GetConnectionCount());
		Area->SetSaveExecutedNodes(true);
	}

	Areas[0]->ExecuteNodeNetwork();

	std::unordered_map<std::string, std::vector<Node*>> ExecutedNodes = NODE_SYSTEM.GetLastExecutedNodes(Areas[0]->GetID());
	size_t ExecutedNodeAreaCount = ExecutedNodes.size();
	std::vector<size_t> ExecutedNodesPerArea;
	for (auto Area : Areas)
		ExecutedNodesPerArea.push_back(ExecutedNodes[Area->GetID()].size());

	//IntegerVariableNode* ResultNode_4 = new IntegerVariableNode();
	std::vector<std::pair<size_t, int>> ExpectedResults = {
		// Depth Level 0.
		{ 0, -1 },
		// Depth Level 1.
		{ 1, -1 }, { 2, -1 }, { 3, -1 },
		// Depth Level 2.
		{ 4, -1 }, { 5, -1 }, { 6, 7 }, { 7, -1 }, { 8, -1 }, { 9, -1 }, { 10, -1 }, { 11, -1 }, { 12, -1 },
		// Depth Level 3.
		{ 13, -1 }, { 14, 19 }, { 15, -1 }, { 16, -1 }, { 17, 27 }, { 18, -1 }, { 19, 32 }, { 20, -1 }, { 21, -1 }, { 22, -1 },
		// Depth Level 4.
		{ 23, 41 }, { 24, 45 }, { 25, 50 }, { 26, 55 }, { 27, 61 }, { 28, 64 }, { 29, 66 }
	};

	for (auto ExpectedResult : ExpectedResults)
	{
		if (ExpectedResult.second == -1)
			continue;

		Node* ResultNode = Areas[ExpectedResult.first]->GetNodesByName("ResultNode_" + std::to_string(ExpectedResult.first))[0];
		IntegerVariableNode* CastedResultNode = reinterpret_cast<IntegerVariableNode*>(ResultNode);
		int ResultValue = CastedResultNode->GetData();
		int ExpectedValue = ExpectedResult.second;
		ASSERT_EQ(ResultValue, ExpectedValue);
	}

	NODE_SYSTEM.SaveToFile("NodeSystemTests_TestSaveLoad_Small.json");
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());

	NODE_SYSTEM.LoadFromFile("NodeSystemTests_TestSaveLoad_Small.json");
	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());
	Areas = TEST_TOOLS.GetOrderedAreasFromSmallLinkedNodeAreaGraph();
	for (auto Area : Areas)
		Area->SetSaveExecutedNodes(true);

	size_t AfterLoadTotalConnections = NODE_SYSTEM.GetTotalConnectionCount();
	for (int i = 0; i < PerAreaConnectionCounts.size(); i++)
		ASSERT_EQ(PerAreaConnectionCounts[i], Areas[i]->GetConnectionCount());

	ASSERT_EQ(TotalConnections, AfterLoadTotalConnections);

	// Reset every leaf's ResultNode to 0 so the post-load ASSERT_EQ proves that
	// execution actually recomputed the values, not that save/load preserved them.
	for (auto ExpectedResult : ExpectedResults)
	{
		if (ExpectedResult.second == -1)
			continue;

		Node* ResultNode = Areas[ExpectedResult.first]->GetNodesByName("ResultNode_" + std::to_string(ExpectedResult.first))[0];
		IntegerVariableNode* CastedResultNode = reinterpret_cast<IntegerVariableNode*>(ResultNode);
		CastedResultNode->SetData(0);
		ASSERT_EQ(CastedResultNode->GetData(), 0); // sanity check
	}

	Areas[0]->ExecuteNodeNetwork();

	std::unordered_map<std::string, std::vector<Node*>> AfterLoadExecutedNodes = NODE_SYSTEM.GetLastExecutedNodes(Areas[0]->GetID());
	size_t AfterLoadExecutedNodeAreaCount = AfterLoadExecutedNodes.size();
	ASSERT_EQ(ExecutedNodeAreaCount, AfterLoadExecutedNodeAreaCount);
	for (int i = 0; i < Areas.size(); i++)
		ASSERT_EQ(ExecutedNodesPerArea[i], static_cast<int>(AfterLoadExecutedNodes[Areas[i]->GetID()].size()));
	
	for (auto ExpectedResult : ExpectedResults)
	{
		if (ExpectedResult.second == -1)
			continue;

		Node* ResultNode = Areas[ExpectedResult.first]->GetNodesByName("ResultNode_" + std::to_string(ExpectedResult.first))[0];
		IntegerVariableNode* CastedResultNode = reinterpret_cast<IntegerVariableNode*>(ResultNode);
		int ResultValue = CastedResultNode->GetData();
		int ExpectedValue = ExpectedResult.second;
		ASSERT_EQ(ResultValue, ExpectedValue);
	}

	NODE_SYSTEM.Clear();
}

TEST(LinkNodeTests, SaveLoad_With_Execute_And_Integer_Connections_Small)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallLinkedNodeAreaGraph();
	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());
	TEST_TOOLS.ConnectSmallLinkedNodeAreaGraph();

	// In each node area we will place integer variables with values equal to area index.
	for (int i = 0; i < Areas.size(); i++)
	{
		IntegerVariableNode* VariableNode = TEST_TOOLS.CreateIntegerVariableNode(i);
		Areas[i]->AddNode(VariableNode);
	}

	size_t TotalConnections = 0;
	std::vector<size_t> PerAreaConnectionCounts;
	for (auto Area : Areas)
	{
		TotalConnections += Area->GetConnectionCount();
		PerAreaConnectionCounts.push_back(Area->GetConnectionCount());
	}

	NODE_SYSTEM.SaveToFile("NodeSystemTests_TestSaveLoad_Small.json");
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());

	NODE_SYSTEM.LoadFromFile("NodeSystemTests_TestSaveLoad_Small.json");
	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());
	Areas = TEST_TOOLS.GetOrderedAreasFromSmallLinkedNodeAreaGraph();

	size_t AfterLoadTotalConnections = 0;
	for (int i = 0; i < PerAreaConnectionCounts.size(); i++)
	{
		AfterLoadTotalConnections += Areas[i]->GetConnectionCount();
		ASSERT_EQ(PerAreaConnectionCounts[i], Areas[i]->GetConnectionCount());
	}

	ASSERT_EQ(TotalConnections, AfterLoadTotalConnections);

	NODE_SYSTEM.Clear();
}

TEST(LinkNodeTests, SetSocketAllowedTypes_DisconnectsIncompatible)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	BoolLiteralNode* UpstreamBoolNode = new BoolLiteralNode();
	UpstreamBoolNode->SetData(true);
	UpstreamArea->AddNode(UpstreamBoolNode);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	BoolVariableNode* DownstreamBoolNode = new BoolVariableNode();
	DownstreamBoolNode->SetData(false);
	DownstreamArea->AddNode(DownstreamBoolNode);

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* UpstreamLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_TRUE(UpstreamLinkNode->AddSocket({ "BOOL" }));

	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);
	ASSERT_NE(DownstreamLinkNode, nullptr);

	// Wire everything up.
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamBoolNode, 0, UpstreamLinkNode, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamBoolNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamBoolNode, 1), true);

	// Verify data flows before type change.
	EXPECT_FALSE(DownstreamBoolNode->GetData());
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_TRUE(DownstreamBoolNode->GetData());

	// Change socket type on upstream link node to something incompatible.
	std::string SocketID = UpstreamLinkNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	NodeSocket* Socket = UpstreamLinkNode->GetSocketByID(SocketID);
	ASSERT_NE(Socket, nullptr);

	bool bNoDisconnections = Socket->SetAllowedTypes({ "FLOAT" });
	ASSERT_FALSE(bNoDisconnections);

	// Upstream connection to the BOOL socket should be severed.
	ASSERT_FALSE(UpstreamArea->IsConnected(UpstreamBoolNode, 0, UpstreamLinkNode, 1));

	// Partner socket on downstream link node should also have changed type.
	std::string PartnerSocketID = DownstreamLinkNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Output);
	ASSERT_FALSE(PartnerSocketID.empty());
	const NodeSocket* PartnerSocket = DownstreamLinkNode->GetSocketByID(PartnerSocketID);
	ASSERT_NE(PartnerSocket, nullptr);
	ASSERT_EQ(PartnerSocket->GetAllowedTypes().size(), 1);
	ASSERT_EQ(PartnerSocket->GetAllowedTypes()[0], "FLOAT");

	// Downstream connection from the BOOL socket should also be severed.
	ASSERT_FALSE(DownstreamArea->IsConnected(DownstreamLinkNode, 1, DownstreamBoolNode, 1));

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, SetSocketAllowedTypes_KeepsCompatible)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	IntegerLiteralNode* UpstreamIntNode = new IntegerLiteralNode();
	UpstreamIntNode->SetData(42);
	UpstreamArea->AddNode(UpstreamIntNode);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	IntegerVariableNode* DownstreamIntNode = new IntegerVariableNode();
	DownstreamIntNode->SetData(0);
	DownstreamArea->AddNode(DownstreamIntNode);

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* UpstreamLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_TRUE(UpstreamLinkNode->AddSocket({ "INT" }));

	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);
	ASSERT_NE(DownstreamLinkNode, nullptr);

	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamIntNode, 0, UpstreamLinkNode, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamIntNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamIntNode, 1), true);

	// Widen type to include INT and FLOAT, INT connections should survive.
	std::string SocketID = UpstreamLinkNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	NodeSocket* Socket = UpstreamLinkNode->GetSocketByID(SocketID);
	ASSERT_NE(Socket, nullptr);

	bool bNoDisconnections = Socket->SetAllowedTypes({ "INT", "FLOAT" });
	ASSERT_TRUE(bNoDisconnections);

	// All connections should still be intact.
	ASSERT_TRUE(UpstreamArea->IsConnected(UpstreamIntNode, 0, UpstreamLinkNode, 1));
	ASSERT_TRUE(DownstreamArea->IsConnected(DownstreamLinkNode, 0, DownstreamIntNode, 0));
	ASSERT_TRUE(DownstreamArea->IsConnected(DownstreamLinkNode, 1, DownstreamIntNode, 1));

	// Data should still flow.
	EXPECT_EQ(DownstreamIntNode->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(DownstreamIntNode->GetData(), 42);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, SetSocketAllowedTypes_PartialDisconnect_MultipleSockets)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	UpstreamArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	BoolLiteralNode* UpstreamBoolNode = new BoolLiteralNode();
	UpstreamBoolNode->SetData(true);
	UpstreamArea->AddNode(UpstreamBoolNode);

	FloatLiteralNode* UpstreamFloatNode = new FloatLiteralNode();
	UpstreamFloatNode->SetData(3.14f);
	UpstreamArea->AddNode(UpstreamFloatNode);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	BoolVariableNode* DownstreamBoolNode = new BoolVariableNode();
	DownstreamBoolNode->SetData(false);
	DownstreamArea->AddNode(DownstreamBoolNode);

	FloatVariableNode* DownstreamFloatNode = new FloatVariableNode();
	DownstreamFloatNode->SetData(0.0f);
	DownstreamArea->AddNode(DownstreamFloatNode);

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* UpstreamLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_TRUE(UpstreamLinkNode->AddSocket({ "BOOL" }));
	ASSERT_TRUE(UpstreamLinkNode->AddSocket({ "FLOAT" }));

	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);
	ASSERT_NE(DownstreamLinkNode, nullptr);

	// Wire execution.
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	// Wire BOOL at socket index 1.
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamBoolNode, 0, UpstreamLinkNode, 1), true);
	// Wire FLOAT at socket index 2.
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamFloatNode, 0, UpstreamLinkNode, 2), true);

	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamBoolNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamBoolNode, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamBoolNode, 0, DownstreamFloatNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 2, DownstreamFloatNode, 1), true);

	// Verify both values flow.
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_TRUE(DownstreamBoolNode->GetData());
	EXPECT_EQ(DownstreamFloatNode->GetData(), 3.14f);

	// Change only the BOOL socket (index 1) to INT, FLOAT socket (index 2) should be unaffected.
	std::string BoolSocketID = UpstreamLinkNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	NodeSocket* Socket = UpstreamLinkNode->GetSocketByID(BoolSocketID);
	ASSERT_NE(Socket, nullptr);

	bool bNoDisconnections = Socket->SetAllowedTypes({ "INT" });
	ASSERT_FALSE(bNoDisconnections);

	// BOOL connections on both sides should be severed.
	ASSERT_FALSE(UpstreamArea->IsConnected(UpstreamBoolNode, UpstreamLinkNode));
	ASSERT_FALSE(DownstreamArea->IsConnected(DownstreamLinkNode, 1, DownstreamBoolNode, 1));

	// FLOAT connections should remain intact.
	ASSERT_TRUE(UpstreamArea->IsConnected(UpstreamFloatNode, UpstreamLinkNode));
	ASSERT_TRUE(DownstreamArea->IsConnected(DownstreamLinkNode, 2, DownstreamFloatNode, 1));

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, RenameSocket_PropagatesPartnerName)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();

	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* UpstreamLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_TRUE(UpstreamLinkNode->AddSocket({ "BOOL" }));

	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);
	ASSERT_NE(DownstreamLinkNode, nullptr);

	// Get the socket on the upstream link node (index 1, after the execution socket).
	std::string UpstreamSocketID = UpstreamLinkNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	NodeSocket* UpstreamSocket = UpstreamLinkNode->GetSocketByID(UpstreamSocketID);
	ASSERT_NE(UpstreamSocket, nullptr);

	// Get the partner socket on the downstream link node.
	std::string DownstreamSocketID = DownstreamLinkNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Output);
	NodeSocket* DownstreamSocket = DownstreamLinkNode->GetSocketByID(DownstreamSocketID);
	ASSERT_NE(DownstreamSocket, nullptr);

	// Both sockets should have the same initial name.
	EXPECT_EQ(UpstreamSocket->GetName(), DownstreamSocket->GetName());

	// Rename the socket on the upstream link node.
	std::string NewName = "RenamedBoolSocket";
	UpstreamSocket->SetName(NewName);

	// The partner socket on the downstream link node should also have the new name.
	EXPECT_EQ(UpstreamSocket->GetName(), NewName);
	EXPECT_EQ(DownstreamSocket->GetName(), NewName);

	// Rename from the downstream side to verify bidirectional propagation.
	std::string AnotherName = "RenamedAgain";
	DownstreamSocket->SetName(AnotherName);

	EXPECT_EQ(DownstreamSocket->GetName(), AnotherName);
	EXPECT_EQ(UpstreamSocket->GetName(), AnotherName);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, Copy_Paste_Dangling)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);
	UpstreamArea->SetSaveExecutedNodes(true);

	IntegerLiteralNode* UpstreamIntNode = new IntegerLiteralNode();
	UpstreamIntNode->SetData(3);
	UpstreamArea->AddNode(UpstreamIntNode);

	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string DownstreamAreaID = DownstreamArea->GetID();
	DownstreamArea->SetSaveExecutedNodes(true);

	IntegerVariableNode* DownstreamIntNode = new IntegerVariableNode();
	DownstreamIntNode->SetData(0);
	DownstreamArea->AddNode(DownstreamIntNode);
	
	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamAreaID, DownstreamAreaID, &LinkResult));
	LinkNode* UpstreamLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkResult.first));
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_TRUE(UpstreamLinkNode->AddSocket({ "INT" }));

	Node* DownstreamLinkNode = DownstreamArea->GetNodeByID(LinkResult.second);
	ASSERT_NE(DownstreamLinkNode, nullptr);

	// Connect execution and integer data.
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamIntNode, 0, UpstreamLinkNode, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamIntNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamIntNode, 1), true);

	// Execute to verify everything is working.
	EXPECT_EQ(DownstreamIntNode->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(DownstreamIntNode->GetData(), 3);
	DownstreamIntNode->SetData(0);
	
	// Simulate copying and pasting the upstream link node.
	TEST_TOOLS.SimulateCopyPasteNodes({ UpstreamLinkNode }, UpstreamArea);

	// Get pasted node (should be the only other link node in the area).
	LinkNode* PastedLinkNode = nullptr;
	std::vector<LinkNode*> UpstreamAreaNodes = UpstreamArea->GetNodesByType<LinkNode>();
	for (auto Node : UpstreamAreaNodes)
	{
		if (Node->GetID() != UpstreamLinkNode->GetID())
		{
			PastedLinkNode = Node;
			break;
		}
	}

	// New node should have same sockets but no connections.
	ASSERT_NE(PastedLinkNode, nullptr);
	EXPECT_EQ(PastedLinkNode->GetInputSocketCount(), UpstreamLinkNode->GetInputSocketCount());
	EXPECT_EQ(PastedLinkNode->GetOutputSocketCount(), UpstreamLinkNode->GetOutputSocketCount());
	for (size_t i = 0; i < UpstreamLinkNode->GetInputSocketCount(); i++)
	{
		NodeSocket* OriginalSocket = UpstreamLinkNode->GetSocketByIndex(i, NodeSocket::SocketFlow::Input);
		NodeSocket* PastedSocket = PastedLinkNode->GetSocketByIndex(i, NodeSocket::SocketFlow::Input);
		EXPECT_EQ(OriginalSocket->GetAllowedTypes(), PastedSocket->GetAllowedTypes());
	}
	EXPECT_EQ(PastedLinkNode->GetNodesConnectedToInput().size(), 0);
	EXPECT_EQ(PastedLinkNode->GetNodesConnectedToOutput().size(), 0);
	// It should be dangling.
	EXPECT_EQ(PastedLinkNode->IsDangling(), true);

	// Old node should still function properly.
	EXPECT_EQ(DownstreamIntNode->GetData(), 0);
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	EXPECT_EQ(DownstreamIntNode->GetData(), 3);
	DownstreamIntNode->SetData(0);

	// Rewire execution and data flow through the pasted node.
	ASSERT_EQ(UpstreamArea->TryToDisconnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, PastedLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToDisconnect(UpstreamIntNode, 0, UpstreamLinkNode, 1), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamIntNode, 0, PastedLinkNode, 1), true);

	// Run execution, this time execution and data flow should not work since the pasted node is dangling and not linked to the downstream area.
	EXPECT_EQ(DownstreamIntNode->GetData(), 0);
	ASSERT_FALSE(DownstreamArea->ExecuteNodeNetwork()); // It does not have entry node, but we want to clear executed nodes list.
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	
	EXPECT_EQ(UpstreamArea->GetLastExecutedNodes().size(), 2);
	EXPECT_EQ(DownstreamIntNode->GetData(), 0);
	EXPECT_EQ(DownstreamArea->GetLastExecutedNodes().size(), 0);

	// Now fix pasted node by undangling it.
	// Without bForceRestorePartner undangling should fail.
	ASSERT_FALSE(NODE_SYSTEM.TryToFixDanglingLinkNode(PastedLinkNode, false));
	// With it should succeed.
	ASSERT_TRUE(NODE_SYSTEM.TryToFixDanglingLinkNode(PastedLinkNode, true));

	// After undangling, in downstream area there should be a new link node that is partner to the pasted link node.
	LinkNode* PastedLinkPartnerNode = nullptr;
	std::vector<LinkNode*> DownstreamAreaNodes = DownstreamArea->GetNodesByType<LinkNode>();
	for (auto Node : DownstreamAreaNodes)
	{
		if (Node->GetID() != DownstreamLinkNode->GetID())
		{
			PastedLinkPartnerNode = Node;
			break;
		}
	}
	ASSERT_NE(PastedLinkPartnerNode, nullptr);

	// Rewire downstream connections to the new partner node.
	ASSERT_EQ(DownstreamArea->TryToDisconnect(DownstreamLinkNode, 0, DownstreamIntNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(PastedLinkPartnerNode, 0, DownstreamIntNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToDisconnect(DownstreamLinkNode, 1, DownstreamIntNode, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(PastedLinkPartnerNode, 1, DownstreamIntNode, 1), true);

	// Run again, everything should work now.
	EXPECT_EQ(DownstreamIntNode->GetData(), 0);
	ASSERT_FALSE(DownstreamArea->ExecuteNodeNetwork());
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());

	std::vector<Node*> UpstreamExecutedNodes = UpstreamArea->GetLastExecutedNodes();
	EXPECT_EQ(UpstreamExecutedNodes.size(), 2);
	ASSERT_TRUE((UpstreamExecutedNodes[0]->GetID() == PastedLinkNode->GetID()) ||
				(UpstreamExecutedNodes[1]->GetID() == PastedLinkNode->GetID()));
	
	EXPECT_EQ(DownstreamIntNode->GetData(), 3);

	std::vector<Node*> DownstreamExecutedNodes = DownstreamArea->GetLastExecutedNodes();
	EXPECT_EQ(DownstreamExecutedNodes.size(), 2);
	ASSERT_TRUE((DownstreamExecutedNodes[0]->GetID() == PastedLinkPartnerNode->GetID()) ||
				(DownstreamExecutedNodes[1]->GetID() == PastedLinkPartnerNode->GetID()));

	DownstreamIntNode->SetData(0);

	// Old link node should not be dangling.
	EXPECT_EQ(UpstreamLinkNode->IsDangling(), false);

	// If we will wire connections back through the old link node, it should still work.
	ASSERT_EQ(UpstreamArea->TryToDisconnect(BeginNode, 0, PastedLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
	ASSERT_EQ(UpstreamArea->TryToDisconnect(UpstreamIntNode, 0, PastedLinkNode, 1), true);
	ASSERT_EQ(UpstreamArea->TryToConnect(UpstreamIntNode, 0, UpstreamLinkNode, 1), true);

	ASSERT_EQ(DownstreamArea->TryToDisconnect(PastedLinkPartnerNode, 0, DownstreamIntNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamIntNode, 0), true);
	ASSERT_EQ(DownstreamArea->TryToDisconnect(PastedLinkPartnerNode, 1, DownstreamIntNode, 1), true);
	ASSERT_EQ(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamIntNode, 1), true);

	EXPECT_EQ(DownstreamIntNode->GetData(), 0);
	ASSERT_FALSE(DownstreamArea->ExecuteNodeNetwork());
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	UpstreamExecutedNodes = UpstreamArea->GetLastExecutedNodes();
	EXPECT_EQ(UpstreamExecutedNodes.size(), 2);
	ASSERT_TRUE((UpstreamExecutedNodes[0]->GetID() == UpstreamLinkNode->GetID()) ||
				(UpstreamExecutedNodes[1]->GetID() == UpstreamLinkNode->GetID()));

	EXPECT_EQ(DownstreamIntNode->GetData(), 3);

	DownstreamExecutedNodes = DownstreamArea->GetLastExecutedNodes();
	EXPECT_EQ(DownstreamExecutedNodes.size(), 2);
	ASSERT_TRUE((DownstreamExecutedNodes[0]->GetID() == DownstreamLinkNode->GetID()) ||
				(DownstreamExecutedNodes[1]->GetID() == DownstreamLinkNode->GetID()));
	DownstreamIntNode->SetData(0);

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(LinkNodeTests, GetImmediateDownstreamAreas_Deduplicates_DuplicateLinks)
{
	NODE_SYSTEM.Clear();

	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	const std::string UpstreamID = UpstreamArea->GetID();
	const std::string DownstreamID = DownstreamArea->GetID();

	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamID, DownstreamID));
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamID, DownstreamID));

	// Two distinct link records exist, but the conceptual downstream set is just one.
	std::vector<NodeArea*> Downstream = NODE_SYSTEM.GetImmediateDownstreamAreas(UpstreamID);
	EXPECT_EQ(Downstream.size(), 1);

	NODE_SYSTEM.Clear();
}

TEST(LinkNodeTests, MoveNodesTo_UnrelatedArea_UpdatesLinkRecord)
{
	NODE_SYSTEM.Clear();

	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* UnrelatedArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(UpstreamArea, nullptr);
	ASSERT_NE(DownstreamArea, nullptr);
	ASSERT_NE(UnrelatedArea, nullptr);

	std::pair<std::string, std::string> LinkIDs;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamArea->GetID(), DownstreamArea->GetID(), &LinkIDs));

	LinkNode* UpstreamLinkNode = static_cast<LinkNode*>(UpstreamArea->GetNodeByID(LinkIDs.first));
	ASSERT_NE(UpstreamLinkNode, nullptr);

	ASSERT_TRUE(NODE_SYSTEM.MoveNodesTo(UpstreamArea, UnrelatedArea));

	// Physical layout reflects the move.
	EXPECT_EQ(UpstreamArea->GetNodesByType<LinkNode>().size(), 0);
	ASSERT_EQ(UnrelatedArea->GetNodesByType<LinkNode>().size(), 1);
	EXPECT_EQ(UnrelatedArea->GetNodesByType<LinkNode>()[0]->GetID(), LinkIDs.first);

	// Link record now follows the LinkNode: UpstreamArea is no longer linked to DownstreamArea, UnrelatedArea is.
	EXPECT_FALSE(NODE_SYSTEM.IsLinked(UpstreamArea->GetID(), DownstreamArea->GetID()));
	EXPECT_TRUE(NODE_SYSTEM.IsLinked(UnrelatedArea->GetID(), DownstreamArea->GetID()));

	EXPECT_EQ(NODE_SYSTEM.GetImmediateDownstreamAreas(UpstreamArea->GetID()).size(), 0);
	ASSERT_EQ(NODE_SYSTEM.GetImmediateDownstreamAreas(UnrelatedArea->GetID()).size(), 1);
	EXPECT_EQ(NODE_SYSTEM.GetImmediateDownstreamAreas(UnrelatedArea->GetID())[0], DownstreamArea);
	ASSERT_EQ(NODE_SYSTEM.GetImmediateUpstreamAreas(DownstreamArea->GetID()).size(), 1);
	EXPECT_EQ(NODE_SYSTEM.GetImmediateUpstreamAreas(DownstreamArea->GetID())[0], UnrelatedArea);

	// GetParent never follows LinkNode pointers, so a moved LinkNode does NOT create a phantom parent on the new host area.
	EXPECT_EQ(UnrelatedArea->GetParent(), nullptr);

	NODE_SYSTEM.Clear();
}

TEST(LinkNodeTests, MoveNodesTo_UnrelatedArea_PartnerStaysLinked_AndDataFlows)
{
	NODE_SYSTEM.Clear();

	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* DownstreamArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* UnrelatedArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(UpstreamArea, nullptr);
	ASSERT_NE(DownstreamArea, nullptr);
	ASSERT_NE(UnrelatedArea, nullptr);

	std::pair<std::string, std::string> LinkIDs;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(UpstreamArea->GetID(), DownstreamArea->GetID(), &LinkIDs));

	LinkNode* UpstreamLinkNode = static_cast<LinkNode*>(UpstreamArea->GetNodeByID(LinkIDs.first));
	LinkNode* DownstreamLinkNode = static_cast<LinkNode*>(DownstreamArea->GetNodeByID(LinkIDs.second));
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_NE(DownstreamLinkNode, nullptr);
	ASSERT_TRUE(UpstreamLinkNode->AddSocket({ "BOOL" }));

	// Upstream graph: BeginNode (entry) and a BOOL literal feeding the link's data socket.
	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	ASSERT_NE(BeginNode, nullptr);
	ASSERT_TRUE(UpstreamArea->AddNode(BeginNode));
	UpstreamArea->SetExecutionEntryNode(BeginNode);

	BoolLiteralNode* UpstreamBoolNode = new BoolLiteralNode();
	UpstreamBoolNode->SetData(true);
	ASSERT_TRUE(UpstreamArea->AddNode(UpstreamBoolNode));

	ASSERT_TRUE(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0));
	ASSERT_TRUE(UpstreamArea->TryToConnect(UpstreamBoolNode, 0, UpstreamLinkNode, 1));

	// Downstream graph: the link drives a BOOL variable (execution and data).
	BoolVariableNode* DownstreamBoolNode = new BoolVariableNode();
	DownstreamBoolNode->SetData(false);
	ASSERT_TRUE(DownstreamArea->AddNode(DownstreamBoolNode));
	ASSERT_TRUE(DownstreamArea->TryToConnect(DownstreamLinkNode, 0, DownstreamBoolNode, 0));
	ASSERT_TRUE(DownstreamArea->TryToConnect(DownstreamLinkNode, 1, DownstreamBoolNode, 1));

	// Data flows across the link before the move.
	ASSERT_TRUE(UpstreamArea->ExecuteNodeNetwork());
	ASSERT_TRUE(DownstreamBoolNode->GetData());

	// Move the whole upstream graph (BeginNode, literal and link) to an unrelated area.
	DownstreamBoolNode->SetData(false);
	ASSERT_TRUE(NODE_SYSTEM.MoveNodesTo(UpstreamArea, UnrelatedArea));

	// Physical layout and link record follow the moved LinkNode.
	EXPECT_EQ(UpstreamArea->GetNodesByType<LinkNode>().size(), 0);
	ASSERT_EQ(UnrelatedArea->GetNodesByType<LinkNode>().size(), 1);
	EXPECT_FALSE(NODE_SYSTEM.IsLinked(UpstreamArea->GetID(), DownstreamArea->GetID()));
	EXPECT_TRUE(NODE_SYSTEM.IsLinked(UnrelatedArea->GetID(), DownstreamArea->GetID()));

	// The non-moved partner must remain linked and resolve to the moved LinkNode.
	EXPECT_FALSE(DownstreamLinkNode->IsDangling());
	EXPECT_EQ(DownstreamLinkNode->GetPartnerNode(), UpstreamLinkNode);

	// Cross-area data still flows when executed from the new host area.
	UnrelatedArea->SetExecutionEntryNode(BeginNode);
	ASSERT_TRUE(UnrelatedArea->ExecuteNodeNetwork());
	EXPECT_TRUE(DownstreamBoolNode->GetData());

	NODE_SYSTEM.Clear();
}

TEST(LinkNodeTests, Load_LinkNodeWithNonLinkNodePartner_IsHandledSafely)
{
	NODE_SYSTEM.Clear();

	// PartnerArea holds a plain, non LinkNode node.
	NodeArea* PartnerArea = NODE_SYSTEM.CreateNodeArea();
	Node* NonLinkNode = NODE_FACTORY.CreateNode("BeginNode");
	ASSERT_NE(NonLinkNode, nullptr);
	ASSERT_TRUE(PartnerArea->AddNode(NonLinkNode));
	const std::string PartnerAreaID = PartnerArea->GetID();
	const std::string NonLinkNodeID = NonLinkNode->GetID();

	// LinkArea holds a real LinkNode, created as a proper pair with a throwaway area.
	NodeArea* LinkArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* ThrowawayArea = NODE_SYSTEM.CreateNodeArea();
	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(LinkArea->GetID(), ThrowawayArea->GetID(), &LinkResult));
	const std::string LinkAreaID = LinkArea->GetID();
	const std::string ThrowawayAreaID = ThrowawayArea->GetID();
	const std::string LinkNodeID = LinkResult.first;
	const std::string PartnerLinkNodeID = LinkResult.second;

	auto ReplaceFirst = [](std::string& Text, const std::string& Needle, const std::string& Replacement) -> bool
	{
		const size_t Position = Text.find(Needle);
		if (Position == std::string::npos)
			return false;
		Text.replace(Position, Needle.size(), Replacement);
		return true;
	};

	// Repoint the LinkNode's partner at the non LinkNode (simulating a hand edited save).
	std::string LinkAreaJson = LinkArea->ToJson();
	std::string PartnerAreaJson = PartnerArea->ToJson();
	ASSERT_TRUE(ReplaceFirst(LinkAreaJson, "\"PartnerNodeID\":\"" + PartnerLinkNodeID + "\"", "\"PartnerNodeID\":\"" + NonLinkNodeID + "\""));
	ASSERT_TRUE(ReplaceFirst(LinkAreaJson, "\"LinkedAreaID\":\"" + ThrowawayAreaID + "\"", "\"LinkedAreaID\":\"" + PartnerAreaID + "\""));

	// Assemble a NodeSystem JSON containing only LinkArea and PartnerArea.
	Json::Value Root;
	Root["SocketTypeToColorAssociations"] = Json::objectValue;
	Json::Value AreasJson(Json::objectValue);
	AreasJson[LinkAreaID] = LinkAreaJson;
	AreasJson[PartnerAreaID] = PartnerAreaJson;
	Root["NodeAreas"] = AreasJson;
	Json::StreamWriterBuilder Builder;
	Builder.settings_["indentation"] = "";
	const std::string SystemJson = Json::writeString(Builder, Root);

	// Loading must complete without crashing or reading out of bounds.
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(SystemJson));

	// The non-LinkNode partner is untouched, and the link was not "fixed" to point at it.
	Node* ReloadedNonLinkNode = NODE_SYSTEM.GetNodeByID(NonLinkNodeID);
	ASSERT_NE(ReloadedNonLinkNode, nullptr);
	EXPECT_EQ(ReloadedNonLinkNode->GetType(), "BeginNode");

	LinkNode* ReloadedLinkNode = dynamic_cast<LinkNode*>(NODE_SYSTEM.GetNodeByID(LinkNodeID));
	ASSERT_NE(ReloadedLinkNode, nullptr);
	EXPECT_TRUE(ReloadedLinkNode->IsDangling());

	NODE_SYSTEM.Clear();
}

TEST(LinkNodeTests, CopyArea_IntoLinkedArea_RejectedLinkNode_SkipsConnection)
{
	NODE_SYSTEM.Clear();

	// SourceArea holds a LinkNode whose partner lives in LinkedArea.
	NodeArea* SourceArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* LinkedArea = NODE_SYSTEM.CreateNodeArea();
	std::pair<std::string, std::string> LinkResult;
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(SourceArea->GetID(), LinkedArea->GetID(), &LinkResult));
	Node* UpstreamLinkNode = NODE_SYSTEM.GetNodeByID(LinkResult.first);
	ASSERT_NE(UpstreamLinkNode, nullptr);

	// A BeginNode connected to the LinkNode, both inside SourceArea.
	Node* ExecutionBeginNode = NODE_FACTORY.CreateNode("BeginNode");
	ASSERT_NE(ExecutionBeginNode, nullptr);
	ASSERT_TRUE(SourceArea->AddNode(ExecutionBeginNode));
	ASSERT_TRUE(SourceArea->TryToConnect(ExecutionBeginNode, 0, UpstreamLinkNode, 0));
	ASSERT_GT(SourceArea->GetConnectionCount(), 0);

	// Copying SourceArea into LinkedArea: the LinkNode's copy (LinkedAreaID == LinkedArea) is
	// rejected by AddNode while the BeginNode copies fine. ProcessConnections must skip the
	// unrecreatable connection instead of dereferencing a null mapped socket.
	NODE_SYSTEM.CopyNodesTo(SourceArea, LinkedArea);

	// The BeginNode was copied, the rejected LinkNode's connection was not recreated.
	EXPECT_EQ(LinkedArea->GetNodesByType<BeginNode>().size(), 1);
	EXPECT_EQ(LinkedArea->GetConnectionCount(), 0);

	NODE_SYSTEM.Clear();
}