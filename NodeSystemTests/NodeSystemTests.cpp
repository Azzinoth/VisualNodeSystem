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

TEST(NodeSystemTests, LinkAreas_Basic)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	std::string ParentID = ParentArea->GetID();
	ASSERT_NE(ParentArea, nullptr);

	NodeArea* ChildArea = NODE_SYSTEM.CreateNodeArea();
	std::string ChildID = ChildArea->GetID();
	ASSERT_NE(ChildArea, nullptr);

	// Invalid inputs should not cause a link to be created, and should return false.
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas("", ""), false);
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(ParentID, ""), false);
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas("", ParentID), false);
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(ParentID, ParentID), false);
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(ChildID, ChildID), false);

	// Valid inputs should create a link and return true, and output the IDs of the nodes involved in the link.
	std::pair<std::string, std::string> LinkResult;
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(ParentID, ChildID, &LinkResult), true);
	ASSERT_NE(LinkResult.first, "");
	ASSERT_NE(LinkResult.second, "");

	std::vector<std::pair<std::string, std::string>> LinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(ParentID, ChildID);
	EXPECT_EQ(LinkingNodes.size(), 1);

	// GetLinkingNodesForAreas outputs the IDs of the nodes involved in the link, so they should match the IDs returned by LinkNodeAreas.
	EXPECT_EQ(LinkResult.first, LinkingNodes[0].first);
	EXPECT_EQ(LinkResult.second, LinkingNodes[0].second);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
	EXPECT_EQ(ChildArea->GetNodeByID(LinkResult.second), nullptr);
	NODE_SYSTEM.DeleteNodeArea(ChildArea);
}

TEST(NodeSystemTests, LinkAreas_Deletion_Basic)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	std::string ParentID = ParentArea->GetID();
	ASSERT_NE(ParentArea, nullptr);

	NodeArea* ChildArea = NODE_SYSTEM.CreateNodeArea();
	std::string ChildID = ChildArea->GetID();
	ASSERT_NE(ChildArea, nullptr);

	std::pair<std::string, std::string > LinkResult;
	EXPECT_EQ(NODE_SYSTEM.LinkNodeAreas(ParentID, ChildID, &LinkResult), true);
	ASSERT_NE(LinkResult.first, "");
	ASSERT_NE(LinkResult.second, "");

	ParentArea->Delete(ParentArea->GetNodeByID(LinkResult.first));

	// Both nodes involved in the link should be deleted.
	EXPECT_EQ(ParentArea->GetNodeByID(LinkResult.first), nullptr);
	EXPECT_EQ(ChildArea->GetNodeByID(LinkResult.second), nullptr);

	std::vector<std::pair<std::string, std::string>> LinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(ParentID, ChildID);
	EXPECT_EQ(LinkingNodes.size(), 0);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
	NODE_SYSTEM.DeleteNodeArea(ChildArea);
}

