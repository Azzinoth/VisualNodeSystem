#include "NodeAreaEventSystemTests.h"
using namespace VisNodeSys;
#include "../CustomNodes/EventCountingNode.h"

TEST(NodeAreaEventSystemTests, Node_Events)
{
	std::vector<std::string> NodesIDList;
	std::vector<std::string> GroupCommentsIDList;

	NodeArea* LocalNodeArea = TEST_TOOLS.CreateTinyPopulatedNodeArea(NodesIDList, GroupCommentsIDList);
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);
	ASSERT_EQ(NodesIDList.size(), 11);
	ASSERT_EQ(GroupCommentsIDList.size(), 1);

	bool bFirstRemoveCall = true;
	Node* NodeToDelete = LocalNodeArea->GetNodeByID(NodesIDList[8]);
	ASSERT_NE(NodeToDelete, nullptr);
	bool bNodeRemovedEventCalled = false;
	LocalNodeArea->AddNodeEventCallback([&](Node* Node, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::DESTROYED)
		{
			if (bFirstRemoveCall)
			{
				bNodeRemovedEventCalled = true;
				ASSERT_EQ(Node->GetID(), NodeToDelete->GetID());
				bFirstRemoveCall = false;
			}
		}
	});
	LocalNodeArea->Delete(NodeToDelete);
	ASSERT_EQ(LocalNodeArea->GetNodeByID(NodesIDList[8]), nullptr);
	ASSERT_TRUE(bNodeRemovedEventCalled);

	Node* NodeToConnectFrom = LocalNodeArea->GetNodeByID(NodesIDList[1]);
	ASSERT_NE(NodeToConnectFrom, nullptr);
	Node* NodeToConnectTo = LocalNodeArea->GetNodeByID(NodesIDList[5]);
	ASSERT_NE(NodeToConnectTo, nullptr);

	bool bFirstCall = true;
	bool bNodeBeforeConnectedEventCalled = false;
	LocalNodeArea->AddNodeEventCallback([&](Node* Node, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::BEFORE_CONNECTED)
		{
			ASSERT_EQ(NodeToConnectTo->GetNodesConnectedToInput().size(), 0);
			ASSERT_EQ(NodeToConnectFrom->GetNodesConnectedToOutput().size(), 0);

			if (bFirstCall)
			{
				ASSERT_EQ(Node->GetID(), NodeToConnectFrom->GetID());
				bFirstCall = false;
			}
			else
			{
				bNodeBeforeConnectedEventCalled = true;
				ASSERT_EQ(Node->GetID(), NodeToConnectTo->GetID());
				bFirstCall = true;
			}
		}
	});

	bool bNodeAfterConnectedEventCalled = false;
	LocalNodeArea->AddNodeEventCallback([&](Node* Node, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::AFTER_CONNECTED)
		{
			ASSERT_EQ(NodeToConnectTo->GetNodesConnectedToInput().size(), 1);
			ASSERT_EQ(NodeToConnectFrom->GetNodesConnectedToOutput().size(), 1);

			if (bFirstCall)
			{
				ASSERT_EQ(Node->GetID(), NodeToConnectFrom->GetID());
				bFirstCall = false;
			}
			else
			{
				bNodeAfterConnectedEventCalled = true;
				ASSERT_EQ(Node->GetID(), NodeToConnectTo->GetID());
			}
		}
	});

	ASSERT_TRUE(LocalNodeArea->TryToConnect(NodeToConnectFrom, 0, NodeToConnectTo, 0));
	ASSERT_TRUE(bNodeBeforeConnectedEventCalled);
	ASSERT_TRUE(bNodeAfterConnectedEventCalled);

	bFirstCall = true;
	bool bNodeBeforeDisconnectedEventCalled = false;
	LocalNodeArea->AddNodeEventCallback([&](Node* Node, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::BEFORE_DISCONNECTED)
		{
			ASSERT_EQ(NodeToConnectTo->GetNodesConnectedToInput().size(), 1);
			ASSERT_EQ(NodeToConnectFrom->GetNodesConnectedToOutput().size(), 1);

			if (bFirstCall)
			{

				ASSERT_EQ(Node->GetID(), NodeToConnectFrom->GetID());
				bFirstCall = false;
			}
			else
			{
				ASSERT_EQ(Node->GetID(), NodeToConnectTo->GetID());
				bNodeBeforeDisconnectedEventCalled = true;
				bFirstCall = true;
			}
		}
	});

	bool bNodeAfterDisconnectedEventCalled = false;
	LocalNodeArea->AddNodeEventCallback([&](Node* Node, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::AFTER_DISCONNECTED)
		{
			ASSERT_EQ(NodeToConnectTo->GetNodesConnectedToInput().size(), 0);
			ASSERT_EQ(NodeToConnectFrom->GetNodesConnectedToOutput().size(), 0);

			if (bFirstCall)
			{
				ASSERT_EQ(Node->GetID(), NodeToConnectFrom->GetID());
				bFirstCall = false;
			}
			else
			{
				ASSERT_EQ(Node->GetID(), NodeToConnectTo->GetID());
				bNodeAfterDisconnectedEventCalled = true;
				bFirstCall = true;
			}
		}
	});

	ASSERT_TRUE(LocalNodeArea->TryToDisconnect(NodeToConnectFrom, 0, NodeToConnectTo, 0));
	ASSERT_TRUE(bNodeBeforeDisconnectedEventCalled);
	ASSERT_TRUE(bNodeAfterDisconnectedEventCalled);

	bFirstCall = true;
	bool bNodeDestroyedEventCalled = false;
	LocalNodeArea->AddNodeEventCallback([&](Node* Node, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::DESTROYED)
		{
			ASSERT_EQ(NodeToConnectTo->GetNodesConnectedToInput().size(), 0);
			ASSERT_EQ(NodeToConnectFrom->GetNodesConnectedToOutput().size(), 0);

			if (bFirstCall)
			{
				ASSERT_EQ(Node->GetID(), NodesIDList[0]);
				bFirstCall = false;
				bNodeDestroyedEventCalled = true;
			}
		}
	});

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
	ASSERT_TRUE(bNodeDestroyedEventCalled);
}

