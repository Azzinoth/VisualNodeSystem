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

TEST(NodeSystemTests, MoveNodesTo_SameSourceAndTarget_Will_Do_Nothing)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	Node* NodeA = new Node();
	Node* NodeB = new Node();
	Area->AddNode(NodeA);
	Area->AddNode(NodeB);

	ASSERT_EQ(Area->GetNodeCount(), 2);
	EXPECT_FALSE(NODE_SYSTEM.MoveNodesTo(Area, Area));
	EXPECT_EQ(Area->GetNodeCount(), 2);

	NODE_SYSTEM.DeleteNodeArea(Area);
}

// When target's AddNode refuses a node MoveNodesTo must leave that node in source.
TEST(NodeSystemTests, MoveNodesTo_AddNodeRejection_NodeIsRetained_InSource)
{
	NODE_SYSTEM.Clear();

	NodeArea* TargetArea = NODE_SYSTEM.CreateNodeArea();

	// Snapshot a LinkNode pointing at a TargetArea via a throwaway donor area.
	NodeArea* DonorArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(DonorArea->GetID(), TargetArea->GetID()));
	std::string DonorJson = DonorArea->ToJson();
	NODE_SYSTEM.DeleteNodeArea(DonorArea);
	ASSERT_EQ(TargetArea->GetNodeCount(), 0);

	NodeArea* SourceArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_TRUE(SourceArea->LoadFromJson(DonorJson));
	ASSERT_EQ(SourceArea->GetNodeCount(), 1);

	NODE_SYSTEM.MoveNodesTo(SourceArea, TargetArea, false);

	// Rejected node must stay in source, target gains nothing.
	EXPECT_EQ(SourceArea->GetNodeCount(), 1);
	EXPECT_EQ(TargetArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, MoveNodesTo_AllNodesAccepted_TransfersThemAndPreservesPartialSelection)
{
	NODE_SYSTEM.Clear();

	NodeArea* SourceArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* TargetArea = NODE_SYSTEM.CreateNodeArea();

	// Pre-existing node in target, should NOT end up selected after the move.
	Node* PreExisting = new Node();
	ASSERT_TRUE(TargetArea->AddNode(PreExisting));

	// Two movable nodes in source.
	Node* MovableA = new Node();
	Node* MovableB = new Node();
	ASSERT_TRUE(SourceArea->AddNode(MovableA));
	ASSERT_TRUE(SourceArea->AddNode(MovableB));

	const std::string MovableAID = MovableA->GetID();
	const std::string MovableBID = MovableB->GetID();
	const std::string PreExistingID = PreExisting->GetID();

	ASSERT_TRUE(NODE_SYSTEM.MoveNodesTo(SourceArea, TargetArea, true));

	EXPECT_EQ(SourceArea->GetNodeCount(), 0);
	EXPECT_EQ(TargetArea->GetNodeCount(), 3);

	// Selection contains only the moved-in nodes.
	std::vector<Node*> Selected = TargetArea->GetSelected();
	EXPECT_EQ(Selected.size(), 2);
	for (Node* SelectedNode : Selected)
		EXPECT_NE(SelectedNode->GetID(), PreExistingID);

	// All three are now owned by target.
	EXPECT_EQ(TargetArea->GetNodeByID(MovableAID), MovableA);
	EXPECT_EQ(TargetArea->GetNodeByID(MovableBID), MovableB);
	EXPECT_EQ(MovableA->GetParentArea(), TargetArea);
	EXPECT_EQ(MovableB->GetParentArea(), TargetArea);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, GetTotalNodeCount_DuplicateAreaIDInFilter_DoesNotDoubleCount)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	BoolVariableNode* NodeA = new BoolVariableNode();
	BoolVariableNode* NodeB = new BoolVariableNode();
	Area->AddNode(NodeA);
	Area->AddNode(NodeB);
	ASSERT_EQ(Area->GetNodeCount(), 2);

	const std::string ID = Area->GetID();

	// Pass the same area ID twice, but it should not double count the nodes in that area.
	EXPECT_EQ(NODE_SYSTEM.GetTotalNodeCount({ ID, ID }), 2);

	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_EQ(Area->GetConnectionCount(), 1);
	// Check that connections are also not double counted when passing duplicate area IDs.
	EXPECT_EQ(NODE_SYSTEM.GetTotalConnectionCount({ ID, ID }), 1);

	GroupComment* Comment = new GroupComment();
	Area->AddGroupComment(Comment);
	EXPECT_EQ(NODE_SYSTEM.GetGroupCommentCount({ ID, ID }), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, TryToFixDanglingLinkNode_WithForceRestorePartner_RestoresCorrectAreaDirection)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();
	const std::string AreaAID = AreaA->GetID();
	const std::string AreaBID = AreaB->GetID();

	// Create a normal A=>B link. InNode lives in A, OutNode lives in B.
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(AreaAID, AreaBID));
	ASSERT_EQ(NODE_SYSTEM.GetDanglingLinkNodes().size(), 0);

	// Sanity check direction before any repair.
	ASSERT_EQ(NODE_SYSTEM.GetImmediateDownstreamAreas(AreaAID).size(), 1);
	ASSERT_EQ(NODE_SYSTEM.GetImmediateDownstreamAreas(AreaAID)[0]->GetID(), AreaBID);

	auto ParseJson = [](const std::string& Text, Json::Value& OutRoot) -> bool {
		Json::CharReaderBuilder Builder;
		JSONCPP_STRING Error;
		const std::unique_ptr<Json::CharReader> Reader(Builder.newCharReader());
		return Reader->parse(Text.c_str(), Text.c_str() + Text.size(), &OutRoot, &Error);
	};

	Json::StreamWriterBuilder WriterBuilder;
	Json::Value Root;
	ASSERT_TRUE(ParseJson(NODE_SYSTEM.ToJson(), Root));

	Json::Value AreaBRoot;
	ASSERT_TRUE(ParseJson(Root["NodeAreas"][AreaBID].asString(), AreaBRoot));
	// Clear all nodes from area B, leaving no partner node for the A=>B link.
	AreaBRoot["Nodes"] = Json::Value(Json::objectValue);
	Root["NodeAreas"][AreaBID] = Json::writeString(WriterBuilder, AreaBRoot);

	ASSERT_TRUE(NODE_SYSTEM.LoadFromJson(Json::writeString(WriterBuilder, Root)));
	ASSERT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 2);

	// After loading, InNode (in A) is dangling, area B exists but has no partner node.
	std::vector<LinkNode*> Dangling = NODE_SYSTEM.GetDanglingLinkNodes();
	ASSERT_EQ(Dangling.size(), 1);
	LinkNode* DanglingInNode = Dangling[0];
	ASSERT_TRUE(DanglingInNode->IsInputNode());

	// Force-recreate the missing partner OutNode in B.
	// This exercises the buggy InAreaID/OutAreaID swap.
	ASSERT_TRUE(NODE_SYSTEM.TryToFixDanglingLinkNode(DanglingInNode, true));
	ASSERT_EQ(NODE_SYSTEM.GetDanglingLinkNodes().size(), 0);

	// The A=>B direction must be preserved: A is upstream, B is downstream.
	const std::vector<NodeArea*> Downstream = NODE_SYSTEM.GetImmediateDownstreamAreas(AreaAID);
	EXPECT_EQ(Downstream.size(), 1);
	EXPECT_EQ(Downstream.empty() ? "" : Downstream[0]->GetID(), AreaBID);

	const std::vector<NodeArea*> Upstream = NODE_SYSTEM.GetImmediateUpstreamAreas(AreaBID);
	EXPECT_EQ(Upstream.size(), 1);
	EXPECT_EQ(Upstream.empty() ? "" : Upstream[0]->GetID(), AreaAID);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, CreateNodeArea_NullEntryInNodesVector_Is_Skipped)
{
	NODE_SYSTEM.Clear();

	std::vector<Node*> NodesWithNull = { nullptr };
	NodeArea* NewArea = NODE_SYSTEM.CreateNodeArea(NodesWithNull, {});

	ASSERT_NE(NewArea, nullptr);
	EXPECT_EQ(NewArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, CreateNodeArea_NullFirstAndInterspersedEntries_AreSkipped)
{
	NODE_SYSTEM.Clear();

	std::vector<Node*> NodesWithNulls;
	NodesWithNulls.push_back(nullptr);
	NodesWithNulls.push_back(new Node());
	NodesWithNulls.push_back(nullptr);
	NodesWithNulls.push_back(new Node());
	NodesWithNulls.push_back(new Node());
	NodesWithNulls.push_back(nullptr);

	NodeArea* NewArea = NODE_SYSTEM.CreateNodeArea(NodesWithNulls, {});

	ASSERT_NE(NewArea, nullptr);
	EXPECT_EQ(NewArea->GetNodeCount(), 3);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, CreateNodeArea_NullEntryInGroupCommentsVector_Is_Skipped)
{
	NODE_SYSTEM.Clear();

	std::vector<GroupComment*> GroupCommentsWithNull = { nullptr };
	NodeArea* NewArea = NODE_SYSTEM.CreateNodeArea({}, GroupCommentsWithNull);

	ASSERT_NE(NewArea, nullptr);
	EXPECT_EQ(NewArea->GetGroupCommentCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, CopyNodesTo_CopiesGroupComments_To_Target)
{
	NODE_SYSTEM.Clear();

	NodeArea* SourceArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* TargetArea = NODE_SYSTEM.CreateNodeArea();

	GroupComment* Original = new GroupComment();
	Original->SetCaption("source comment");
	ASSERT_TRUE(SourceArea->AddGroupComment(Original));
	ASSERT_EQ(SourceArea->GetGroupCommentCount(), 1);

	NODE_SYSTEM.CopyNodesTo(SourceArea, TargetArea);

	EXPECT_EQ(SourceArea->GetGroupCommentCount(), 1);
	EXPECT_EQ(TargetArea->GetGroupCommentCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, Paste_AllNodesRejectedByTarget_TargetNeverShrinksBelowSource)
{
	NODE_SYSTEM.Clear();

	NodeArea* TargetArea = NODE_SYSTEM.CreateNodeArea();

	// Snapshot a LinkNode pointing at TargetArea, then cascade-empty the target.
	NodeArea* DonorArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(DonorArea->GetID(), TargetArea->GetID()));
	std::string DonorJson = DonorArea->ToJson();
	NODE_SYSTEM.DeleteNodeArea(DonorArea);
	ASSERT_EQ(TargetArea->GetNodeCount(), 0);

	// Mirror the paste path's clipboard-like load.
	NodeArea* NewNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_TRUE(NewNodeArea->LoadFromJson(DonorJson));
	ASSERT_EQ(NewNodeArea->GetNodeCount(), 1);

	// LinkedAreaID matches TargetArea, so AddNode rejects the copy.
	const size_t TargetNodeCountBefore = TargetArea->GetNodeCount();
	NODE_SYSTEM.CopyNodesTo(NewNodeArea, TargetArea);
	EXPECT_EQ(TargetArea->GetNodeCount(), TargetNodeCountBefore);
	EXPECT_EQ(NewNodeArea->GetNodeCount(), 1);

	EXPECT_GE(TargetArea->GetNodeCount(), TargetNodeCountBefore);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, CopyNodesTo_SourceEqualsTarget_IsNoOp)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	BoolLiteralNode* NodeInArea = new BoolLiteralNode();
	ASSERT_TRUE(Area->AddNode(NodeInArea));
	const size_t NodeCountBefore = Area->GetNodeCount();

	NODE_SYSTEM.CopyNodesTo(Area, Area);
	EXPECT_EQ(Area->GetNodeCount(), NodeCountBefore);

	NODE_SYSTEM.Clear();
}