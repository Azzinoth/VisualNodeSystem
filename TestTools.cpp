#include "TestTools.h"
using namespace VisNodeSys;

TestTools::TestTools() {}
TestTools::~TestTools() {}

void TestTools::AddOutputSocketsToNode(Node* NodeToAddSockets, std::string SocketType, int OutputCount)
{
	for (int i = 0; i < OutputCount; i++)
		NodeToAddSockets->AddSocket(new NodeSocket(NodeToAddSockets, SocketType, "out_" + std::to_string(i), true));
}

NodeArea* TestTools::CreateTinyPopulatedNodeArea(std::vector<std::string>& NodesIDList, std::vector<std::string>& GroupCommentsIDList)
{
	NodesIDList.clear();
	GroupCommentsIDList.clear();

	NodeArea* NodeArea = nullptr;
	NodeArea = NODE_SYSTEM.CreateNodeArea();
	NodeArea->SetIsFillingWindow(true);

	ImVec2 WindowSize = ImVec2(1280.0f, 720.0f);

	// Need to place that node in the center of the screen
	Node* DefaultNode = new Node();
	NodesIDList.push_back(DefaultNode->GetID());
	DefaultNode->SetName("Default node");
	DefaultNode->AddSocket(new NodeSocket(DefaultNode, "EXECUTE", "in", false));
	DefaultNode->AddSocket(new NodeSocket(DefaultNode, "EXECUTE", "out", true));
	DefaultNode->SetPosition(ImVec2(WindowSize.x / 2.0f - DefaultNode->GetSize().x / 2.0f, 250.0f));
	NodeArea->AddNode(DefaultNode);

	Node* RoundNode = new Node();
	NodesIDList.push_back(RoundNode->GetID());
	RoundNode->SetStyle(CIRCLE);
	RoundNode->SetName("Round node");
	RoundNode->AddSocket(new NodeSocket(RoundNode, "EXECUTE", "in", false));
	RoundNode->AddSocket(new NodeSocket(RoundNode, "EXECUTE", "out", true));
	RoundNode->SetPosition(ImVec2(WindowSize.x / 2.0f - RoundNode->GetSize().x / 2.0f, 400.0f));
	NodeArea->AddNode(RoundNode);

	// Custom node
	CustomNode* CustomNodeExample = new CustomNode();
	NodesIDList.push_back(CustomNodeExample->GetID());
	CustomNodeExample->SetName("CustomNodeExample");
	CustomNodeExample->SetPosition(ImVec2(WindowSize.x / 2.0f - CustomNodeExample->GetSize().x / 2, 540.0f));
	NodeArea->AddNode(CustomNodeExample);

	// Some simple rules on sockets
	CustomNode2* CustomNode2Example = new CustomNode2();
	NodesIDList.push_back(CustomNode2Example->GetID());
	CustomNode2Example->SetName("CustomNode2Example");
	CustomNode2Example->SetPosition(ImVec2(10.0f, 40.0f));
	CustomNode2Example->SetSize(CustomNode2Example->GetSize() + ImVec2(20.0f, 0.0f));
	NodeArea->AddNode(CustomNode2Example);

	CustomNode3* CustomNode3Example = new CustomNode3();
	NodesIDList.push_back(CustomNode3Example->GetID());
	CustomNode3Example->SetName("CustomNode3Example");
	CustomNode3Example->SetPosition(ImVec2(WindowSize.x / 2.0f - CustomNode3Example->GetSize().x / 2.0f, 40.0f));
	CustomNode3Example->SetSize(CustomNode3Example->GetSize() + ImVec2(80.0f, 0.0f));
	NodeArea->AddNode(CustomNode3Example);

	// Sockets events.
	CustomNode4* CustomNode4Example = new CustomNode4();
	NodesIDList.push_back(CustomNode4Example->GetID());
	CustomNode4Example->SetName("CustomNode4Example");
	CustomNode4Example->SetPosition(ImVec2(850.0f, 360.0f));
	NodeArea->AddNode(CustomNode4Example);

	// Socket style.
	CustomNodeStyleDemonstration* CustomNode5Example = new CustomNodeStyleDemonstration();
	NodesIDList.push_back(CustomNode5Example->GetID());
	CustomNode5Example->SetName("CustomNode5Example");
	CustomNode5Example->SetPosition(ImVec2(10.0f, 250.0f));
	CustomNode5Example->SetSize(CustomNode5Example->GetSize() + ImVec2(80.0f, 10.0f));
	NodeArea->AddNode(CustomNode5Example);

	CustomNodeStyleDemonstration* CustomNode6Example = new CustomNodeStyleDemonstration();
	NodesIDList.push_back(CustomNode6Example->GetID());
	CustomNode6Example->SetName("CustomNode6Example");
	CustomNode6Example->SetPosition(ImVec2(300.0f, 250.0f));
	NodeArea->AddNode(CustomNode6Example);

	NodeArea->TryToConnect(CustomNode5Example, 0, CustomNode6Example, 0);
	NodeArea->TryToConnect(CustomNode5Example, 1, CustomNode6Example, 1);
	NodeArea->TryToConnect(CustomNode5Example, 2, CustomNode6Example, 2);
	NodeArea->TryToConnect(CustomNode5Example, 3, CustomNode6Example, 3);

	// Demonstration of reroute nodes.
	Node* RerouteDemostrationNode = new Node();
	NodesIDList.push_back(RerouteDemostrationNode->GetID());
	RerouteDemostrationNode->SetName("Some node");
	RerouteDemostrationNode->AddSocket(new NodeSocket(RerouteDemostrationNode, "EXECUTE", "out", true));
	RerouteDemostrationNode->SetSize(RerouteDemostrationNode->GetSize() - ImVec2(80.0f, 0.0f));
	RerouteDemostrationNode->SetPosition(ImVec2(10.0f, 490.0f));
	NodeArea->AddNode(RerouteDemostrationNode);

	Node* RerouteDemostrationNodeMiddle = new Node();
	NodesIDList.push_back(RerouteDemostrationNodeMiddle->GetID());
	RerouteDemostrationNodeMiddle->SetName("Node in between");
	RerouteDemostrationNodeMiddle->SetSize(RerouteDemostrationNodeMiddle->GetSize() - ImVec2(70.0f, 0.0f));
	RerouteDemostrationNodeMiddle->SetPosition(ImVec2(180.0f, 490.0f));
	NodeArea->AddNode(RerouteDemostrationNodeMiddle);

	Node* RerouteDemostrationNodeEnd = new Node();
	NodesIDList.push_back(RerouteDemostrationNodeEnd->GetID());
	RerouteDemostrationNodeEnd->SetName("Some node");
	RerouteDemostrationNodeEnd->AddSocket(new NodeSocket(RerouteDemostrationNodeEnd, "EXECUTE", "in", false));
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
	GroupCommentsIDList.push_back(GroupCommentExample->GetID());
	GroupCommentExample->SetPosition(ImVec2(520.0f, 180.0f));
	GroupCommentExample->SetSize(ImVec2(710.0f, 500.0f));
	GroupCommentExample->SetCaption("Group of some nodes");
	NodeArea->AddGroupComment(GroupCommentExample);

	return NodeArea;
}

