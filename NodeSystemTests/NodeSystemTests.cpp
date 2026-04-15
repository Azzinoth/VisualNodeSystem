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

TEST(NodeSystemTests, LoadShouldClear)
{
	NODE_SYSTEM.Clear();

	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 1);

	std::string JsonString = NODE_SYSTEM.ToJson();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 2);

	NODE_SYSTEM.LoadFromJson(JsonString);
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, TryToConnect_CrossArea_IsRejected)
{
	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();

	BoolLiteralNode* SourceInAreaA = new BoolLiteralNode();
	BoolVariableNode* DestinationInAreaB = new BoolVariableNode();
	AreaA->AddNode(SourceInAreaA);
	AreaB->AddNode(DestinationInAreaB);

	// Nodes SourceInAreaA and DestinationInAreaB belong to different areas, so connection should be rejected.
	EXPECT_FALSE(AreaB->TryToConnect(SourceInAreaA, 0, DestinationInAreaB, 1));

	// No new connection should appear in either area.
	EXPECT_EQ(AreaA->GetConnectionCount(), 0);
	EXPECT_EQ(AreaB->GetConnectionCount(), 0);

	NODE_SYSTEM.DeleteNodeArea(AreaA);
	NODE_SYSTEM.DeleteNodeArea(AreaB);
}

TEST(NodeSystemTests, TryToConnect_CorrectConnection_ButWrongArea_IsRejected)
{
	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();

	BoolLiteralNode* SourceInAreaA = new BoolLiteralNode();
	BoolVariableNode* DestinationInAreaA = new BoolVariableNode();
	AreaA->AddNode(SourceInAreaA);
	AreaA->AddNode(DestinationInAreaA);

	// Nodes SourceInAreaA and DestinationInAreaA both belong to AreaA.
	// AreaB should reject this.
	EXPECT_FALSE(AreaB->TryToConnect(SourceInAreaA, 0, DestinationInAreaA, 1));

	// No new connection should appear in either area.
	EXPECT_EQ(AreaA->GetConnectionCount(), 0);
	EXPECT_EQ(AreaB->GetConnectionCount(), 0);

	NODE_SYSTEM.DeleteNodeArea(AreaA);
	NODE_SYSTEM.DeleteNodeArea(AreaB);
}

TEST(NodeSystemTests, TryToConnect_NodesWithNoParent_IsRejected)
{
	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	BoolLiteralNode* NodeA = new BoolLiteralNode();
	BoolVariableNode* NodeB = new BoolVariableNode();

	// Nodes NodeA and NodeB do not belong to any area, so connection should be rejected.
	EXPECT_FALSE(AreaA->TryToConnect(NodeA, 0, NodeB, 1));

	// No new connection should appear.
	EXPECT_EQ(AreaA->GetConnectionCount(), 0);
	NODE_SYSTEM.DeleteNodeArea(AreaA);
}