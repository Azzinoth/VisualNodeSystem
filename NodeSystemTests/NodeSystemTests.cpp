#include "NodeSystemTests.h"
using namespace VisNodeSys;
#include "../CustomNodes/DivergentCopyNode.h"

TEST(NodeSystemTests, CreateAndDelete)
{
	NODE_SYSTEM.Clear();

	std::vector<std::string> NodeAreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	EXPECT_EQ(NodeAreaIDs.size(), 0);

	NodeArea* NewNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NewNodeArea, nullptr);

	NodeAreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	EXPECT_EQ(NodeAreaIDs.size(), 1);
	EXPECT_EQ(NodeAreaIDs[0] == NewNodeArea->GetID(), true);

	NODE_SYSTEM.Clear();

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
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, TryToConnect_CorrectConnection_ButWrongArea_IsRejected)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, TryToConnect_NodesWithNoParent_IsRejected)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	BoolLiteralNode* NodeA = new BoolLiteralNode();
	BoolVariableNode* NodeB = new BoolVariableNode();

	// Nodes NodeA and NodeB do not belong to any area, so connection should be rejected.
	EXPECT_FALSE(AreaA->TryToConnect(NodeA, 0, NodeB, 1));

	// No new connection should appear.
	EXPECT_EQ(AreaA->GetConnectionCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, MoveNodesTo_SameSourceAndTarget_Will_Do_Nothing)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	Node* NodeA = new Node();
	Node* NodeB = new Node();
	Area->AddNode(NodeA);
	Area->AddNode(NodeB);

	ASSERT_EQ(Area->GetNodeCount(), 2);
	EXPECT_FALSE(NODE_SYSTEM.MoveNodesTo(Area, Area));
	EXPECT_EQ(Area->GetNodeCount(), 2);

	NODE_SYSTEM.Clear();
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