TEST(NodeAreaEventSystemTests, Node_CanConnect_Functionality)
{
	// To-Do
}

TEST(NodeAreaEventSystemTests, Node_SocketEvent_Functionality)
{
	// To-Do
}

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

TEST(NodeAreaEventSystemTests, LastExecutedNodes_IsInExecutionOrder_Manual)
{
	std::vector<std::string> NodesIDList;
	std::vector<std::string> GroupCommentsIDList;

	NodeArea* LocalNodeArea = TEST_TOOLS.CreateTinyPopulatedNodeArea(NodesIDList, GroupCommentsIDList);
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_GE(NodesIDList.size(), 3);

	LocalNodeArea->SetSaveExecutedNodes(true);

	// Connect Node 0 => Node 1 => Node 2.
	Node* FirstNode = LocalNodeArea->GetNodeByID(NodesIDList[0]);
	Node* SecondNode = LocalNodeArea->GetNodeByID(NodesIDList[1]);
	Node* ThirdNode = LocalNodeArea->GetNodeByID(NodesIDList[2]);
	ASSERT_NE(FirstNode, nullptr);
	ASSERT_NE(SecondNode, nullptr);
	ASSERT_NE(ThirdNode, nullptr);

	// Trigger execution events manually in a known order: First, Second, Third.
	LocalNodeArea->TriggerOrphanSocketEvent(FirstNode, EXECUTE);
	LocalNodeArea->TriggerOrphanSocketEvent(SecondNode, EXECUTE);
	LocalNodeArea->TriggerOrphanSocketEvent(ThirdNode, EXECUTE);

	std::vector<Node*> ExecutedNodes = LocalNodeArea->GetLastExecutedNodes();
	ASSERT_EQ(ExecutedNodes.size(), 3);

	// The first executed node should be first in the list.
	EXPECT_EQ(ExecutedNodes[0]->GetID(), FirstNode->GetID());
	EXPECT_EQ(ExecutedNodes[1]->GetID(), SecondNode->GetID());
	EXPECT_EQ(ExecutedNodes[2]->GetID(), ThirdNode->GetID());

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(NodeAreaEventSystemTests, LastExecutedNodes_IsInExecutionOrder_ThroughConnections)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	LocalNodeArea->SetSaveExecutedNodes(true);

	// Create 3 BoolVariableNode forming a simple chain.
	BoolVariableNode* NodeA = new BoolVariableNode();
	BoolVariableNode* NodeB = new BoolVariableNode();
	BoolVariableNode* NodeC = new BoolVariableNode();

	ASSERT_TRUE(LocalNodeArea->AddNode(NodeA));
	ASSERT_TRUE(LocalNodeArea->AddNode(NodeB));
	ASSERT_TRUE(LocalNodeArea->AddNode(NodeC));

	// Connect NodeA -> NodeB -> NodeC.
	ASSERT_TRUE(LocalNodeArea->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(NodeB, 0, NodeC, 0));

	ASSERT_TRUE(LocalNodeArea->SetExecutionEntryNode(NodeA));
	ASSERT_TRUE(LocalNodeArea->ExecuteNodeNetwork());

	std::vector<Node*> ExecutedNodes = LocalNodeArea->GetLastExecutedNodes();
	ASSERT_GE(ExecutedNodes.size(), 3);

	// First executed node should be first in the list.
	EXPECT_EQ(ExecutedNodes[0]->GetID(), NodeA->GetID());
	EXPECT_EQ(ExecutedNodes[1]->GetID(), NodeB->GetID());
	EXPECT_EQ(ExecutedNodes[2]->GetID(), NodeC->GetID());

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(NodeAreaEventSystemTests, RunOnEachConnectedNode_NullStartNode_DoesNotCrash)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	int Counter = 0;
	Area->RunOnEachConnectedNode(nullptr, [&](Node*) { Counter++; });
	EXPECT_EQ(Counter, 0);

	NODE_SYSTEM.DeleteNodeArea(Area);
}

