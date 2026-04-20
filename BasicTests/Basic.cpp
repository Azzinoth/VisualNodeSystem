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

	LocalNodeArea->Delete(DefaultNode);

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

	FirstNode->AddSocket(new NodeSocket(FirstNode, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
	ASSERT_EQ(FirstNode->GetInputSocketCount(), 1);
	FirstNode->AddSocket(new NodeSocket(FirstNode, "EXECUTE", "out", NodeSocket::SocketFlow::Output));
	ASSERT_EQ(FirstNode->GetOutputSocketCount(), 1);

	LocalNodeArea->AddNode(FirstNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	Node* SecondNode = new Node();
	ASSERT_NE(SecondNode, nullptr);
	ASSERT_EQ(SecondNode->GetInputSocketCount(), 0);
	ASSERT_EQ(SecondNode->GetOutputSocketCount(), 0);

	SecondNode->AddSocket(new NodeSocket(SecondNode, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
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
	LocalNodeArea->Delete(FirstNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	ConnectedNodes = SecondNode->GetNodesConnectedToInput();
	ASSERT_EQ(ConnectedNodes.size(), 0);
	
	// Check incompatible sockets.
	Node* ThirdNode = new Node();
	ASSERT_NE(ThirdNode, nullptr);
	ASSERT_EQ(ThirdNode->GetInputSocketCount(), 0);
	ASSERT_EQ(ThirdNode->GetOutputSocketCount(), 0);

	ThirdNode->AddSocket(new NodeSocket(ThirdNode, "SOME_TYPE", "SOME_NAME", NodeSocket::SocketFlow::Output));
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

TEST(Basic, NodeSocketDeletion)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 0);

	Node* FirstNode = new Node();
	ASSERT_NE(FirstNode, nullptr);
	ASSERT_EQ(FirstNode->GetInputSocketCount(), 0);
	ASSERT_EQ(FirstNode->GetOutputSocketCount(), 0);

	FirstNode->AddSocket(new NodeSocket(FirstNode, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
	ASSERT_EQ(FirstNode->GetInputSocketCount(), 1);
	FirstNode->AddSocket(new NodeSocket(FirstNode, "EXECUTE", "out", NodeSocket::SocketFlow::Output));
	ASSERT_EQ(FirstNode->GetOutputSocketCount(), 1);

	LocalNodeArea->AddNode(FirstNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	Node* SecondNode = new Node();
	ASSERT_NE(SecondNode, nullptr);
	ASSERT_EQ(SecondNode->GetInputSocketCount(), 0);
	ASSERT_EQ(SecondNode->GetOutputSocketCount(), 0);

	SecondNode->AddSocket(new NodeSocket(SecondNode, "EXECUTE", "in", NodeSocket::SocketFlow::Input));
	ASSERT_EQ(SecondNode->GetInputSocketCount(), 1);
	ASSERT_EQ(SecondNode->GetOutputSocketCount(), 0);

	LocalNodeArea->AddNode(SecondNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 2);

	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstNode, 0, SecondNode, 0));
	
	// Delete socket, check if connections are deleted.
	FirstNode->DeleteSocket(FirstNode->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output));
	ASSERT_FALSE(LocalNodeArea->IsConnected(FirstNode, SecondNode));

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

	LocalNodeArea->Delete(NonDestractableNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 1);

	Node* OrdinaryNode = new Node();
	ASSERT_NE(OrdinaryNode, nullptr);

	LocalNodeArea->AddNode(OrdinaryNode);
	ASSERT_EQ(LocalNodeArea->GetNodeCount(), 2);

	LocalNodeArea->Delete(OrdinaryNode);
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

TEST(Basic, SetAllowedTypes_DisconnectsIncompatible)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* FirstNode = new Node();
	FirstNode->AddSocket(new NodeSocket(FirstNode, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	LocalNodeArea->AddNode(FirstNode);

	Node* SecondNode = new Node();
	SecondNode->AddSocket(new NodeSocket(SecondNode, "TYPE_A", "in", NodeSocket::SocketFlow::Input));
	LocalNodeArea->AddNode(SecondNode);

	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstNode, 0, SecondNode, 0));
	ASSERT_EQ(FirstNode->GetNodesConnectedToOutput().size(), 1);
	ASSERT_EQ(SecondNode->GetNodesConnectedToInput().size(), 1);

	// Change the input socket type to something incompatible.
	std::string SocketID = SecondNode->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	NodeSocket* Socket = SecondNode->GetSocketByID(SocketID);
	ASSERT_NE(Socket, nullptr);

	bool bNoDisconnections = Socket->SetAllowedTypes({ "TYPE_B" });
	ASSERT_FALSE(bNoDisconnections);

	// Connection should be gone.
	ASSERT_EQ(FirstNode->GetNodesConnectedToOutput().size(), 0);
	ASSERT_EQ(SecondNode->GetNodesConnectedToInput().size(), 0);
	ASSERT_FALSE(LocalNodeArea->IsConnected(FirstNode, SecondNode));

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, SetAllowedTypes_KeepsCompatible)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* FirstNode = new Node();
	FirstNode->AddSocket(new NodeSocket(FirstNode, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	LocalNodeArea->AddNode(FirstNode);

	Node* SecondNode = new Node();
	SecondNode->AddSocket(new NodeSocket(SecondNode, "TYPE_A", "in", NodeSocket::SocketFlow::Input));
	LocalNodeArea->AddNode(SecondNode);

	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstNode, 0, SecondNode, 0));

	// Change type to a set that still includes the original type.
	std::string SocketID = SecondNode->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	NodeSocket* Socket = SecondNode->GetSocketByID(SocketID);
	ASSERT_NE(Socket, nullptr);

	bool bNoDisconnections = Socket->SetAllowedTypes({ "TYPE_A", "TYPE_B" });
	ASSERT_TRUE(bNoDisconnections);

	// Connection should survive.
	ASSERT_EQ(FirstNode->GetNodesConnectedToOutput().size(), 1);
	ASSERT_EQ(SecondNode->GetNodesConnectedToInput().size(), 1);
	ASSERT_TRUE(LocalNodeArea->IsConnected(FirstNode, SecondNode));

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, SetAllowedTypes_PartialDisconnect)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* NodeA = new Node();
	NodeA->AddSocket(new NodeSocket(NodeA, "TYPE_A", "out", NodeSocket::SocketFlow::Output));
	LocalNodeArea->AddNode(NodeA);

	Node* NodeB = new Node();
	NodeB->AddSocket(new NodeSocket(NodeB, "TYPE_B", "out", NodeSocket::SocketFlow::Output));
	LocalNodeArea->AddNode(NodeB);

	Node* Receiver = new Node();
	Receiver->AddSocket(new NodeSocket(Receiver, std::vector<std::string>{"TYPE_A", "TYPE_B"}, "in", NodeSocket::SocketFlow::Input));
	LocalNodeArea->AddNode(Receiver);

	ASSERT_TRUE(LocalNodeArea->TryToConnect(NodeA, 0, Receiver, 0));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(NodeB, 0, Receiver, 0));
	ASSERT_EQ(Receiver->GetNodesConnectedToInput().size(), 2);

	// Narrow the input socket to only TYPE_A, should disconnect NodeB but keep NodeA.
	std::string SocketID = Receiver->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	NodeSocket* Socket = Receiver->GetSocketByID(SocketID);
	ASSERT_NE(Socket, nullptr);

	bool bNoDisconnections = Socket->SetAllowedTypes({ "TYPE_A" });
	ASSERT_FALSE(bNoDisconnections);

	ASSERT_TRUE(LocalNodeArea->IsConnected(NodeA, Receiver));
	ASSERT_FALSE(LocalNodeArea->IsConnected(NodeB, Receiver));
	ASSERT_EQ(Receiver->GetNodesConnectedToInput().size(), 1);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, DuplicateConnection_IsRejected)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	BoolLiteralNode* NodeA = TEST_TOOLS.CreateBoolLiteralNode(true);
	BoolVariableNode* NodeB = TEST_TOOLS.CreateBoolVariableNode(false);
	Area->AddNode(NodeA);
	Area->AddNode(NodeB);

	// First connection must succeed.
	EXPECT_TRUE(Area->TryToConnect(NodeA, 0, NodeB, 1));
	EXPECT_EQ(Area->GetConnectionCount(), 1);

	// Second identical connection must be rejected.
	EXPECT_FALSE(Area->TryToConnect(NodeA, 0, NodeB, 1));
	EXPECT_EQ(Area->GetConnectionCount(), 1);

	NODE_SYSTEM.DeleteNodeArea(Area);
}