TEST(NodeSystemTests, MoveNodesTo_StraddlingConnection_IsSevered)
{
	NODE_SYSTEM.Clear();

	NodeArea* SourceArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* TargetArea = NODE_SYSTEM.CreateNodeArea();

	Node* CollidingNode = new Node("DuplicateID");
	CollidingNode->AddSocket(new NodeSocket(CollidingNode, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(SourceArea->AddNode(CollidingNode));

	Node* NodeThatWillChangeArea = new Node();
	NodeThatWillChangeArea->AddSocket(new NodeSocket(NodeThatWillChangeArea, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(SourceArea->AddNode(NodeThatWillChangeArea));

	ASSERT_TRUE(SourceArea->TryToConnect(CollidingNode, 0, NodeThatWillChangeArea, 0));
	ASSERT_EQ(SourceArea->GetConnectionCount(), 1);

	// TargetArea already owns a node with the same ID, so it will be rejectd.
	Node* BlockerNode = new Node("DuplicateID");
	ASSERT_TRUE(TargetArea->AddNode(BlockerNode));

	ASSERT_TRUE(NODE_SYSTEM.MoveNodesTo(SourceArea, TargetArea));

	EXPECT_EQ(CollidingNode->GetParentArea(), SourceArea);
	EXPECT_EQ(NodeThatWillChangeArea->GetParentArea(), TargetArea);

	// The straddling connection is deleted, both at the area level and at the
	// socket level (the sockets must no longer reference each other across areas).
	EXPECT_EQ(SourceArea->GetConnectionCount(), 0);
	EXPECT_FALSE(SourceArea->IsConnected(CollidingNode, NodeThatWillChangeArea));
	EXPECT_TRUE(CollidingNode->GetNodesConnectedToOutput().empty());
	EXPECT_TRUE(NodeThatWillChangeArea->GetNodesConnectedToInput().empty());

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, MoveNodesTo_DropsMovedElementsFromSourceSelection)
{
	NODE_SYSTEM.Clear();

	NodeArea* SourceArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea* TargetArea = NODE_SYSTEM.CreateNodeArea();

	Node* OutputNode = new Node();
	OutputNode->AddSocket(new NodeSocket(OutputNode, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(SourceArea->AddNode(OutputNode));

	Node* InputNode = new Node();
	InputNode->AddSocket(new NodeSocket(InputNode, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(SourceArea->AddNode(InputNode));

	ASSERT_TRUE(SourceArea->TryToConnect(OutputNode, 0, InputNode, 0));

	// Select a node and a reroute on the connection, both of which will move.
	RerouteNode* Reroute = SourceArea->AddRerouteNodeToConnection(OutputNode, 0, InputNode, 0, 0, ImVec2(10.0f, 10.0f));
	ASSERT_NE(Reroute, nullptr);
	ASSERT_TRUE(SourceArea->AddSelected(OutputNode));
	ASSERT_TRUE(SourceArea->AddSelected(Reroute));
	ASSERT_TRUE(SourceArea->IsSelected(OutputNode));
	ASSERT_TRUE(SourceArea->IsSelected(Reroute));

	ASSERT_TRUE(NODE_SYSTEM.MoveNodesTo(SourceArea, TargetArea, false));

	// Both the node and its connection (with the reroute) moved to the target.
	EXPECT_EQ(OutputNode->GetParentArea(), TargetArea);
	EXPECT_EQ(SourceArea->GetConnectionCount(), 0);

	// The source area no longer references the moved node or reroute in its selection.
	EXPECT_FALSE(SourceArea->IsSelected(OutputNode));
	EXPECT_FALSE(SourceArea->IsSelected(Reroute));
	EXPECT_TRUE(SourceArea->GetSelected().empty());

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

TEST(NodeSystemTests, CreateNodeArea_NodesSpanningMultipleAreas_RecreatesAllConnections)
{
	NODE_SYSTEM.Clear();

	// Two independent areas, each holding its own internal connection.
	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();

	BoolVariableNode* SourceInAreaA = new BoolVariableNode();
	BoolVariableNode* DestinationInAreaA = new BoolVariableNode();
	ASSERT_TRUE(AreaA->AddNode(SourceInAreaA));
	ASSERT_TRUE(AreaA->AddNode(DestinationInAreaA));
	ASSERT_TRUE(AreaA->TryToConnect(SourceInAreaA, 0, DestinationInAreaA, 0));

	BoolVariableNode* SourceInAreaB = new BoolVariableNode();
	BoolVariableNode* DestinationInAreaB = new BoolVariableNode();
	ASSERT_TRUE(AreaB->AddNode(SourceInAreaB));
	ASSERT_TRUE(AreaB->AddNode(DestinationInAreaB));
	ASSERT_TRUE(AreaB->TryToConnect(SourceInAreaB, 0, DestinationInAreaB, 0));

	// Copying nodes from more than one area should work fine.
	std::vector<Node*> NodesFromBothAreas = { SourceInAreaA, DestinationInAreaA, SourceInAreaB, DestinationInAreaB };
	NodeArea* MergedArea = NODE_SYSTEM.CreateNodeArea(NodesFromBothAreas, {});

	ASSERT_NE(MergedArea, nullptr);
	EXPECT_EQ(MergedArea->GetNodeCount(), 4);
	// Both connections are recreated in the merged area.
	EXPECT_EQ(MergedArea->GetConnectionCount(), 2);

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

TEST(NodeSystemTests, CopyNodesTo_NodeWithFaultyCopyConstructor_IsRejected)
{
	NODE_SYSTEM.Clear();

	NodeArea* SourceNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(SourceNodeArea, nullptr);
	NodeArea* TargetNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TargetNodeArea, nullptr);

	DivergentCopyNode* DivergentNode = new DivergentCopyNode();
	ASSERT_EQ(DivergentNode->GetInputSocketCount(), 2);
	ASSERT_TRUE(SourceNodeArea->AddNode(DivergentNode));

	// The copy has fewer sockets than the source, so it is rejected, not copied.
	NODE_SYSTEM.CopyNodesTo(SourceNodeArea, TargetNodeArea);
	EXPECT_EQ(TargetNodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, CopyNodesTo_AfterSocketDeleted_CopyKeepsRemainingSockets)
{
	NODE_SYSTEM.Clear();

	NodeArea* SourceNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(SourceNodeArea, nullptr);
	NodeArea* TargetNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TargetNodeArea, nullptr);

	// Node created with three input sockets.
	Node* OriginalNode = new Node();
	NodeSocket* FirstSocket = new NodeSocket(OriginalNode, "INT", "A", NodeSocket::SocketFlow::Input);
	OriginalNode->AddSocket(FirstSocket);
	OriginalNode->AddSocket(new NodeSocket(OriginalNode, "INT", "B", NodeSocket::SocketFlow::Input));
	OriginalNode->AddSocket(new NodeSocket(OriginalNode, "INT", "C", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(SourceNodeArea->AddNode(OriginalNode));
	ASSERT_EQ(OriginalNode->GetInputSocketCount(), 3);

	// One socket is deleted, leaving two.
	ASSERT_TRUE(OriginalNode->DeleteSocket(FirstSocket));
	ASSERT_EQ(OriginalNode->GetInputSocketCount(), 2);

	NODE_SYSTEM.CopyNodesTo(SourceNodeArea, TargetNodeArea);
	ASSERT_EQ(TargetNodeArea->GetNodeCount(), 1);

	std::vector<Node*> CopiedNodes = TargetNodeArea->GetNodesByStringType("VisualNode");
	ASSERT_EQ(CopiedNodes.size(), 1);
	EXPECT_EQ(CopiedNodes[0]->GetInputSocketCount(), 2);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemTests, CopyNodesTo_CallbackConnectsOtherPair_ReroutesStayOnCopiedConnection)
{
	NODE_SYSTEM.Clear();

	NodeArea* SourceArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(SourceArea, nullptr);

	Node* NodeA = new Node();
	NodeA->SetName("NodeA");
	NodeA->AddSocket(new NodeSocket(NodeA, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(SourceArea->AddNode(NodeA));

	Node* NodeB = new Node();
	NodeB->SetName("NodeB");
	NodeB->AddSocket(new NodeSocket(NodeB, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(SourceArea->AddNode(NodeB));

	ASSERT_TRUE(SourceArea->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_NE(SourceArea->AddRerouteNodeToConnection(NodeA, 0, NodeB, 0, 0, ImVec2(100.0f, 100.0f)), nullptr);
	ASSERT_EQ(SourceArea->GetRerouteConnectionCount(), 1);

	NodeArea* TargetArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TargetArea, nullptr);

	// Two unconnected nodes that the callback will connect during the copy.
	Node* NodeX = new Node();
	NodeX->SetName("NodeX");
	NodeX->AddSocket(new NodeSocket(NodeX, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(TargetArea->AddNode(NodeX));

	Node* NodeY = new Node();
	NodeY->SetName("NodeY");
	NodeY->AddSocket(new NodeSocket(NodeY, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(TargetArea->AddNode(NodeY));

	bool bCallbackConnectIssued = false;
	TargetArea->AddNodeEventCallback([&](Node*, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::AFTER_CONNECTED && !bCallbackConnectIssued)
		{
			bCallbackConnectIssued = true;
			TargetArea->TryToConnect(NodeX, 0, NodeY, 0);
		}
	});

	NODE_SYSTEM.CopyNodesTo(SourceArea, TargetArea);
	EXPECT_TRUE(bCallbackConnectIssued);
	EXPECT_EQ(TargetArea->GetConnectionCount(), 2);

	std::vector<Node*> CopiedANodes = TargetArea->GetNodesByName("NodeA");
	std::vector<Node*> CopiedBNodes = TargetArea->GetNodesByName("NodeB");
	ASSERT_EQ(CopiedANodes.size(), 1);
	ASSERT_EQ(CopiedBNodes.size(), 1);

	// The copied reroute belongs to the copied connection, segment count = reroutes + 1.
	EXPECT_EQ(TargetArea->GetConnectionSegments(CopiedANodes[0], 0, CopiedBNodes[0], 0).size(), 2);
	EXPECT_EQ(TargetArea->GetConnectionSegments(NodeX, 0, NodeY, 0).size(), 1);
	EXPECT_EQ(TargetArea->GetRerouteConnectionCount(), 1);

	NODE_SYSTEM.Clear();
}