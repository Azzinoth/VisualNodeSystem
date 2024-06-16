#include "Basic.h"
using namespace VisNodeSys;

std::vector<std::string> TemporaryNodesIDList;
std::vector<std::string> TemporaryGroupCommentsIDList;

NodeArea* CreateTinyScene()
{
	TemporaryNodesIDList.clear();
	TemporaryGroupCommentsIDList.clear();

	NodeArea* NodeArea = nullptr;
	NodeArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea->SetIsFillingWindow(true);

	ImVec2 WindowSize = ImVec2(1280.0f, 720.0f);

	// Need to place that node in the center of the screen
	Node* DefaultNode = new Node();
	TemporaryNodesIDList.push_back(DefaultNode->GetID());
	DefaultNode->SetName("Default node");
	DefaultNode->AddSocket(new NodeSocket(DefaultNode, "EXEC", "in", false));
	DefaultNode->AddSocket(new NodeSocket(DefaultNode, "EXEC", "out", true));
	DefaultNode->SetPosition(ImVec2(WindowSize.x / 2.0f - DefaultNode->GetSize().x / 2.0f, 250.0f));
	NodeArea->AddNode(DefaultNode);

	Node* RoundNode = new Node();
	TemporaryNodesIDList.push_back(RoundNode->GetID());
	RoundNode->SetStyle(CIRCLE);
	RoundNode->SetName("Round node");
	RoundNode->AddSocket(new NodeSocket(RoundNode, "EXEC", "in", false));
	RoundNode->AddSocket(new NodeSocket(RoundNode, "EXEC", "out", true));
	RoundNode->SetPosition(ImVec2(WindowSize.x / 2.0f - RoundNode->GetSize().x / 2.0f, 400.0f));
	NodeArea->AddNode(RoundNode);

	// Custom node
	CustomNode* CustomNodeExample = new CustomNode();
	TemporaryNodesIDList.push_back(CustomNodeExample->GetID());
	CustomNodeExample->SetName("CustomNodeExample");
	CustomNodeExample->SetPosition(ImVec2(WindowSize.x / 2.0f - CustomNodeExample->GetSize().x / 2, 540.0f));
	NodeArea->AddNode(CustomNodeExample);

	// Some simple rules on sockets
	CustomNode2* CustomNode2Example = new CustomNode2();
	TemporaryNodesIDList.push_back(CustomNode2Example->GetID());
	CustomNode2Example->SetName("CustomNode2Example");
	CustomNode2Example->SetPosition(ImVec2(10.0f, 40.0f));
	CustomNode2Example->SetSize(CustomNode2Example->GetSize() + ImVec2(20.0f, 0.0f));
	NodeArea->AddNode(CustomNode2Example);

	CustomNode3* CustomNode3Example = new CustomNode3();
	TemporaryNodesIDList.push_back(CustomNode3Example->GetID());
	CustomNode3Example->SetName("CustomNode3Example");
	CustomNode3Example->SetPosition(ImVec2(WindowSize.x / 2.0f - CustomNode3Example->GetSize().x / 2.0f, 40.0f));
	CustomNode3Example->SetSize(CustomNode3Example->GetSize() + ImVec2(80.0f, 0.0f));
	NodeArea->AddNode(CustomNode3Example);

	// Sockets events.
	CustomNode4* CustomNode4Example = new CustomNode4();
	TemporaryNodesIDList.push_back(CustomNode4Example->GetID());
	CustomNode4Example->SetName("CustomNode4Example");
	CustomNode4Example->SetPosition(ImVec2(850.0f, 360.0f));
	NodeArea->AddNode(CustomNode4Example);

	// Socket style.
	CustomNodeStyleDemonstration* CustomNode5Example = new CustomNodeStyleDemonstration();
	TemporaryNodesIDList.push_back(CustomNode5Example->GetID());
	CustomNode5Example->SetName("CustomNode5Example");
	CustomNode5Example->SetPosition(ImVec2(10.0f, 250.0f));
	CustomNode5Example->SetSize(CustomNode5Example->GetSize() + ImVec2(80.0f, 10.0f));
	NodeArea->AddNode(CustomNode5Example);

	CustomNodeStyleDemonstration* CustomNode6Example = new CustomNodeStyleDemonstration();
	TemporaryNodesIDList.push_back(CustomNode6Example->GetID());
	CustomNode6Example->SetName("CustomNode6Example");
	CustomNode6Example->SetPosition(ImVec2(300.0f, 250.0f));
	NodeArea->AddNode(CustomNode6Example);

	NodeArea->TryToConnect(CustomNode5Example, 0, CustomNode6Example, 0);
	NodeArea->TryToConnect(CustomNode5Example, 1, CustomNode6Example, 1);
	NodeArea->TryToConnect(CustomNode5Example, 2, CustomNode6Example, 2);
	NodeArea->TryToConnect(CustomNode5Example, 3, CustomNode6Example, 3);

	// Demonstration of reroute nodes.
	Node* RerouteDemostrationNode = new Node();
	TemporaryNodesIDList.push_back(RerouteDemostrationNode->GetID());
	RerouteDemostrationNode->SetName("Some node");
	RerouteDemostrationNode->AddSocket(new NodeSocket(RerouteDemostrationNode, "EXEC", "out", true));
	RerouteDemostrationNode->SetSize(RerouteDemostrationNode->GetSize() - ImVec2(80.0f, 0.0f));
	RerouteDemostrationNode->SetPosition(ImVec2(10.0f, 490.0f));
	NodeArea->AddNode(RerouteDemostrationNode);

	Node* RerouteDemostrationNodeMiddle = new Node();
	TemporaryNodesIDList.push_back(RerouteDemostrationNodeMiddle->GetID());
	RerouteDemostrationNodeMiddle->SetName("Node in between");
	RerouteDemostrationNodeMiddle->SetSize(RerouteDemostrationNodeMiddle->GetSize() - ImVec2(70.0f, 0.0f));
	RerouteDemostrationNodeMiddle->SetPosition(ImVec2(180.0f, 490.0f));
	NodeArea->AddNode(RerouteDemostrationNodeMiddle);

	Node* RerouteDemostrationNodeEnd = new Node();
	TemporaryNodesIDList.push_back(RerouteDemostrationNodeEnd->GetID());
	RerouteDemostrationNodeEnd->SetName("Some node");
	RerouteDemostrationNodeEnd->AddSocket(new NodeSocket(RerouteDemostrationNodeEnd, "EXEC", "in", false));
	RerouteDemostrationNodeEnd->SetSize(RerouteDemostrationNodeEnd->GetSize() - ImVec2(80.0f, 0.0f));
	RerouteDemostrationNodeEnd->SetPosition(ImVec2(350.0f, 490.0f));
	NodeArea->AddNode(RerouteDemostrationNodeEnd);

	NodeArea->TryToConnect(RerouteDemostrationNode, 0, RerouteDemostrationNodeEnd, 0);
	int SegmentIndex = 0;
	NodeArea->AddRerouteNodeToConnection(RerouteDemostrationNode, 0, RerouteDemostrationNodeEnd, 0, SegmentIndex, ImVec2(190.0f, 470.0f));
	NodeArea->AddRerouteNodeToConnection(RerouteDemostrationNode, 0, RerouteDemostrationNodeEnd, 0, SegmentIndex + 1, ImVec2(312.0f, 470.0f));

	// In previous example SegmentIndex would be 0 and 1.
	// But in real project you would need to know already existing reroute nodes(segments).
	// GetConnectionSegments function will give you vector of pair(begin and end coordinates) for each segment. 
	std::vector<std::pair<ImVec2, ImVec2>> Segments = NodeArea->GetConnectionSegments(RerouteDemostrationNode, 0, RerouteDemostrationNodeEnd, 0);

	// Add group comment to demonstrate how user can highlight node collections, signifying their combined functionality.
	GroupComment* GroupCommentExample = new GroupComment();
	TemporaryGroupCommentsIDList.push_back(GroupCommentExample->GetID());
	GroupCommentExample->SetPosition(ImVec2(520.0f, 180.0f));
	GroupCommentExample->SetSize(ImVec2(710.0f, 500.0f));
	GroupCommentExample->SetCaption("Group of some nodes");
	NodeArea->AddGroupComment(GroupCommentExample);

	return NodeArea;
}

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

	FirstNode->AddSocket(new NodeSocket(FirstNode, "EXEC", "in", false));
	ASSERT_EQ(FirstNode->GetInputSocketCount(), 1);
	FirstNode->AddSocket(new NodeSocket(FirstNode, "EXEC", "out", true));
	ASSERT_EQ(FirstNode->GetOutputSocketCount(), 1);

	LocalNodeArea->AddNode(FirstNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	Node* SecondNode = new Node();
	ASSERT_NE(SecondNode, nullptr);
	ASSERT_EQ(SecondNode->GetInputSocketCount(), 0);
	ASSERT_EQ(SecondNode->GetOutputSocketCount(), 0);

	SecondNode->AddSocket(new NodeSocket(SecondNode, "EXEC", "in", false));
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
	NodeArea* LocalNodeArea = CreateTinyScene();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);
	ASSERT_EQ(TemporaryNodesIDList.size(), 11);
	ASSERT_EQ(TemporaryGroupCommentsIDList.size(), 1);

	std::string FilePath = "NodeAreaTest.json";
	LocalNodeArea->SaveToFile(FilePath.c_str());

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);

	ImVec2 WindowSize = ImVec2(1280.0f, 720.0f);
	LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	LocalNodeArea->LoadFromFile(FilePath.c_str());

	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);
	ASSERT_EQ(LocalNodeArea->GetGroupCommentCount(), 1);

	std::vector<Node*> Nodes = LocalNodeArea->GetNodesByName("Default node");
	ASSERT_EQ(Nodes.size(), 1);
	Node* NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), TemporaryNodesIDList[0]);
	ASSERT_EQ(NodeToCheck->GetStyle(), DEFAULT);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(WindowSize.x / 2.0f - NodeToCheck->GetSize().x / 2.0f, 250.0f));

	Nodes = LocalNodeArea->GetNodesByName("Round node");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), TemporaryNodesIDList[1]);
	ASSERT_EQ(NodeToCheck->GetStyle(), CIRCLE);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(WindowSize.x / 2.0f - NodeToCheck->GetSize().x / 2.0f, 400.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNodeExample");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), TemporaryNodesIDList[2]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(WindowSize.x / 2.0f - NodeToCheck->GetSize().x / 2, 540.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode2Example");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), TemporaryNodesIDList[3]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(10.0f, 40.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode3Example");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), TemporaryNodesIDList[4]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(WindowSize.x / 2.0f - (NodeToCheck->GetSize().x - 80.0f) / 2.0f, 40.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode4Example");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), TemporaryNodesIDList[5]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(850.0f, 360.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode5Example");
	ASSERT_EQ(Nodes.size(), 1);
	NodeToCheck = Nodes[0];
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetID(), TemporaryNodesIDList[6]);
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 4);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 4);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(10.0f, 250.0f));

	Nodes = LocalNodeArea->GetNodesByName("CustomNode6Example");
	ASSERT_EQ(Nodes.size(), 1);
	Node* SecondNodeToCheck = Nodes[0];
	ASSERT_NE(SecondNodeToCheck, nullptr);
	ASSERT_EQ(SecondNodeToCheck->GetID(), TemporaryNodesIDList[7]);
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

	NodeToCheck = LocalNodeArea->GetNodeByID(TemporaryNodesIDList[8]);
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetName(), "Some node");
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(10.0f, 490.0f));

	NodeToCheck = LocalNodeArea->GetNodeByID(TemporaryNodesIDList[9]);
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetName(), "Node in between");
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(180.0f, 490.0f));

	NodeToCheck = LocalNodeArea->GetNodeByID(TemporaryNodesIDList[10]);
	ASSERT_NE(NodeToCheck, nullptr);
	ASSERT_EQ(NodeToCheck->GetName(), "Some node");
	ASSERT_EQ(NodeToCheck->GetInputSocketCount(), 1);
	ASSERT_EQ(NodeToCheck->GetOutputSocketCount(), 0);
	ASSERT_EQ(NodeToCheck->GetPosition(), ImVec2(350.0f, 490.0f));

	GroupComment* GroupCommentToCheck = LocalNodeArea->GetGroupCommentByID(TemporaryGroupCommentsIDList[0]);
	ASSERT_NE(GroupCommentToCheck, nullptr);
	ASSERT_EQ(GroupCommentToCheck->GetCaption(), "Group of some nodes");
	ASSERT_EQ(GroupCommentToCheck->GetPosition(), ImVec2(520.0f, 180.0f));
	ASSERT_EQ(GroupCommentToCheck->GetSize(), ImVec2(710.0f, 500.0f));

	Nodes = LocalNodeArea->GetNodesInGroupComment(GroupCommentToCheck);
	ASSERT_EQ(Nodes.size(), 4);
	ASSERT_EQ(Nodes[0]->GetID(), TemporaryNodesIDList[0]);
	ASSERT_EQ(Nodes[1]->GetID(), TemporaryNodesIDList[1]);
	ASSERT_EQ(Nodes[2]->GetID(), TemporaryNodesIDList[2]);
	ASSERT_EQ(Nodes[3]->GetID(), TemporaryNodesIDList[5]);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeAreaWide_Node_Events)
{
	NodeArea* LocalNodeArea = CreateTinyScene();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 11);
	ASSERT_EQ(TemporaryNodesIDList.size(), 11);
	ASSERT_EQ(TemporaryGroupCommentsIDList.size(), 1);

	bool bFirstRemoveCall = true;
	Node* NodeToDelete = LocalNodeArea->GetNodeByID(TemporaryNodesIDList[8]);
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
	ASSERT_EQ(LocalNodeArea->GetNodeByID(TemporaryNodesIDList[8]), nullptr);
	ASSERT_TRUE(bNodeRemovedEventCalled);

	
	Node* NodeToConnectFrom = LocalNodeArea->GetNodeByID(TemporaryNodesIDList[1]);
	ASSERT_NE(NodeToConnectFrom, nullptr);
	Node* NodeToConnectTo = LocalNodeArea->GetNodeByID(TemporaryNodesIDList[5]);
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
				ASSERT_EQ(Node->GetID(), TemporaryNodesIDList[0]);
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