#include "NodeAreaEventSystemTests.h"
using namespace VisNodeSys;

TEST(NodeAreaEventSystemTests, RunOnEachNode_VisitsAllNodes)
{
	std::vector<std::string> NodesIDList;
	std::vector<std::string> GroupCommentsIDList;

	NodeArea* LocalNodeArea = TEST_TOOLS.CreateTinyPopulatedNodeArea(NodesIDList, GroupCommentsIDList);
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);

	int VisitedCounter = 0;
	std::vector<std::string> VisitedNodesIDList;
	auto RecordVisitedNodes = [&](Node* NodeToCount) {
		VisitedCounter++;
		VisitedNodesIDList.push_back(NodeToCount->GetID());
	};

	LocalNodeArea->RunOnEachNode(RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, LocalNodeArea->GetNodeCount());
	ASSERT_EQ(VisitedNodesIDList.size(), LocalNodeArea->GetNodeCount());
	for (size_t i = 0; i < VisitedNodesIDList.size(); i++)
	{
		// The order of visits is not guaranteed.
		for (size_t j = 0; j < NodesIDList.size(); j++)
		{
			if (VisitedNodesIDList[i] == NodesIDList[j])
				break;
			
			if (j == NodesIDList.size() - 1)
				FAIL() << "Node with ID " << VisitedNodesIDList[i] << " was not visited.";
		}
	}

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(NodeAreaEventSystemTests, RunOnEachConnectedNode_FollowsOutputConnections)
{
	std::vector<std::string> NodesIDList;

	NodeArea* LocalNodeArea = TEST_TOOLS.CreateSmallConnectedNodeArea(NodesIDList);
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 30);

	// Testing this hierarchy:
	//
	//                  0
	//          /       |       \
	//         1        2        3
	//       / | \    / | \    / | \
	//      4  5  6  7  8  9  10 11 12
	//     /\  |     |  |  |   |  |  |\
	//   13 14 15   16 17 18  19 20 21 22
	//   |     |     |     |      |  |  \
	//  23    24    25    26      27 28  29

	int VisitedCounter = 0;
	std::vector<std::string> VisitedNodesIDList;
	auto RecordVisitedNodes = [&](Node* NodeToCount) {
		VisitedCounter++;
		VisitedNodesIDList.push_back(NodeToCount->GetID());
	};

	auto FoundNodeWithIDInList = [&](std::string ID) {
		return std::find(VisitedNodesIDList.begin(), VisitedNodesIDList.end(), ID) != VisitedNodesIDList.end();
	};

	// Depth Level 0.
	// Node 0.
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[0]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, LocalNodeArea->GetNodeCount() - 1);
	ASSERT_EQ(VisitedNodesIDList.size(), LocalNodeArea->GetNodeCount() - 1);
	// All nodes except the root should be visited.
	for (size_t i = 0; i < VisitedNodesIDList.size(); i++)
	{
		// The order of visits is not guaranteed.
		for (size_t j = 1; j < NodesIDList.size(); j++)
		{
			if (VisitedNodesIDList[i] == NodesIDList[j])
				break;

			if (j == NodesIDList.size() - 1)
				FAIL() << "Node with ID " << VisitedNodesIDList[i] << " was not visited.";	
		}
	}

	// Depth Level 1.
	// Node 1.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[1]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 8);
	ASSERT_EQ(VisitedNodesIDList.size(), 8);
	// Nodes with index 4, 5, 6, 13, 14, 15, 23 and 24 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[4]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[5]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[6]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[13]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[14]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[15]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[23]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[24]), true);

	// Node 2.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[2]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 8);
	ASSERT_EQ(VisitedNodesIDList.size(), 8);
	// Nodes with index 7, 8, 9, 16, 17, 18, 25 and 26 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[7]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[8]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[9]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[16]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[17]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[18]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[25]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[26]), true);

	// Node 3.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[3]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 10);
	ASSERT_EQ(VisitedNodesIDList.size(), 10);
	// Nodes with index 10, 11, 12, 19, 20, 21, 22, 27, 28 and 29 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[10]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[11]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[12]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[19]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[20]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[21]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[22]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[27]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[28]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[29]), true);

	// Depth Level 2.
	// Node 4.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[4]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 3);
	ASSERT_EQ(VisitedNodesIDList.size(), 3);
	// Nodes with index 13, 14 and 23 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[13]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[14]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[23]), true);

	// Node 5.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[5]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 2);
	ASSERT_EQ(VisitedNodesIDList.size(), 2);
	// Nodes with index 15 and 24 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[15]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[24]), true);

	// Node 6.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[6]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 0);
	ASSERT_EQ(VisitedNodesIDList.size(), 0);

	// Node 7.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[7]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 2);
	ASSERT_EQ(VisitedNodesIDList.size(), 2);
	// Nodes with index 16 and 25 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[16]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[25]), true);

	// Node 8.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[8]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 17 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[17]), true);

	// Node 9.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[9]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 2);
	ASSERT_EQ(VisitedNodesIDList.size(), 2);
	// Nodes with index 18 and 26 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[18]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[26]), true);

	// Node 10.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[10]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 19 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[19]), true);

	// Node 11.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[11]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 2);
	ASSERT_EQ(VisitedNodesIDList.size(), 2);
	// Nodes with index 20 and 27 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[20]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[27]), true);

	// Node 12.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[12]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 4);
	ASSERT_EQ(VisitedNodesIDList.size(), 4);
	// Nodes with index 21, 22, 28 and 29 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[21]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[22]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[28]), true);
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[29]), true);

	// Depth Level 3.
	// Node 13.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[13]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 23 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[23]), true);

	// Node 14.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[14]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 0);
	ASSERT_EQ(VisitedNodesIDList.size(), 0);

	// Node 15.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[15]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 24 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[24]), true);

	// Node 16.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[16]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 25 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[25]), true);

	// Node 17.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[17]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 0);
	ASSERT_EQ(VisitedNodesIDList.size(), 0);

	// Node 18.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[18]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 26 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[26]), true);

	// Node 19.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[19]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 0);
	ASSERT_EQ(VisitedNodesIDList.size(), 0);

	// Node 20.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[20]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 27 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[27]), true);

	// Node 21.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[21]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 28 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[28]), true);

	// Node 22.
	VisitedCounter = 0;
	VisitedNodesIDList.clear();
	LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[22]), RecordVisitedNodes);
	ASSERT_EQ(VisitedCounter, 1);
	ASSERT_EQ(VisitedNodesIDList.size(), 1);
	// Node with index 29 should be visited.
	ASSERT_EQ(FoundNodeWithIDInList(NodesIDList[29]), true);

	// Depth Level 4.
	for (int i = 23; i <= 29; i++)
	{
		VisitedCounter = 0;
		VisitedNodesIDList.clear();
		LocalNodeArea->RunOnEachConnectedNode(LocalNodeArea->GetNodeByID(NodesIDList[i]), RecordVisitedNodes);
		ASSERT_EQ(VisitedCounter, 0);
		ASSERT_EQ(VisitedNodesIDList.size(), 0);
	}

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}