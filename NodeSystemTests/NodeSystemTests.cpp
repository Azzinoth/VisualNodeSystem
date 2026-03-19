#include "NodeSystemTests.h"
using namespace VisNodeSys;

TEST(NodeSystemTests, CreateAndDelete)
{
	std::vector<std::string> NodeAreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	EXPECT_EQ(NodeAreaIDs.size(), 0);

	NodeArea* NewNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NewNodeArea, nullptr);

	NodeAreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	EXPECT_EQ(NodeAreaIDs.size(), 1);
	EXPECT_EQ(NodeAreaIDs[0] == NewNodeArea->GetID(), true);

	NODE_SYSTEM.DeleteNodeArea(NewNodeArea);

	NodeAreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	EXPECT_EQ(NodeAreaIDs.size(), 0);
}

TEST(NodeSystemTests, LinkAreas_Basic_EstablishConnection)
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

TEST(NodeSystemTests, LinkAreas_Basic_AddSockets)
{
	NodeArea* UpstreamArea = NODE_SYSTEM.CreateNodeArea();
	std::string UpstreamAreaID = UpstreamArea->GetID();
	ASSERT_NE(UpstreamArea, nullptr);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	ASSERT_NE(BeginNode, nullptr);
	UpstreamArea->AddNode(BeginNode);
	UpstreamArea->SetExecutionEntryNode(BeginNode);
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
	EXPECT_TRUE(NODE_SYSTEM.AddSocketToLink(UpstreamAreaID, LinkResult.first, "BOOL"));

	// Connect node to a parent link node.
	Node* UpstreamLinkNode = UpstreamArea->GetNodeByID(LinkResult.first);
	ASSERT_NE(UpstreamLinkNode, nullptr);
	ASSERT_EQ(UpstreamArea->TryToConnect(BeginNode, 0, UpstreamLinkNode, 0), true);
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

	NODE_SYSTEM.DeleteNodeArea(UpstreamArea);
	NODE_SYSTEM.DeleteNodeArea(DownstreamArea);
}

TEST(NodeSystemTests, LinkAreas_MultipleSockets)
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
	ASSERT_TRUE(NODE_SYSTEM.AddSocketToLink(UpstreamAreaID, LinkResult.first, "BOOL"));
	ASSERT_TRUE(NODE_SYSTEM.AddSocketToLink(UpstreamAreaID, LinkResult.first, "FLOAT"));

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

TEST(NodeSystemTests, LinkAreas_ArithmeticAcrossLink)
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
	ASSERT_TRUE(NODE_SYSTEM.AddSocketToLink(UpstreamAreaID, LinkResult.first, "INT"));

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

TEST(NodeSystemTests, LinkAreas_ReExecuteWithChangedData)
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
	ASSERT_TRUE(NODE_SYSTEM.AddSocketToLink(UpstreamAreaID, LinkResult.first, "FLOAT"));

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

TEST(NodeSystemTests, LinkAreas_Deletion_Basic)
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

TEST(NodeSystemTests, LinkAreas_DeletingDownstreamArea_RemovesUpstreamLinkNode)
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

TEST(NodeSystemTests, LinkAreas_Basic_Tiny_Graph)
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

TEST(NodeSystemTests, LinkAreas_InfiniteLoop)
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

TEST(NodeSystemTests, LinkAreas_Small_Graph)
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

TEST(NodeSystemTests, SaveLoad_No_Connections_Small)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallLinkedNodeAreaGraph();
	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());

	NODE_SYSTEM.SaveToFile("NodeSystemTests_TestSaveLoad_Small.json");

	NODE_SYSTEM.Clear();
	EXPECT_FALSE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());

	NODE_SYSTEM.LoadFromFile("NodeSystemTests_TestSaveLoad_Small.json");
	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());

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
			NODE_SYSTEM.AddSocketToLink(CurrentArea->GetID(), DownstreamLinkingNodeIDs[0].first, "INT");
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

TEST(NodeSystemTests, SaveLoad_With_Execute_Connections_Small)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallLinkedNodeAreaGraph();
	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());
	IntCalculationInSmallLinkedNodeAreaGraph();

	int TotalConnections = NODE_SYSTEM.GetTotalConnectionCount();
	std::vector<int> PerAreaConnectionCounts;
	for (auto Area : Areas)
	{
		PerAreaConnectionCounts.push_back(Area->GetConnectionCount());
		Area->SetSaveExecutedNodes(true);
	}

	Areas[0]->ExecuteNodeNetwork();

	std::unordered_map<std::string, std::vector<Node*>> ExecutedNodes = NODE_SYSTEM.GetLastExecutedNodes(Areas[0]->GetID());
	int ExecutedNodeAreaCount = ExecutedNodes.size();
	std::vector<int> ExecutedNodesPerArea;
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

	int AfterLoadTotalConnections = NODE_SYSTEM.GetTotalConnectionCount();
	for (int i = 0; i < PerAreaConnectionCounts.size(); i++)
		ASSERT_EQ(PerAreaConnectionCounts[i], Areas[i]->GetConnectionCount());

	ASSERT_EQ(TotalConnections, AfterLoadTotalConnections);

	Areas[0]->ExecuteNodeNetwork();

	std::unordered_map<std::string, std::vector<Node*>> AfterLoadExecutedNodes = NODE_SYSTEM.GetLastExecutedNodes(Areas[0]->GetID());
	int AfterLoadExecutedNodeAreaCount = AfterLoadExecutedNodes.size();
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

TEST(NodeSystemTests, SaveLoad_With_Execute_And_Integer_Connections_Small)
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

	int TotalConnections = 0;
	std::vector<int> PerAreaConnectionCounts;
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

	int AfterLoadTotalConnections = 0;
	for (int i = 0; i < PerAreaConnectionCounts.size(); i++)
	{
		AfterLoadTotalConnections += Areas[i]->GetConnectionCount();
		ASSERT_EQ(PerAreaConnectionCounts[i], Areas[i]->GetConnectionCount());
	}

	ASSERT_EQ(TotalConnections, AfterLoadTotalConnections);

	NODE_SYSTEM.Clear();
}