TEST(Basic, CouldBeDestroyed_RegularNode_ReturnsTrue)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	Node* RegularNode = new Node();
	Area->AddNode(RegularNode);

	// A plain node with no protection should report that it can be destroyed.
	EXPECT_TRUE(RegularNode->CouldBeDestroyed());

	NODE_SYSTEM.DeleteNodeArea(Area);
}

TEST(Basic, Delete_NullptrNode_DoesNotCrash)
{
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	EXPECT_FALSE(Area->Delete(static_cast<Node*>(nullptr)));
	EXPECT_FALSE(Area->Delete(static_cast<GroupComment*>(nullptr)));
	EXPECT_FALSE(Area->Delete(static_cast<RerouteNode*>(nullptr)));

	NODE_SYSTEM.DeleteNodeArea(Area);
}

TEST(Basic, NodeCopyConstructor_GeneratesNewID)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* Original = new Node();
	LocalNodeArea->AddNode(Original);

	Node* Copy = new Node(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_NE(Copy->GetID(), Original->GetID());
	ASSERT_FALSE(Copy->GetID().empty());

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeCopyConstructor_CopiesGeometry)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* Original = new Node();
	Original->SetPosition(ImVec2(123.0f, 456.0f));
	Original->SetSize(ImVec2(321.0f, 654.0f));
	LocalNodeArea->AddNode(Original);

	Node* Copy = new Node(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_EQ(Copy->GetPosition(), ImVec2(123.0f, 456.0f));
	ASSERT_EQ(Copy->GetSize(), ImVec2(321.0f, 654.0f));

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeCopyConstructor_CopiesNameAndType)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	CustomNode2* Original = new CustomNode2();
	Original->SetName("MyCustomName");
	const std::string OriginalType = Original->GetType();
	LocalNodeArea->AddNode(Original);

	CustomNode2* Copy = new CustomNode2(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_EQ(Copy->GetName(), "MyCustomName");
	ASSERT_EQ(Copy->GetType(), OriginalType);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeCopyConstructor_CopiesStyle)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* Original = new Node();
	Original->SetStyle(CIRCLE);
	ASSERT_EQ(Original->GetStyle(), CIRCLE);
	LocalNodeArea->AddNode(Original);

	Node* Copy = new Node(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_EQ(Copy->GetStyle(), CIRCLE);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeCopyConstructor_PreservesCouldBeMovedFlag)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* Original = new Node();
	Original->SetCouldBeMoved(false);
	ASSERT_FALSE(Original->CouldBeMoved());
	LocalNodeArea->AddNode(Original);

	Node* Copy = new Node(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_FALSE(Copy->CouldBeMoved());

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeCopyConstructor_PreservesRenderTitleBarFlag)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* Original = new Node();
	Original->SetRenderTitleBar(false);
	ASSERT_FALSE(Original->GetRenderTitleBar());
	LocalNodeArea->AddNode(Original);

	Node* Copy = new Node(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_FALSE(Copy->GetRenderTitleBar());

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeCopyConstructor_PreservesCouldBeDestroyedFlag)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	BeginNode* Original = new BeginNode();
	ASSERT_FALSE(Original->CouldBeDestroyed());
	LocalNodeArea->AddNode(Original);

	BeginNode* Copy = new BeginNode(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_EQ(Copy->CouldBeDestroyed(), Original->CouldBeDestroyed());

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeCopyConstructor_DeepCopiesSockets)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* Original = new Node();
	NodeSocket* InSocket = new NodeSocket(Original, "FLOAT", "MyInput", NodeSocket::SocketFlow::Input);
	NodeSocket* OutSocket = new NodeSocket(Original, std::vector<std::string>{"FLOAT", "INT"}, "MyOutput", NodeSocket::SocketFlow::Output);
	Original->AddSocket(InSocket);
	Original->AddSocket(OutSocket);
	ASSERT_EQ(Original->GetInputSocketCount(), 1);
	ASSERT_EQ(Original->GetOutputSocketCount(), 1);
	LocalNodeArea->AddNode(Original);

	Node* Copy = new Node(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_EQ(Copy->GetInputSocketCount(), 1);
	ASSERT_EQ(Copy->GetOutputSocketCount(), 1);

	NodeSocket* CopiedIn = Copy->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	NodeSocket* CopiedOut = Copy->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	ASSERT_NE(CopiedIn, nullptr);
	ASSERT_NE(CopiedOut, nullptr);

	// Deep copy: different objects.
	ASSERT_NE(CopiedIn, InSocket);
	ASSERT_NE(CopiedOut, OutSocket);

	// Deep copy: different IDs (sockets get fresh IDs via NODE_CORE).
	ASSERT_NE(CopiedIn->GetID(), InSocket->GetID());
	ASSERT_NE(CopiedOut->GetID(), OutSocket->GetID());

	// Same payload.
	ASSERT_EQ(CopiedIn->GetName(), "MyInput");
	ASSERT_EQ(CopiedOut->GetName(), "MyOutput");
	ASSERT_EQ(CopiedIn->GetFlowDirection(), NodeSocket::SocketFlow::Input);
	ASSERT_EQ(CopiedOut->GetFlowDirection(), NodeSocket::SocketFlow::Output);
	ASSERT_EQ(CopiedIn->GetAllowedTypes(), (std::vector<std::string>{"FLOAT"}));
	ASSERT_EQ(CopiedOut->GetAllowedTypes(), (std::vector<std::string>{"FLOAT", "INT"}));

	// Parent rewired to the copy, not the source.
	ASSERT_EQ(CopiedIn->GetParent(), Copy);
	ASSERT_EQ(CopiedOut->GetParent(), Copy);

	// Source's sockets still point at the source.
	ASSERT_EQ(InSocket->GetParent(), Original);
	ASSERT_EQ(OutSocket->GetParent(), Original);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, NodeCopyConstructor_CopiesZeroSocketsCleanly)
{
	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Node* Original = new Node();
	LocalNodeArea->AddNode(Original);

	Node* Copy = new Node(*Original);
	LocalNodeArea->AddNode(Copy);

	ASSERT_EQ(Copy->GetInputSocketCount(), 0);
	ASSERT_EQ(Copy->GetOutputSocketCount(), 0);

	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, DeleteSocket_OnOrphanNode)
{
	Node* NewNode = new Node();
	NewNode->AddSocket(new NodeSocket(NewNode, "TEST", "test", NodeSocket::SocketFlow::Input));
	ASSERT_EQ(NewNode->GetInputSocketCount(), 1);

	std::string SocketID = NewNode->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	NewNode->DeleteSocket(SocketID);
	EXPECT_EQ(NewNode->GetInputSocketCount(), 0);

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	LocalNodeArea->AddNode(NewNode);
	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}

TEST(Basic, SetCaption_RejectsStringLongerThanMaxLength)
{
	// Build a string that is exactly one character over the limit.
	const std::string OversizedCaption(NODE_NAME_MAX_LENGTH + 1, 'X');

	Node* NewNode = new Node();
	NewNode->SetName(OversizedCaption);
	EXPECT_LE(NewNode->GetName().size(), static_cast<size_t>(NODE_NAME_MAX_LENGTH));

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	LocalNodeArea->AddNode(NewNode);
	NODE_SYSTEM.DeleteNodeArea(LocalNodeArea);
}