TEST(NodeSystemTests, LinkAreas_Basic_Tiny_Graph)
{
	// Create a 3-level hierarchy (Parent => Child => Grandchild).
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	std::string ParentID = ParentArea->GetID();
	ASSERT_NE(ParentArea, nullptr);

	NodeArea* ChildArea = NODE_SYSTEM.CreateNodeArea();
	std::string ChildID = ChildArea->GetID();
	ASSERT_NE(ChildArea, nullptr);

	NodeArea* GrandchildArea = NODE_SYSTEM.CreateNodeArea();
	std::string GrandchildID = GrandchildArea->GetID();
	ASSERT_NE(GrandchildArea, nullptr);

	// Link Parent => Child.
	NODE_SYSTEM.LinkNodeAreas(ParentID, ChildID);

	// Parent: no upstream, Child is immediate and only downstream.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(ParentArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(ParentArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(ParentArea, { ChildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(ParentArea, { ChildArea }));

	// Child: Parent is upstream, no downstream yet.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(ChildArea, { ParentArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(ChildArea, { ParentArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(ChildArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(ChildArea, {}));

	// Link Child => Grandchild.
	NODE_SYSTEM.LinkNodeAreas(ChildID, GrandchildID);

	// Parent: immediate downstream is still just Child, but all downstream now includes Grandchild.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(ParentArea, { ChildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(ParentArea, { ChildArea, GrandchildArea }));

	// Child: Parent upstream, Grandchild downstream.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(ChildArea, { ParentArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(ChildArea, { GrandchildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(ChildArea, { GrandchildArea }));

	// Grandchild: both Parent and Child upstream, no downstream.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(GrandchildArea, { ChildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(GrandchildArea, { ChildArea, ParentArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(GrandchildArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(GrandchildArea, {}));

	// Removing Child should sever links.
	NODE_SYSTEM.DeleteNodeArea(ChildArea);

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(ParentArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(ParentArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(GrandchildArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(GrandchildArea, {}));

	// No dangling link nodes should remain.
	EXPECT_TRUE(TEST_TOOLS.VerifyNoLinkNodes(ParentArea));
	EXPECT_TRUE(TEST_TOOLS.VerifyNoLinkNodes(GrandchildArea));

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
	NODE_SYSTEM.DeleteNodeArea(GrandchildArea);
}

TEST(NodeSystemTests, LinkAreas_InfiniteLoop)
{
	// Create a loop (Parent => Child => Grandchild => Parent).
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	std::string ParentID = ParentArea->GetID();
	ASSERT_NE(ParentArea, nullptr);

	NodeArea* ChildArea = NODE_SYSTEM.CreateNodeArea();
	std::string ChildID = ChildArea->GetID();
	ASSERT_NE(ChildArea, nullptr);

	NodeArea* GrandchildArea = NODE_SYSTEM.CreateNodeArea();
	std::string GrandchildID = GrandchildArea->GetID();
	ASSERT_NE(GrandchildArea, nullptr);

	// Link Parent => Child.
	std::pair<std::string, std::string > LinkResult;
	EXPECT_TRUE(NODE_SYSTEM.LinkNodeAreas(ParentID, ChildID, &LinkResult));

	// Link Child => Grandchild.
	EXPECT_TRUE(NODE_SYSTEM.LinkNodeAreas(ChildID, GrandchildID, &LinkResult));

	// Link Grandchild => Parent.
	EXPECT_TRUE(NODE_SYSTEM.LinkNodeAreas(GrandchildID, ParentID, &LinkResult));

	// Test different levels of upstream/downstream retrieval to ensure no infinite loops and correct results.
	// Parent.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(ParentArea, { GrandchildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(ParentArea, { ChildArea, GrandchildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(ParentArea, { ChildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(ParentArea, { ChildArea, GrandchildArea }));

	// Child.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(ChildArea, { ParentArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(ChildArea, { ParentArea, GrandchildArea }));

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(ChildArea, { GrandchildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(ChildArea, { GrandchildArea, ParentArea }));

	// Grandchild.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(GrandchildArea, { ChildArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(GrandchildArea, { ChildArea, ParentArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(GrandchildArea, { ParentArea }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(GrandchildArea, { ParentArea, ChildArea }));

	// Removing Child should sever links.
	NODE_SYSTEM.DeleteNodeArea(ChildArea);

	NODE_SYSTEM.IsLinked(ParentID, ChildID);
	EXPECT_FALSE(NODE_SYSTEM.IsLinked(ParentID, ChildID));
	EXPECT_FALSE(NODE_SYSTEM.IsLinked(ChildID, GrandchildID));
	EXPECT_TRUE(NODE_SYSTEM.IsLinked(ParentID, GrandchildID));

	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(ParentArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(ParentArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(GrandchildArea, {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(GrandchildArea, {}));

	// No dangling link nodes should remain.
	EXPECT_TRUE(TEST_TOOLS.VerifyNoDanglingLinkNodes(ParentArea));
	EXPECT_TRUE(TEST_TOOLS.VerifyNoDanglingLinkNodes(GrandchildArea));

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
	NODE_SYSTEM.DeleteNodeArea(GrandchildArea);
}

TEST(NodeSystemTests, LinkAreas_Small_Graph)
{
	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallLinkedNodeAreaGraph();

	// Depth Level 0.
	// Root has no upstream, three immediate downstream.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[0], {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[0], {}));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[0], { Areas[1], Areas[2], Areas[3] }));

	// All downstream from root should be everything except root itself.
	std::vector<NodeArea*> AllExceptRoot(Areas.begin() + 1, Areas.end());
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[0], AllExceptRoot));

	// Depth Level 1.
	// Node 1.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[1], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[1], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[1], { Areas[4], Areas[5], Areas[6] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[1], { Areas[4], Areas[5], Areas[6], Areas[13], Areas[14], Areas[15], Areas[23], Areas[24] }));
	// Node 2.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[2], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[2], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[2], { Areas[7], Areas[8], Areas[9] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[2], { Areas[7], Areas[8], Areas[9], Areas[16], Areas[17], Areas[18], Areas[25], Areas[26] }));
	// Node 3.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[3], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[3], { Areas[0] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[3], { Areas[10], Areas[11], Areas[12] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[3], { Areas[10], Areas[11], Areas[12], Areas[19], Areas[20], Areas[21], Areas[22], Areas[27], Areas[28], Areas[29] }));

	// Depth Level 2.
	// Node 4.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[4], { Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[4], { Areas[0], Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[4], { Areas[13], Areas[14] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[4], { Areas[13], Areas[14], Areas[23] }));
	// Node 5.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[5], { Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[5], { Areas[0], Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[5], { Areas[15] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[5], { Areas[15], Areas[24] }));
	// Node 6.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[6], { Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[6], { Areas[0], Areas[1] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[6], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[6], { }));
	// Node 7.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[7], { Areas[2] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[7], { Areas[0], Areas[2] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[7], { Areas[16] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[7], { Areas[16], Areas[25] }));
	// Node 8.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[8], { Areas[2] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[8], { Areas[0], Areas[2] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[8], { Areas[17] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[8], { Areas[17] }));
	// Node 9.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[9], { Areas[2] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[9], { Areas[0], Areas[2] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[9], { Areas[18] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[9], { Areas[18], Areas[26] }));
	// Node 10.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[10], { Areas[3] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[10], { Areas[0], Areas[3] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[10], { Areas[19] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[10], { Areas[19] }));
	// Node 11.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[11], { Areas[3] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[11], { Areas[0], Areas[3] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[11], { Areas[20] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[11], { Areas[20], Areas[27] }));
	// Node 12.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[12], { Areas[3] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[12], { Areas[0], Areas[3] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[12], { Areas[21], Areas[22] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[12], { Areas[21], Areas[22], Areas[28], Areas[29] }));
	
	// Depth Level 3.
	// Node 13.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[13], { Areas[4] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[13], { Areas[0], Areas[1], Areas[4] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[13], { Areas[23] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[13], { Areas[23] }));
	// Node 14.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[14], { Areas[4] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[14], { Areas[0], Areas[1], Areas[4] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[14], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[14], { }));
	// Node 15.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[15], { Areas[5] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[15], { Areas[0], Areas[1], Areas[5] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[15], { Areas[24] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[15], { Areas[24] }));
	// Node 16.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[16], { Areas[7] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[16], { Areas[0], Areas[2], Areas[7] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[16], { Areas[25] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[16], { Areas[25] }));
	// Node 17.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[17], { Areas[8] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[17], { Areas[0], Areas[2], Areas[8] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[17], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[17], { }));
	// Node 18.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[18], { Areas[9] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[18], { Areas[0], Areas[2], Areas[9] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[18], { Areas[26] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[18], { Areas[26] }));
	// Node 19.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[19], { Areas[10] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[19], { Areas[0], Areas[3], Areas[10] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[19], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[19], { }));
	// Node 20.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[20], { Areas[11] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[20], { Areas[0], Areas[3], Areas[11] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[20], { Areas[27] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[20], { Areas[27] }));
	// Node 21.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[21], { Areas[12] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[21], { Areas[0], Areas[3], Areas[12] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[21], { Areas[28] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[21], { Areas[28] }));
	// Node 22.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[22], { Areas[12] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[22], { Areas[0], Areas[3], Areas[12] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[22], { Areas[29] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[22], { Areas[29] }));

	// Depth Level 4.
	// Node 23.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[23], { Areas[13] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[23], { Areas[0], Areas[1], Areas[4], Areas[13] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[23], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[23], { }));
	// Node 24.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[24], { Areas[15] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[24], { Areas[0], Areas[1], Areas[5], Areas[15] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[24], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[24], { }));
	// Node 25.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[25], { Areas[16] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[25], { Areas[0], Areas[2], Areas[7], Areas[16] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[25], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[25], { }));
	// Node 26.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[26], { Areas[18] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[26], { Areas[0], Areas[2], Areas[9], Areas[18] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[26], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[26], { }));
	// Node 27.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[27], { Areas[20] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[27], { Areas[0], Areas[3], Areas[11], Areas[20] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[27], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[27], { }));
	// Node 28.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[28], { Areas[21] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[28], { Areas[0], Areas[3], Areas[12], Areas[21] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[28], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[28], { }));
	// Node 29.
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[29], { Areas[22] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllUpstreamAreas(Areas[29], { Areas[0], Areas[3], Areas[12], Areas[22] }));
	EXPECT_TRUE(TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[29], { }));
	EXPECT_TRUE(TEST_TOOLS.VerifyAllDownstreamAreas(Areas[29], { }));

	// After verification of the graph structure, unlink some areas and verify the structure updates correctly.
	EXPECT_TRUE(NODE_SYSTEM.UnlinkNodeAreas(Areas[0]->GetID(), Areas[1]->GetID()));
	// Try to unlink again to ensure it fails gracefully and doesn't cause issues.
	EXPECT_FALSE(NODE_SYSTEM.UnlinkNodeAreas(Areas[0]->GetID(), Areas[1]->GetID()));

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
	
	// Clean up by deleting all areas.
	for (auto Area : Areas)
		NODE_SYSTEM.DeleteNodeArea(Area);
}