//                  A
//          /       |       \
//         B        C        D
//          \      /
//           C (shared)
TEST(NodeAreaEventSystemTests, RunOnEachConnectedNode_SharedNode_VisitsAllNodesExactlyOnce)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	// NodeA: 3 outputs, 0 inputs
	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "outB", NodeSocket::SocketFlow::Output));
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "outC", NodeSocket::SocketFlow::Output));
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "outD", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	// NodeB: 1 input, 1 output  (B => C)
	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "out", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeB);

	// NodeC: 2 inputs, 0 outputs  (receives from A and from B)
	Node* NodeC = new Node();
	NodeC->AddSocket(new NodeSocket(NodeC, "EXECUTE", "inA", NodeSocket::SocketFlow::Input));
	NodeC->AddSocket(new NodeSocket(NodeC, "EXECUTE", "inB", NodeSocket::SocketFlow::Input));
	Area->AddNode(NodeC);

	// NodeD: 1 input, 0 outputs  (receives from A only)
	Node* NodeD = new Node();
	NodeD->AddSocket(new NodeSocket(NodeD, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
	Area->AddNode(NodeD);

	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0)); // A.outB => B.in
	ASSERT_TRUE(Area->TryToConnect(NodeA, 1, NodeC, 0)); // A.outC => C.inA
	ASSERT_TRUE(Area->TryToConnect(NodeA, 2, NodeD, 0)); // A.outD => D.in
	ASSERT_TRUE(Area->TryToConnect(NodeB, 0, NodeC, 1)); // B.out  => C.inB

	std::vector<std::string> VisitedIDs;
	Area->RunOnEachConnectedNode(NodeA, [&](Node* CurrentNode) {
		VisitedIDs.push_back(CurrentNode->GetID());
	});

	auto WasVisited = [&](Node* CurrentNode) {
		return std::find(VisitedIDs.begin(), VisitedIDs.end(), CurrentNode->GetID()) != VisitedIDs.end();
	};

	// All three downstream nodes must be visited exactly once.
	EXPECT_EQ(VisitedIDs.size(), 3);
	EXPECT_TRUE(WasVisited(NodeB));
	EXPECT_TRUE(WasVisited(NodeC));
	EXPECT_TRUE(WasVisited(NodeD));

	NODE_SYSTEM.DeleteNodeArea(Area);
}

