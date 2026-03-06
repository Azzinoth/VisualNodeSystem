#include "Basic.h"
using namespace VisNodeSys;

TEST(Basic, NodeTesting)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	Node* DefaultNode = new Node();
	ASSERT_NE(DefaultNode, nullptr);
	DefaultNode->SetName("New default node");
	ASSERT_EQ(DefaultNode->GetName(), "New default node");

	LocalNodeArea->AddNode(DefaultNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	ASSERT_EQ(DefaultNode->GetParentArea(), LocalNodeArea);

	// Geting node by name.
	auto ListOfNodes = LocalNodeArea->GetNodesByName("New default node");
	ASSERT_EQ(ListOfNodes.size(), 1);
	Node* ReturnedNode = ListOfNodes[0];
	ASSERT_EQ(ReturnedNode, DefaultNode);

	LocalNodeArea->DeleteNode(DefaultNode);

	ListOfNodes = LocalNodeArea->GetNodesByName("New default node");
	ASSERT_EQ(ListOfNodes.size(), 0);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeArea_TryToConnect_CheckProperFailures)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	Node* SomeNode = new Node();
	ASSERT_NE(SomeNode, nullptr);

	ASSERT_FALSE(LocalNodeArea->TryToConnect(nullptr, 0, nullptr, 0));
	ASSERT_FALSE(LocalNodeArea->TryToConnect(nullptr, 0, SomeNode, 0));
	ASSERT_FALSE(LocalNodeArea->TryToConnect(SomeNode, 0, nullptr, 0));

	ASSERT_FALSE(LocalNodeArea->TryToConnect(SomeNode, 0, SomeNode, 0));
	ASSERT_FALSE(LocalNodeArea->TryToConnect(SomeNode, -1, SomeNode, -1));

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeSockets)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	Node* FirstNode = new Node();
	ASSERT_NE(FirstNode, nullptr);
	ASSERT_EQ(FirstNode->GetInputSocketCount(), 0);
	ASSERT_EQ(FirstNode->GetOutputSocketCount(), 0);

	FirstNode->AddSocket(new NodeSocket(FirstNode, "EXECUTE", "in", false));
	ASSERT_EQ(FirstNode->GetInputSocketCount(), 1);
	FirstNode->AddSocket(new NodeSocket(FirstNode, "EXECUTE", "out", true));
	ASSERT_EQ(FirstNode->GetOutputSocketCount(), 1);

	LocalNodeArea->AddNode(FirstNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	Node* SecondNode = new Node();
	ASSERT_NE(SecondNode, nullptr);
	ASSERT_EQ(SecondNode->GetInputSocketCount(), 0);
	ASSERT_EQ(SecondNode->GetOutputSocketCount(), 0);

	SecondNode->AddSocket(new NodeSocket(SecondNode, "EXECUTE", "in", false));
	ASSERT_EQ(SecondNode->GetInputSocketCount(), 1);
	ASSERT_EQ(SecondNode->GetOutputSocketCount(), 0);

	LocalNodeArea->AddNode(SecondNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 2);

	// Connections.
	std::vector<Node*> ConnectedNodes = FirstNode->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 0);
	ConnectedNodes = FirstNode->GetNodesConnectedToOutput();
	ASSERT_EQ(ConnectedNodes.size(), 0);

	ConnectedNodes = SecondNode->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 0);
	ConnectedNodes = SecondNode->GetNodesConnectedToOutput();
	ASSERT_EQ(ConnectedNodes.size(), 0);

	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstNode, 0, SecondNode, 0));
	ASSERT_FALSE(LocalNodeArea->TryToConnect(FirstNode, 2, SecondNode, 5));
	
	ConnectedNodes = FirstNode->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 0);
	ConnectedNodes = FirstNode->GetNodesConnectedToOutput();
	ASSERT_EQ(ConnectedNodes.size(), 1);

	ConnectedNodes = SecondNode->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 1);
	ConnectedNodes = SecondNode->GetNodesConnectedToOutput();
	ASSERT_EQ(ConnectedNodes.size(), 0);

	// Delete one node, check if connections are deleted.
	LocalNodeArea->DeleteNode(FirstNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	ConnectedNodes = SecondNode->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 0);
	
	// Check incompatible sockets.
	Node* ThirdNode = new Node();
	ASSERT_NE(ThirdNode, nullptr);
	ASSERT_EQ(ThirdNode->GetInputSocketCount(), 0);
	ASSERT_EQ(ThirdNode->GetOutputSocketCount(), 0);

	ThirdNode->AddSocket(new NodeSocket(ThirdNode, "SOME_TYPE", "SOME_NAME", true));
	ASSERT_EQ(ThirdNode->GetInputSocketCount(), 0);
	ASSERT_EQ(ThirdNode->GetOutputSocketCount(), 1);

	LocalNodeArea->AddNode(ThirdNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 2);

	ASSERT_FALSE(LocalNodeArea->TryToConnect(ThirdNode, 0, SecondNode, 0));

	ConnectedNodes = SecondNode->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 0);
	ConnectedNodes = SecondNode->GetNodesConnectedToOutput();
	ASSERT_EQ(ConnectedNodes.size(), 0);

	ConnectedNodes = ThirdNode->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 0);
	ConnectedNodes = ThirdNode->GetNodesConnectedToOutput();
	ASSERT_EQ(ConnectedNodes.size(), 0);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NonDestractableNode)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	CustomNode* NonDestractableNode = new CustomNode();
	ASSERT_NE(NonDestractableNode, nullptr);
	NonDestractableNode->SetName("Non destractable node");

	LocalNodeArea->AddNode(NonDestractableNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	LocalNodeArea->DeleteNode(NonDestractableNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	Node* OrdinaryNode = new Node();
	ASSERT_NE(OrdinaryNode, nullptr);

	LocalNodeArea->AddNode(OrdinaryNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 2);

	LocalNodeArea->DeleteNode(OrdinaryNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, Save_And_Load_NodeArea)
{
	std::vector<std::string> NodesIDList;
	std::vector<std::string> GroupCommentsIDList;

	NodeArea* LocalNodeArea = TEST_TOOLS.CreateTinyPopulatedNodeArea(NodesIDList, GroupCommentsIDList);
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);
	ASSERT_EQ(NodesIDList.size(), 11);
	ASSERT_EQ(LocalNodeArea->GetGroupCommentCount(), 1);
	ASSERT_EQ(GroupCommentsIDList.size(), 1);
	ASSERT_EQ(LocalNodeArea->GetConnectionCount(), 5);
	ASSERT_EQ(LocalNodeArea->GetRerouteConnectionCount(), 2);

	std::string FilePath = "NodeAreaTest.json";
	LocalNodeArea->SaveToFile(FilePath.c_str());

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);

	ImVec2 WindowSize = ImVec2(1280.0f, 720.0f);
	LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	LocalNodeArea->LoadFromFile(FilePath.c_str());

	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);
	ASSERT_EQ(LocalNodeArea->GetGroupCommentCount(), 1);
	ASSERT_EQ(LocalNodeArea->GetConnectionCount(), 5);
	ASSERT_EQ(LocalNodeArea->GetRerouteConnectionCount(), 2);

	std::vector<Node*> Nodes = LocalNodeArea->GetNodesByName("Default node");
	ASSERT_EQ(Nodes.size(), 1);
	Node* NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), NodesIDList[0]);
	ASSERT_EQ(NodeToCheck->GetStyle(), DEFAULT);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(WindowSize.x / 2.0f - NodeToCheck->GetSize().x / 2.0f, 250.0f));

	Nodes = LocalNodeArea->GetNodesByName("Round node");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), NodesIDList[1]);
	ASSERT_EQ(NodeToCheck->GetStyle(), CIRCLE);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(WindowSize.x / 2.0f - NodeToCheck->GetSize().x / 2.0f, 400.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNodeExample");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), NodesIDList[2]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(WindowSize.x / 2.0f - NodeToCheck->GetSize().x / 2, 540.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode2Example");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), NodesIDList[3]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(10.0f, 40.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode3Example");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), NodesIDList[4]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(WindowSize.x / 2.0f - (NodeToCheck->GetSize().x - 80.0f) / 2.0f, 40.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode4Example");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), NodesIDList[5]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(850.0f, 360.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode5Example");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), NodesIDList[6]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 4);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 4);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(10.0f, 250.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode6Example");
	ASSERT_EQ(Nodes.size(), 1);
	Node* SecondNodeToCheck = Nodes[0];
	ASSERT_NE(SecondNodeToCheck, nullptr);
	ASSERT_EQ(SecondNodeToCheck->GetID(), NodesIDList[7]);
	ASSERT_EQ(SecondNodeToCheck->GetInputSocketCount(), 4);
	ASSERT_EQ(SecondNodeToCheck->GetOutputSocketCount(), 4);
	ASSERT_EQ(SecondNodeToCheck->GetPosition(), ImVec2(300.0f, 250.0f));

	std::vector<Node*> ConnectedNodes = NodeToCheck->GetNodesConnectedToOutput();
	ASSERT_EQ(ConnectedNodes.size(), 1);
	ASSERT_EQ(ConnectedNodes[0], SecondNodeToCheck);

	ConnectedNodes = NodeToCheck->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 0);

	ConnectedNodes = SecondNodeToCheck->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 1);
	ASSERT_EQ(ConnectedNodes[0], NodeToCheck);

	ConnectedNodes = SecondNodeToCheck->GetNodesConnectedToOutput();
	ASSERT_EQ(ConnectedNodes.size(), 0);

	Node* RerouteDemostrationNode = LocalNodeArea->GetNodeByID(NodesIDList[8]);
	ASSERT_NE(RerouteDemostrationNode, nullptr);
	ASSERT_EQ(RerouteDemostrationNode->GetName(), "Some node");
	ASSERT_EQ(RerouteDemostrationNode->GetInputSocketCount(), 0);
	ASSERT_EQ(RerouteDemostrationNode->GetOutputSocketCount(), 1);
	ASSERT_EQ(RerouteDemostrationNode->GetPosition(), ImVec2(10.0f, 490.0f));

	Node* RerouteDemostrationNodeMiddle = LocalNodeArea->GetNodeByID(NodesIDList[9]);
	ASSERT_NE(RerouteDemostrationNodeMiddle, nullptr);
	ASSERT_EQ(RerouteDemostrationNodeMiddle->GetName(), "Node in between");
	ASSERT_EQ(RerouteDemostrationNodeMiddle->GetInputSocketCount(), 0);
	ASSERT_EQ(RerouteDemostrationNodeMiddle->GetOutputSocketCount(), 0);
	ASSERT_EQ(RerouteDemostrationNodeMiddle->GetPosition(), ImVec2(180.0f, 490.0f));

	Node* RerouteDemostrationNodeEnd = LocalNodeArea->GetNodeByID(NodesIDList[10]);
	ASSERT_NE(RerouteDemostrationNodeEnd, nullptr);
	ASSERT_EQ(RerouteDemostrationNodeEnd->GetName(), "Some node");
	ASSERT_EQ(RerouteDemostrationNodeEnd->GetInputSocketCount(), 1);
	ASSERT_EQ(RerouteDemostrationNodeEnd->GetOutputSocketCount(), 0);
	ASSERT_EQ(RerouteDemostrationNodeEnd->GetPosition(), ImVec2(350.0f, 490.0f));

	LocalNodeArea->TryToConnect(RerouteDemostrationNode, 0, RerouteDemostrationNodeEnd, 0);
	int SegmentIndex = 0;
	ASSERT_EQ(LocalNodeArea->AddRerouteNodeToConnection(RerouteDemostrationNode, 0, RerouteDemostrationNodeEnd, 0, SegmentIndex, ImVec2(190.0f, 470.0f)), true);
	ASSERT_EQ(LocalNodeArea->AddRerouteNodeToConnection(RerouteDemostrationNode, 0, RerouteDemostrationNodeEnd, 0, SegmentIndex + 1, ImVec2(312.0f, 470.0f)), true);

	GroupComment* GroupCommentToCheck = LocalNodeArea->GetGroupCommentByID(GroupCommentsIDList[0]);
	ASSERT_NE(GroupCommentToCheck, nullptr);
	ASSERT_EQ(GroupCommentToCheck->GetCaption(), "Group of some nodes");
	ASSERT_EQ(GroupCommentToCheck->GetPosition(), ImVec2(520.0f, 180.0f));
	ASSERT_EQ(GroupCommentToCheck->GetSize(), ImVec2(710.0f, 500.0f));

	Nodes = LocalNodeArea->GetNodesInGroupComment(GroupCommentToCheck);
	ASSERT_EQ(Nodes.size(), 4);
	ASSERT_EQ(Nodes[0]->GetID(), NodesIDList[0]);
	ASSERT_EQ(Nodes[1]->GetID(), NodesIDList[1]);
	ASSERT_EQ(Nodes[2]->GetID(), NodesIDList[2]);
	ASSERT_EQ(Nodes[3]->GetID(), NodesIDList[5]);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeAreaWide_Node_Events)
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
		if (EventType == NODE_EVENT::REMOVED)
		{
			if (bFirstRemoveCall)
			{
				bNodeRemovedEventCalled = true;
				ASSERT_EQ(Node->GetID(), NodeToDelete->GetID());
				bFirstRemoveCall = false;
			}
		}
	});
	LocalNodeArea->DeleteNode(NodeToDelete);
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

TEST(Basic, Node_CanConnect_Functionality)
{
	// To-Do
}

TEST(Basic, Node_SocketEvent_Functionality)
{
	// To-Do
}