VisNodeSys::NodeArea* TestTools::CreateSmallConnectedNodeArea(std::vector<std::string>& NodesIDList)
{
	NodesIDList.clear();
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	// Create 30 nodes with this hierarchy:
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

	std::vector<Node*> Nodes;
	auto LocalAddNode = [&](Node* NewNode) {
		Area->AddNode(NewNode);
		NodesIDList.push_back(NewNode->GetID());
		Nodes.push_back(NewNode);
	};

	// Depth Level 0.
	// Node with index 0 is the root, it should have 3 output sockets and no input socket.
	Node* NewNode = new Node();
	NewNode->SetName("Node_0");
	AddOutputSocketsToNode(NewNode, "DUMMY", 3);
	LocalAddNode(NewNode);

	// Depth Level 1.
	for (int i = 1; i <= 3; i++)
	{
		NewNode = new Node();
		NewNode->SetName("Node_" + std::to_string(i));
		NewNode->AddSocket(new NodeSocket(NewNode, "DUMMY", "in_0", false));
		AddOutputSocketsToNode(NewNode, "DUMMY", 3);
		LocalAddNode(NewNode);
	}

	// Depth Level 2.
	for (int i = 4; i <= 12; i++)
	{
		NewNode = new Node();
		NewNode->SetName("Node_" + std::to_string(i));
		NewNode->AddSocket(new NodeSocket(NewNode, "DUMMY", "in_0", false));
		if (i == 4 || i == 12)
		{
			AddOutputSocketsToNode(NewNode, "DUMMY", 2);
		}
		else
		{
			AddOutputSocketsToNode(NewNode, "DUMMY", 3);
		}
			
		LocalAddNode(NewNode);
	}

	// Depth Level 3.
	for (int i = 13; i <= 22; i++)
	{
		NewNode = new Node();
		NewNode->SetName("Node_" + std::to_string(i));
		NewNode->AddSocket(new NodeSocket(NewNode, "DUMMY", "in_0", false));

		if (i == 13 || i == 15 || i == 16 || i == 18 || i >= 20)
			AddOutputSocketsToNode(NewNode, "DUMMY", 1);
		
		LocalAddNode(NewNode);
	}

	// Depth Level 4.
	for (int i = 23; i <= 29; i++)
	{
		NewNode = new Node();
		NewNode->SetName("Node_" + std::to_string(i));
		NewNode->AddSocket(new NodeSocket(NewNode, "DUMMY", "in_0", false));
		LocalAddNode(NewNode);
	}

	// Now we should connect the nodes according to the hierarchy described above.
	// Depth Level 0.
	Area->TryToConnect(Nodes[0], 0, Nodes[1], 0);
	Area->TryToConnect(Nodes[0], 1, Nodes[2], 0);
	Area->TryToConnect(Nodes[0], 2, Nodes[3], 0);

	// Depth Level 1.
	for (int i = 1; i <= 3; i++)
	{
		Area->TryToConnect(Nodes[i], 0, Nodes[i * 3 + 1], 0);
		Area->TryToConnect(Nodes[i], 1, Nodes[i * 3 + 2], 0);
		Area->TryToConnect(Nodes[i], 2, Nodes[i * 3 + 3], 0);
	}

	// Depth Level 2.
	Area->TryToConnect(Nodes[4], 0, Nodes[13], 0);
	Area->TryToConnect(Nodes[4], 1, Nodes[14], 0);
	Area->TryToConnect(Nodes[5], 0, Nodes[15], 0);
	Area->TryToConnect(Nodes[7], 0, Nodes[16], 0);
	Area->TryToConnect(Nodes[8], 0, Nodes[17], 0);
	Area->TryToConnect(Nodes[9], 0, Nodes[18], 0);
	Area->TryToConnect(Nodes[10], 0, Nodes[19], 0);
	Area->TryToConnect(Nodes[11], 0, Nodes[20], 0);
	Area->TryToConnect(Nodes[12], 0, Nodes[21], 0);
	Area->TryToConnect(Nodes[12], 1, Nodes[22], 0);

	// Depth Level 3.
	Area->TryToConnect(Nodes[13], 0, Nodes[23], 0);
	Area->TryToConnect(Nodes[15], 0, Nodes[24], 0);
	Area->TryToConnect(Nodes[16], 0, Nodes[25], 0);
	Area->TryToConnect(Nodes[18], 0, Nodes[26], 0);
	Area->TryToConnect(Nodes[20], 0, Nodes[27], 0);
	Area->TryToConnect(Nodes[21], 0, Nodes[28], 0);
	Area->TryToConnect(Nodes[22], 0, Nodes[29], 0);

	return Area;
}