//              A
//            /   \
//           B     C
//            \   /
//              D
//
TEST(NodeAreaEventSystemTests, RunOnEachConnectedNode_Diamond_VisitsEachOnce)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "outB", NodeSocket::SocketFlow::Output));
	NodeA->AddSocket(new NodeSocket(NodeA, "EXECUTE", "outC", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
	NodeB->AddSocket(new NodeSocket(NodeB, "EXECUTE", "out", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeB);

	Node* NodeC = new Node();
	NodeC->AddSocket(new NodeSocket(NodeC, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
	NodeC->AddSocket(new NodeSocket(NodeC, "EXECUTE", "out", NodeSocket::SocketFlow::Output));
	Area->AddNode(NodeC);

	Node* NodeD = new Node();
	NodeD->AddSocket(new NodeSocket(NodeD, "EXECUTE", "inB", NodeSocket::SocketFlow::Input));
	NodeD->AddSocket(new NodeSocket(NodeD, "EXECUTE", "inC", NodeSocket::SocketFlow::Input));
	Area->AddNode(NodeD);

	ASSERT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 0));
	ASSERT_TRUE(Area->TryToConnect(NodeA, 1, NodeC, 0));
	ASSERT_TRUE(Area->TryToConnect(NodeB, 0, NodeD, 0));
	ASSERT_TRUE(Area->TryToConnect(NodeC, 0, NodeD, 1));

	std::vector<std::string> VisitedIDs;
	Area->RunOnEachConnectedNode(NodeA, [&](Node* CurrentNode) {
		VisitedIDs.push_back(CurrentNode->GetID());
	});

	EXPECT_EQ(VisitedIDs.size(), 3);

	auto WasVisited = [&](Node* CurrentNode) {
		return std::find(VisitedIDs.begin(), VisitedIDs.end(), CurrentNode->GetID()) != VisitedIDs.end();
	};

	EXPECT_TRUE(WasVisited(NodeB));
	EXPECT_TRUE(WasVisited(NodeC));
	EXPECT_TRUE(WasVisited(NodeD));

	NODE_SYSTEM.DeleteNodeArea(Area);
}

TEST(NodeAreaEventSystemTests, Delete_Connection_OutSocketNodeReceivesDisconnectedEvent)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	EventCountingNode* OutNode = new EventCountingNode();
	OutNode->AddSocket(new NodeSocket(OutNode, "T", "out", NodeSocket::SocketFlow::Output));

	EventCountingNode* InNode = new EventCountingNode();
	InNode->AddSocket(new NodeSocket(InNode, "T", "in", NodeSocket::SocketFlow::Input));

	Area->AddNode(OutNode);
	Area->AddNode(InNode);
	ASSERT_TRUE(Area->TryToConnect(OutNode, 0, InNode, 0));
	ASSERT_EQ(Area->GetConnectionCount(), 1u);

	// Both nodes should have received CONNECTED on the connect.
	EXPECT_EQ(InNode->GetConnectedCount(), 1);
	EXPECT_EQ(OutNode->GetConnectedCount(), 1);

	std::string OutSocketID = OutNode->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output);
	ASSERT_TRUE(Area->TryToDisconnect(OutNode, OutSocketID));

	// In socket's parent gets the event.
	EXPECT_EQ(InNode->GetDisconnectedCount(), 1);

	// Out socket's parent should also receive DISCONNECTED.
	EXPECT_EQ(OutNode->GetDisconnectedCount(), 1);

	NODE_SYSTEM.DeleteNodeArea(Area);
}

TEST(NodeAreaEventSystemTests, Clear_EveryNodeReceivesDestroyed_EvenWhenCallbackDeletesOthers)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	Node* NodeA = new Node();
	Node* NodeB = new Node();
	Node* NodeC = new Node();
	ASSERT_TRUE(Area->AddNode(NodeA));
	ASSERT_TRUE(Area->AddNode(NodeB));
	ASSERT_TRUE(Area->AddNode(NodeC));

	std::string NodeAID = NodeA->GetID();
	std::string NodeBID = NodeB->GetID();
	std::string NodeCID = NodeC->GetID();

	std::vector<std::string> DestroyedIDs;
	Area->AddNodeEventCallback([&](Node* Node, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::DESTROYED)
		{
			DestroyedIDs.push_back(Node->GetID());

			// When the first node is being destroyed, simulate a listener that
			// deletes another node in the area.
			if (Node->GetID() == NodeAID && Area->GetNodeByID(NodeBID) != nullptr)
				Area->Delete(Area->GetNodeByID(NodeBID));
		}
	});

	Area->Clear();

	// All three nodes were in the area when Clear() started, so all three should have received DESTROYED.
	EXPECT_EQ(DestroyedIDs.size(), 3u);
	EXPECT_NE(std::find(DestroyedIDs.begin(), DestroyedIDs.end(), NodeAID), DestroyedIDs.end());
	EXPECT_NE(std::find(DestroyedIDs.begin(), DestroyedIDs.end(), NodeBID), DestroyedIDs.end());
	EXPECT_NE(std::find(DestroyedIDs.begin(), DestroyedIDs.end(), NodeCID), DestroyedIDs.end());

	NODE_SYSTEM.DeleteNodeArea(Area);
}