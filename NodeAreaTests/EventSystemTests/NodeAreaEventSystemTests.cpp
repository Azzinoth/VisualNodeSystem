#include "NodeAreaEventSystemTests.h"
using namespace VisNodeSys;
#include "../CustomNodes/EventCountingNode.h"

TEST(NodeAreaEventSystemTests, Node_Events)
{
	NODE_SYSTEM.Clear();

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

TEST(NodeAreaEventSystemTests, ReentrantConnectionDelete_FromBeforeDisconnectedCallback_IsSafe)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	IntegerLiteralNode* OutNode = new IntegerLiteralNode();
	ASSERT_TRUE(LocalNodeArea->AddNode(OutNode));

	IntegerVariableNode* InNode = new IntegerVariableNode();
	ASSERT_TRUE(LocalNodeArea->AddNode(InNode));

	// Integer data connection: OutNode output => InNode input.
	ASSERT_TRUE(LocalNodeArea->TryToConnect(OutNode, 0, InNode, 1));
	ASSERT_EQ(LocalNodeArea->GetConnectionCount(), 1);

	const std::string InNodeID = InNode->GetID();

	// A callback that deletes one of the connection's endpoint.
	bool bBeforeDisconnectedCalled = false;
	LocalNodeArea->AddNodeEventCallback([&](Node*, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::BEFORE_DISCONNECTED && !bBeforeDisconnectedCalled)
		{
			bBeforeDisconnectedCalled = true;
			LocalNodeArea->Delete(InNode);
		}
	});

	ASSERT_TRUE(LocalNodeArea->TryToDisconnect(OutNode, 0, InNode, 1));

	EXPECT_TRUE(bBeforeDisconnectedCalled);
	EXPECT_EQ(LocalNodeArea->GetConnectionCount(), 0);
	EXPECT_EQ(LocalNodeArea->GetNodeByID(InNodeID), nullptr);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, SiblingConnectionDelete_FromBeforeDisconnectedCallback_IsSafe)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* SourceNode = new Node();
	SourceNode->AddSocket(new NodeSocket(SourceNode, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(SourceNode));

	Node* FirstTarget = new Node();
	FirstTarget->AddSocket(new NodeSocket(FirstTarget, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(LocalNodeArea->AddNode(FirstTarget));

	Node* SecondTarget = new Node();
	SecondTarget->AddSocket(new NodeSocket(SecondTarget, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(LocalNodeArea->AddNode(SecondTarget));

	// Two connections share SourceNode's output socket.
	ASSERT_TRUE(LocalNodeArea->TryToConnect(SourceNode, 0, FirstTarget, 0));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(SourceNode, 0, SecondTarget, 0));
	ASSERT_EQ(LocalNodeArea->GetConnectionCount(), 2);

	// During the first disconnect, the callback disconnects the SIBLING connection, which the outer snapshot loop still references.
	bool bSiblingDisconnected = false;
	LocalNodeArea->AddNodeEventCallback([&](Node*, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::BEFORE_DISCONNECTED && !bSiblingDisconnected)
		{
			bSiblingDisconnected = true;
			LocalNodeArea->TryToDisconnect(SourceNode, 0, SecondTarget, 0);
		}
	});

	// Disconnect everything on SourceNode's output socket.
	const std::string SocketID = SourceNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Output)->GetID();
	ASSERT_TRUE(LocalNodeArea->TryToDisconnect(SourceNode, SocketID));

	// The reentrancy ran and the area is left consistent, with no dangling sockets.
	EXPECT_TRUE(bSiblingDisconnected);
	EXPECT_EQ(LocalNodeArea->GetConnectionCount(), 0);
	EXPECT_TRUE(SourceNode->GetNodesConnectedToOutput().empty());
	EXPECT_TRUE(FirstTarget->GetNodesConnectedToInput().empty());
	EXPECT_TRUE(SecondTarget->GetNodesConnectedToInput().empty());

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, ReentrantNodeDelete_FromDestroyedCallback_IsSafe)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* TargetNode = new Node();
	TargetNode->AddSocket(new NodeSocket(TargetNode, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(LocalNodeArea->AddNode(TargetNode));

	Node* OtherNode = new Node();
	OtherNode->AddSocket(new NodeSocket(OtherNode, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(OtherNode));

	ASSERT_TRUE(LocalNodeArea->TryToConnect(OtherNode, 0, TargetNode, 0));
	ASSERT_EQ(LocalNodeArea->GetConnectionCount(), 1);

	const std::string TargetNodeID = TargetNode->GetID();

	// The DESTROYED callback re-enters Delete on the same node, freeing it before the outer Delete continues.
	bool bReentered = false;
	LocalNodeArea->AddNodeEventCallback([&](Node* EventNode, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::DESTROYED && !bReentered)
		{
			bReentered = true;
			LocalNodeArea->Delete(EventNode);
		}
	});

	LocalNodeArea->Delete(TargetNode);

	// The reentrancy ran, the node was deleted exactly once and the area is intact.
	EXPECT_TRUE(bReentered);
	EXPECT_EQ(LocalNodeArea->GetNodeByID(TargetNodeID), nullptr);
	EXPECT_EQ(LocalNodeArea->GetNodeCount(), 1);
	EXPECT_EQ(LocalNodeArea->GetConnectionCount(), 0);
	EXPECT_TRUE(OtherNode->GetNodesConnectedToOutput().empty());

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, ReentrantNodeDelete_FromBeforeDisconnectedCallback_IsSafe)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* TargetNode = new Node();
	TargetNode->AddSocket(new NodeSocket(TargetNode, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(LocalNodeArea->AddNode(TargetNode));

	Node* OtherNode = new Node();
	OtherNode->AddSocket(new NodeSocket(OtherNode, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(OtherNode));

	ASSERT_TRUE(LocalNodeArea->TryToConnect(OtherNode, 0, TargetNode, 0));
	ASSERT_EQ(LocalNodeArea->GetConnectionCount(), 1);

	const std::string TargetNodeID = TargetNode->GetID();

	// A BEFORE_DISCONNECTED callback fires while the node's connections are torn down inside Delete.
	// re-deleting the same node there must not leave the teardown loop operating on the freed node.
	bool bReentered = false;
	LocalNodeArea->AddNodeEventCallback([&](Node* EventNode, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::BEFORE_DISCONNECTED && !bReentered)
		{
			bReentered = true;
			Node* NodeToDelete = LocalNodeArea->GetNodeByID(TargetNodeID);
			if (NodeToDelete != nullptr)
				LocalNodeArea->Delete(NodeToDelete);
		}
	});

	LocalNodeArea->Delete(TargetNode);

	// The reentrancy ran, the node was deleted exactly once and the area is intact.
	EXPECT_TRUE(bReentered);
	EXPECT_EQ(LocalNodeArea->GetNodeByID(TargetNodeID), nullptr);
	EXPECT_EQ(LocalNodeArea->GetNodeCount(), 1);
	EXPECT_EQ(LocalNodeArea->GetConnectionCount(), 0);
	EXPECT_TRUE(OtherNode->GetNodesConnectedToOutput().empty());

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, PropagateUpdate_NotifiesNeighborsNotCaller)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	EventCountingNode* UpstreamNode = new EventCountingNode();
	UpstreamNode->AddSocket(new NodeSocket(UpstreamNode, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(UpstreamNode));

	EventCountingNode* CallerNode = new EventCountingNode();
	CallerNode->AddSocket(new NodeSocket(CallerNode, "T", "in", NodeSocket::SocketFlow::Input));
	CallerNode->AddSocket(new NodeSocket(CallerNode, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(CallerNode));

	EventCountingNode* DownstreamNode = new EventCountingNode();
	DownstreamNode->AddSocket(new NodeSocket(DownstreamNode, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(LocalNodeArea->AddNode(DownstreamNode));

	// Upstream => Caller (input side) and Caller => Downstream (output side).
	ASSERT_TRUE(LocalNodeArea->TryToConnect(UpstreamNode, 0, CallerNode, 0));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(CallerNode, 0, DownstreamNode, 0));

	UpstreamNode->ResetCounters();
	CallerNode->ResetCounters();
	DownstreamNode->ResetCounters();
	LocalNodeArea->PropagateUpdateToConnectedNodes(CallerNode);

	// Both neighbors are notified exactly once, the caller never notifies itself.
	EXPECT_EQ(UpstreamNode->GetUpdateCount(), 1);
	EXPECT_EQ(DownstreamNode->GetUpdateCount(), 1);
	EXPECT_EQ(CallerNode->GetUpdateCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, Node_CanConnect_Functionality)
{
	NODE_SYSTEM.Clear();

	// To-Do
}

TEST(NodeAreaEventSystemTests, Node_SocketEvent_Functionality)
{
	NODE_SYSTEM.Clear();

	// To-Do
}

TEST(NodeAreaEventSystemTests, RunOnEachNode_VisitsAllNodes)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, RunOnEachConnectedNode_FollowsOutputConnections)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, LastExecutedNodes_IsInExecutionOrder_Manual)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, LastExecutedNodes_IsInExecutionOrder_ThroughConnections)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, LastExecutedNodes_ExcludesDeletedNode)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	LocalNodeArea->SetSaveExecutedNodes(true);

	Node* ExecutedNode = new Node();
	ExecutedNode->AddSocket(new NodeSocket(ExecutedNode, "EXECUTE", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(ExecutedNode));
	ASSERT_TRUE(LocalNodeArea->SetExecutionEntryNode(ExecutedNode));

	ASSERT_TRUE(LocalNodeArea->ExecuteNodeNetwork());

	std::vector<Node*> Before = LocalNodeArea->GetLastExecutedNodes();
	ASSERT_FALSE(Before.empty());
	Node* ExecutedPtr = Before[0];
	ASSERT_EQ(ExecutedPtr, ExecutedNode);

	ASSERT_TRUE(LocalNodeArea->Delete(ExecutedNode));

	// The freed pointer must no longer be in return values.
	std::vector<Node*> After = LocalNodeArea->GetLastExecutedNodes();
	EXPECT_EQ(std::find(After.begin(), After.end(), ExecutedPtr), After.end());

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, RunOnEachConnectedNode_NullStartNode_DoesNotCrash)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	int Counter = 0;
	Area->RunOnEachConnectedNode(nullptr, [&](Node*) { Counter++; });
	EXPECT_EQ(Counter, 0);

	NODE_SYSTEM.Clear();
}

//                  A
//          /       |       \
//         B        C        D
//          \      /
//           C (shared)
TEST(NodeAreaEventSystemTests, RunOnEachConnectedNode_SharedNode_VisitsAllNodesExactlyOnce)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

//              A
//            /   \
//           B     C
//            \   /
//              D
//
TEST(NodeAreaEventSystemTests, RunOnEachConnectedNode_Diamond_VisitsEachOnce)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, Delete_Connection_OutSocketNodeReceivesDisconnectedEvent)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	EventCountingNode* OutNode = new EventCountingNode();
	OutNode->AddSocket(new NodeSocket(OutNode, "T", "out", NodeSocket::SocketFlow::Output));

	EventCountingNode* InNode = new EventCountingNode();
	InNode->AddSocket(new NodeSocket(InNode, "T", "in", NodeSocket::SocketFlow::Input));

	Area->AddNode(OutNode);
	Area->AddNode(InNode);
	ASSERT_TRUE(Area->TryToConnect(OutNode, 0, InNode, 0));
	ASSERT_EQ(Area->GetConnectionCount(), 1);

	// Both nodes should have received CONNECTED on the connect.
	EXPECT_EQ(InNode->GetConnectedCount(), 1);
	EXPECT_EQ(OutNode->GetConnectedCount(), 1);

	std::string OutSocketID = OutNode->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output);
	ASSERT_TRUE(Area->TryToDisconnect(OutNode, OutSocketID));

	// In socket's parent gets the event.
	EXPECT_EQ(InNode->GetDisconnectedCount(), 1);

	// Out socket's parent should also receive DISCONNECTED.
	EXPECT_EQ(OutNode->GetDisconnectedCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, Clear_EveryNodeReceivesDestroyed_EvenWhenCallbackDeletesOthers)
{
	NODE_SYSTEM.Clear();

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
	EXPECT_EQ(DestroyedIDs.size(), 3);
	EXPECT_NE(std::find(DestroyedIDs.begin(), DestroyedIDs.end(), NodeAID), DestroyedIDs.end());
	EXPECT_NE(std::find(DestroyedIDs.begin(), DestroyedIDs.end(), NodeBID), DestroyedIDs.end());
	EXPECT_NE(std::find(DestroyedIDs.begin(), DestroyedIDs.end(), NodeCID), DestroyedIDs.end());

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, TriggerOrphanSocketEvent_Reject_ForeignNode)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(AreaA, nullptr);
	ASSERT_NE(AreaB, nullptr);

	AreaB->SetSaveExecutedNodes(true);

	Node* NodeInAreaA = new Node();
	NodeInAreaA->AddSocket(new NodeSocket(NodeInAreaA, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(AreaA->AddNode(NodeInAreaA));

	EXPECT_FALSE(AreaB->TriggerOrphanSocketEvent(NodeInAreaA, EXECUTE));
	EXPECT_EQ(AreaB->GetLastExecutedNodes().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, Delete_CallbackDeletesAnotherNode_DoesNotCrash)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	Node* NodeA = new Node();
	Node* NodeB = new Node();
	ASSERT_TRUE(Area->AddNode(NodeA));
	ASSERT_TRUE(Area->AddNode(NodeB));

	std::string NodeAID = NodeA->GetID();
	std::string NodeBID = NodeB->GetID();

	// When NodeB is being destroyed, delete NodeA, which sits earlier in the node list.
	Area->AddNodeEventCallback([&](Node* Node, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::DESTROYED && Node->GetID() == NodeBID && Area->GetNodeByID(NodeAID) != nullptr)
			Area->Delete(Area->GetNodeByID(NodeAID));
	});

	EXPECT_TRUE(Area->Delete(NodeB));
	EXPECT_EQ(Area->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, TryToConnect_CallbackDeletesNode_AbortsSafely)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* OutputNode = new Node();
	OutputNode->AddSocket(new NodeSocket(OutputNode, "INT", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(OutputNode));

	Node* InputNode = new Node();
	InputNode->AddSocket(new NodeSocket(InputNode, "INT", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(LocalNodeArea->AddNode(InputNode));

	const std::string InputNodeID = InputNode->GetID();

	// User callback that deletes the input node during the connect handshake.
	LocalNodeArea->AddNodeEventCallback([LocalNodeArea, InputNodeID](Node* CurrentNode, NODE_EVENT EventType) {
		if (EventType == BEFORE_CONNECTED)
		{
			Node* NodeToDelete = LocalNodeArea->GetNodeByID(InputNodeID);
			if (NodeToDelete != nullptr)
				LocalNodeArea->Delete(NodeToDelete);
		}
	});

	// The connection is abandoned safely.
	EXPECT_FALSE(LocalNodeArea->TryToConnect(OutputNode, 0, InputNode, 0));
	EXPECT_EQ(LocalNodeArea->GetNodeByID(InputNodeID), nullptr);
	EXPECT_EQ(LocalNodeArea->GetConnectionCount(), 0);
	EXPECT_TRUE(OutputNode->GetNodesConnectedToOutput().empty());

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, TryToConnect_CallbackDeletesInNode_FromAfterConnected_IsSafe)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* OutputNode = new Node();
	OutputNode->AddSocket(new NodeSocket(OutputNode, "INT", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(OutputNode));

	Node* InputNode = new Node();
	InputNode->AddSocket(new NodeSocket(InputNode, "INT", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(LocalNodeArea->AddNode(InputNode));

	const std::string OutputNodeID = OutputNode->GetID();
	const std::string InputNodeID = InputNode->GetID();

	// A callback that inspects the node it is notified about and deletes the input node
	// during the AFTER_CONNECTED handshake. The connect path must not deliver a second
	// AFTER_CONNECTED for the just-deleted node, otherwise it reads the freed input
	// socket to resolve its parent and hands a dangling node to this callback.
	int AfterConnectedCount = 0;
	LocalNodeArea->AddNodeEventCallback([&](Node* CurrentNode, NODE_EVENT EventType) {
		if (EventType != NODE_EVENT::AFTER_CONNECTED)
			return;

		AfterConnectedCount++;
		CurrentNode->GetID();

		if (AfterConnectedCount == 1)
		{
			Node* NodeToDelete = LocalNodeArea->GetNodeByID(InputNodeID);
			if (NodeToDelete != nullptr)
				LocalNodeArea->Delete(NodeToDelete);
		}
	});

	LocalNodeArea->TryToConnect(OutputNode, 0, InputNode, 0);

	// The input node and its connection are gone, the output node survives, and the
	// callback was notified exactly once (never for the deleted node).
	EXPECT_EQ(LocalNodeArea->GetNodeByID(InputNodeID), nullptr);
	EXPECT_EQ(LocalNodeArea->GetConnectionCount(), 0);
	EXPECT_NE(LocalNodeArea->GetNodeByID(OutputNodeID), nullptr);
	EXPECT_EQ(AfterConnectedCount, 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, DeleteConnection_CallbackDeletesNode_DoesNotNotifyDeletedNode)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	BoolLiteralNode* Literal = TEST_TOOLS.CreateBoolLiteralNode(true);
	BoolVariableNode* Variable = TEST_TOOLS.CreateBoolVariableNode(false);
	ASSERT_NE(Literal, nullptr);
	ASSERT_NE(Variable, nullptr);

	LocalNodeArea->AddNode(Literal);
	LocalNodeArea->AddNode(Variable);
	ASSERT_TRUE(LocalNodeArea->TryToConnect(Literal, 0, Variable, 1));

	Node* LiteralPtr = Literal;
	Node* VariablePtr = Variable;
	bool bDeleteIssued = false;
	bool bVariableDeleted = false;
	bool bCallbackReceivedDeletedNode = false;
	bool bSurvivorWasNotified = false;

	LocalNodeArea->AddNodeEventCallback([&](Node* CallbackNode, NODE_EVENT EventType) {
		if (bVariableDeleted && CallbackNode == VariablePtr)
		{
			bCallbackReceivedDeletedNode = true;
			return;
		}

		if (CallbackNode == LiteralPtr && EventType == NODE_EVENT::BEFORE_DISCONNECTED)
		{
			bSurvivorWasNotified = true;

			if (!bDeleteIssued)
			{
				// React to the disconnect by deleting the other endpoint.
				bDeleteIssued = true;
				LocalNodeArea->Delete(VariablePtr);
				bVariableDeleted = true;
			}
		}
	});

	LocalNodeArea->TryToDisconnect(Literal, 0, Variable, 1);

	// The surviving endpoint was notified, the deleted one was skipped.
	EXPECT_TRUE(bSurvivorWasNotified);
	EXPECT_FALSE(bCallbackReceivedDeletedNode);
	EXPECT_EQ(LocalNodeArea->GetConnectionCount(), 0);
	EXPECT_EQ(LocalNodeArea->GetNodeCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaEventSystemTests, TryToConnect_CallbackConnectsSamePair_DoesNotDuplicateConnection)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* OutNode = new Node();
	OutNode->AddSocket(new NodeSocket(OutNode, "T", "out", NodeSocket::SocketFlow::Output));
	ASSERT_TRUE(LocalNodeArea->AddNode(OutNode));

	Node* InNode = new Node();
	InNode->AddSocket(new NodeSocket(InNode, "T", "in", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(LocalNodeArea->AddNode(InNode));

	// The callback reentrantly connects the same pair.
	bool bReentrantConnectIssued = false;
	bool bReentrantConnectResult = false;
	LocalNodeArea->AddNodeEventCallback([&](Node*, NODE_EVENT EventType) {
		if (EventType == NODE_EVENT::BEFORE_CONNECTED && !bReentrantConnectIssued)
		{
			bReentrantConnectIssued = true;
			bReentrantConnectResult = LocalNodeArea->TryToConnect(OutNode, 0, InNode, 0);
		}
	});

	// The reentrant call wins and creates the connection, the outer call detects the duplicate.
	EXPECT_FALSE(LocalNodeArea->TryToConnect(OutNode, 0, InNode, 0));
	EXPECT_TRUE(bReentrantConnectIssued);
	EXPECT_TRUE(bReentrantConnectResult);

	EXPECT_EQ(LocalNodeArea->GetConnectionCount(), 1);
	EXPECT_TRUE(LocalNodeArea->IsConnected(OutNode, 0, InNode, 0));

	// Exactly one entry per socket for the single logical edge.
	NodeSocket* OutSocket = OutNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	NodeSocket* InSocket = InNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	ASSERT_NE(OutSocket, nullptr);
	ASSERT_NE(InSocket, nullptr);
	EXPECT_EQ(OutSocket->GetConnectedSockets().size(), 1);
	EXPECT_EQ(InSocket->GetConnectedSockets().size(), 1);

	NODE_SYSTEM.Clear();
}