#include "TestTools.h"
using namespace VisNodeSys;

TestTools::TestTools() {}
TestTools::~TestTools() {}

bool TestTools::VerifyNodeAreaSaveLoadCycle_BasicChecks(VisNodeSys::NodeArea* NodeArea)
{
	if (NodeArea == nullptr)
		return false;

	std::string NodeAreaID = NodeArea->GetID();
	std::string NodeAreaName = NodeArea->GetName();
	int NodeCount = NodeArea->GetNodeCount();
	int ConnectionCount = NodeArea->GetConnectionCount();
	int RerouteConnectionCount = NodeArea->GetRerouteConnectionCount();
	int GroupCommentCount = NodeArea->GetGroupCommentCount();

	if (!NodeArea->SaveToFile("TemporaryNodeArea.json"))
		return false;

	NODE_SYSTEM.DeleteNodeArea(NodeArea);

	NodeArea = NODE_SYSTEM.CreateNodeArea();
	if (!NodeArea->LoadFromFile("TemporaryNodeArea.json"))
		return false;

	if (NodeArea->GetID() != NodeAreaID)
		return false;

	if (NodeArea->GetName() != NodeAreaName)
		return false;

	if (NodeArea->GetNodeCount() != NodeCount)
		return false;

	if (NodeArea->GetConnectionCount() != ConnectionCount)
		return false;

	if (NodeArea->GetRerouteConnectionCount() != RerouteConnectionCount)
		return false;

	if (NodeArea->GetGroupCommentCount() != GroupCommentCount)
		return false;

	return true;
}

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

bool TestTools::IsFirstIDsListSubsetOfSecond(const std::vector<std::string>& FirstList, const std::vector<std::string>& SecondList)
{
	for (size_t i = 0; i < FirstList.size(); i++)
	{
		bool bFound = false;
		for (size_t j = 0; j < SecondList.size(); j++)
		{
			if (FirstList[i] == SecondList[j])
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
			return false;
	}

	return true;
}

static bool UnorderedAreaMatch(const std::vector<NodeArea*>& Actual, const std::vector<NodeArea*>& Expected)
{
	if (Actual.size() != Expected.size())
		return false;

	// Every expected area must appear in actual, and vice versa.
	for (NodeArea* Area : Expected)
	{
		if (std::find(Actual.begin(), Actual.end(), Area) == Actual.end())
			return false;
	}

	for (NodeArea* Area : Actual)
	{
		if (std::find(Expected.begin(), Expected.end(), Area) == Expected.end())
			return false;
	}

	return true;
}

bool TestTools::VerifyImmediateDownstreamAreas(NodeArea* Area, const std::vector<NodeArea*>& Expected)
{
	std::vector<NodeArea*> Actual = NODE_SYSTEM.GetImmediateDownstreamAreas(Area->GetID());
	return UnorderedAreaMatch(Actual, Expected);
}

bool TestTools::VerifyImmediateUpstreamAreas(NodeArea* Area, const std::vector<NodeArea*>& Expected)
{
	std::vector<NodeArea*> Actual = NODE_SYSTEM.GetImmediateUpstreamAreas(Area->GetID());
	return UnorderedAreaMatch(Actual, Expected);
}

bool TestTools::VerifyAllDownstreamAreas(NodeArea* Area, const std::vector<NodeArea*>& Expected)
{
	std::vector<NodeArea*> Actual = NODE_SYSTEM.GetAllDownstreamAreas(Area->GetID());
	return UnorderedAreaMatch(Actual, Expected);
}

bool TestTools::VerifyAllUpstreamAreas(VisNodeSys::NodeArea* Area, const std::vector<VisNodeSys::NodeArea*>& Expected)
{
	std::vector<NodeArea*> Actual = NODE_SYSTEM.GetAllUpstreamAreas(Area->GetID());
	return UnorderedAreaMatch(Actual, Expected);
}

bool TestTools::VerifyNoLinkNodes(NodeArea* Area)
{
	return Area->GetNodesByType<LinkNode>().size() == 0;
}

// Verifies that no dangling LinkNodes remain in the given area.
bool TestTools::VerifyNoDanglingLinkNodes(VisNodeSys::NodeArea* Area)
{
	std::vector<LinkNode*> LinkNodes = Area->GetNodesByType<LinkNode>();
	for (LinkNode* LinkNode : LinkNodes)
	{
		if (LinkNode->GetPartnerNode() == nullptr || LinkNode->GetLinkedArea() == nullptr)
			return false;
	}

	return true;
}

std::vector<NodeArea*> TestTools::CreateSmallLinkedNodeAreaGraph()
{
	// Create 30 NodeAreas with this hierarchy:
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

	std::vector<NodeArea*> Areas;
	for (int i = 0; i < 30; i++)
	{
		NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
		Area->SetName(std::to_string(i));
		Areas.push_back(Area);
	}

	// Depth Level 0.
	NODE_SYSTEM.LinkNodeAreas(Areas[0]->GetID(), Areas[1]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[0]->GetID(), Areas[2]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[0]->GetID(), Areas[3]->GetID());

	// Depth Level 1.
	for (int i = 1; i <= 3; i++)
	{
		NODE_SYSTEM.LinkNodeAreas(Areas[i]->GetID(), Areas[i * 3 + 1]->GetID());
		NODE_SYSTEM.LinkNodeAreas(Areas[i]->GetID(), Areas[i * 3 + 2]->GetID());
		NODE_SYSTEM.LinkNodeAreas(Areas[i]->GetID(), Areas[i * 3 + 3]->GetID());
	}

	// Depth Level 2.
	NODE_SYSTEM.LinkNodeAreas(Areas[4]->GetID(), Areas[13]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[4]->GetID(), Areas[14]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[5]->GetID(), Areas[15]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[7]->GetID(), Areas[16]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[8]->GetID(), Areas[17]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[9]->GetID(), Areas[18]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[10]->GetID(), Areas[19]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[11]->GetID(), Areas[20]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[12]->GetID(), Areas[21]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[12]->GetID(), Areas[22]->GetID());

	// Depth Level 3.
	NODE_SYSTEM.LinkNodeAreas(Areas[13]->GetID(), Areas[23]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[15]->GetID(), Areas[24]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[16]->GetID(), Areas[25]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[18]->GetID(), Areas[26]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[20]->GetID(), Areas[27]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[21]->GetID(), Areas[28]->GetID());
	NODE_SYSTEM.LinkNodeAreas(Areas[22]->GetID(), Areas[29]->GetID());
	return Areas;
}

void TestTools::ConnectSmallLinkedNodeAreaGraph()
{
	std::vector<NodeArea*> Areas = GetOrderedAreasFromSmallLinkedNodeAreaGraph();
	for (size_t i = 0; i < Areas.size(); i++)
	{
		if (Areas[i] == nullptr)
			return;
	}

	if (Areas.size() != 30)
		return;

	NodeArea* CurrentArea = Areas[0];
	NodeArea* CurrentUpstreamArea = nullptr;
	NodeArea* CurrentDownstreamArea = nullptr;

	Node* CurrentExecutionSourceNode = nullptr;
	LinkNode* CurrentUpstreamLinkNode = nullptr;
	LinkNode* CurrentDownstreamLinkNode = nullptr;

	std::vector<std::pair<std::string, std::string>> UpstreamLinkingNodes = {};
	std::vector<std::pair<std::string, std::string>> DownstreamLinkingNodes = {};

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	ASSERT_NE(BeginNode, nullptr);
	Areas[0]->AddNode(BeginNode);
	Areas[0]->SetExecutionEntryNode(BeginNode);
	
	// 0 => 1.
	CurrentExecutionSourceNode = BeginNode;
	CurrentDownstreamArea = Areas[1];
	DownstreamLinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(CurrentArea->GetID(), CurrentDownstreamArea->GetID());
	CurrentDownstreamLinkNode = reinterpret_cast<LinkNode*>(CurrentArea->GetNodeByID(DownstreamLinkingNodes[0].first));
	ASSERT_EQ(CurrentArea->TryToConnect(CurrentExecutionSourceNode, 0, CurrentDownstreamLinkNode, 0), true);

	// 0 => 2.
	CurrentDownstreamArea = Areas[2];
	DownstreamLinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(CurrentArea->GetID(), CurrentDownstreamArea->GetID());
	CurrentDownstreamLinkNode = reinterpret_cast<LinkNode*>(CurrentArea->GetNodeByID(DownstreamLinkingNodes[0].first));
	ASSERT_EQ(CurrentArea->TryToConnect(CurrentExecutionSourceNode, 0, CurrentDownstreamLinkNode, 0), true);

	// 0 => 3.
	CurrentDownstreamArea = Areas[2];
	DownstreamLinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(CurrentArea->GetID(), CurrentDownstreamArea->GetID());
	CurrentDownstreamLinkNode = reinterpret_cast<LinkNode*>(CurrentArea->GetNodeByID(DownstreamLinkingNodes[0].first));
	ASSERT_EQ(CurrentArea->TryToConnect(CurrentExecutionSourceNode, 0, CurrentDownstreamLinkNode, 0), true);


	// Connects the incoming upstream link node to the link node leading to the given downstream area.
	auto ConnectFromUpstream = [&](NodeArea* CurrentArea, NodeArea* UpstreamArea, NodeArea* DownstreamArea) {
		auto UpstreamLinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(UpstreamArea->GetID(), CurrentArea->GetID());
		auto DownstreamLinkingNodes = NODE_SYSTEM.GetLinkingNodesForAreas(CurrentArea->GetID(), DownstreamArea->GetID());
		LinkNode* UpstreamLinkNode = reinterpret_cast<LinkNode*>(CurrentArea->GetNodeByID(UpstreamLinkingNodes[0].second));
		LinkNode* DownstreamLinkNode = reinterpret_cast<LinkNode*>(CurrentArea->GetNodeByID(DownstreamLinkingNodes[0].first));
		ASSERT_EQ(CurrentArea->TryToConnect(UpstreamLinkNode, 0, DownstreamLinkNode, 0), true);
	};

	// Depth Level 1.
	ConnectFromUpstream(Areas[1], Areas[0], Areas[4]);
	ConnectFromUpstream(Areas[1], Areas[0], Areas[5]);
	ConnectFromUpstream(Areas[1], Areas[0], Areas[6]);

	ConnectFromUpstream(Areas[2], Areas[0], Areas[7]);
	ConnectFromUpstream(Areas[2], Areas[0], Areas[8]);
	ConnectFromUpstream(Areas[2], Areas[0], Areas[9]);

	ConnectFromUpstream(Areas[3], Areas[0], Areas[10]);
	ConnectFromUpstream(Areas[3], Areas[0], Areas[11]);
	ConnectFromUpstream(Areas[3], Areas[0], Areas[12]);

	// Depth Level 2.
	ConnectFromUpstream(Areas[4], Areas[1], Areas[13]);
	ConnectFromUpstream(Areas[4], Areas[1], Areas[14]);
	ConnectFromUpstream(Areas[5], Areas[1], Areas[15]);

	ConnectFromUpstream(Areas[7], Areas[2], Areas[16]);
	ConnectFromUpstream(Areas[8], Areas[2], Areas[17]);
	ConnectFromUpstream(Areas[9], Areas[2], Areas[18]);

	ConnectFromUpstream(Areas[10], Areas[3], Areas[19]);
	ConnectFromUpstream(Areas[11], Areas[3], Areas[20]);
	ConnectFromUpstream(Areas[12], Areas[3], Areas[21]);
	ConnectFromUpstream(Areas[12], Areas[3], Areas[22]);

	// Depth Level 3.
	ConnectFromUpstream(Areas[13], Areas[4], Areas[23]);
	ConnectFromUpstream(Areas[15], Areas[5], Areas[24]);
	ConnectFromUpstream(Areas[16], Areas[7], Areas[25]);
	ConnectFromUpstream(Areas[18], Areas[9], Areas[26]);
	ConnectFromUpstream(Areas[20], Areas[11], Areas[27]);
	ConnectFromUpstream(Areas[21], Areas[12], Areas[28]);
	ConnectFromUpstream(Areas[22], Areas[12], Areas[29]);
}

std::vector<NodeArea*> TestTools::GetOrderedAreasFromSmallLinkedNodeAreaGraph()
{
	std::vector<NodeArea*> Result(30, nullptr);
	std::vector<std::string> AreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	if (AreaIDs.size() != 30)
		return Result;

	// We can not rely on order of areas in the list, so we need to find them by names (which we set to be the same as their index in the hierarchy).
	for (const std::string& AreaID : AreaIDs)
	{
		NodeArea* Area = NODE_SYSTEM.GetNodeAreaByID(AreaID);
		if (Area == nullptr)
			return Result;

		int Index = -1;
		try
		{
			Index = std::stoi(Area->GetName());
		}
		catch (...)
		{
			return Result;
		}

		if (Index < 0 || Index >= 30)
			return Result;

		Result[Index] = Area;
	}

	return Result;
}

bool TestTools::VerifyLinksInSmallNodeAreaGraph()
{
	std::vector<NodeArea*> Areas = GetOrderedAreasFromSmallLinkedNodeAreaGraph();
	for (size_t i = 0; i < Areas.size(); i++)
	{
		if (Areas[i] == nullptr)
			return false;
	}

	if (Areas.size() != 30)
		return false;

	// Depth Level 0.
	// Root has no upstream, three immediate downstream.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[0], {}))
		return false;
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[0], {}))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[0], {}))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[0], { Areas[1], Areas[2], Areas[3] }))
		return false;

	// All downstream from root should be everything except root itself.
	std::vector<NodeArea*> AllExceptRoot(Areas.begin() + 1, Areas.end());
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[0], AllExceptRoot))
		return false;

	// Depth Level 1.
	// Node 1.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[1], { Areas[0] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[1], { Areas[0] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[1], { Areas[4], Areas[5], Areas[6] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[1], { Areas[4], Areas[5], Areas[6], Areas[13], Areas[14], Areas[15], Areas[23], Areas[24] }))
		return false;
	// Node 2.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[2], { Areas[0] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[2], { Areas[0] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[2], { Areas[7], Areas[8], Areas[9] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[2], { Areas[7], Areas[8], Areas[9], Areas[16], Areas[17], Areas[18], Areas[25], Areas[26] }))
		return false;
	// Node 3.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[3], { Areas[0] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[3], { Areas[0] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[3], { Areas[10], Areas[11], Areas[12] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[3], { Areas[10], Areas[11], Areas[12], Areas[19], Areas[20], Areas[21], Areas[22], Areas[27], Areas[28], Areas[29] }))
		return false;

	// Depth Level 2.
	// Node 4.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[4], { Areas[1] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[4], { Areas[0], Areas[1] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[4], { Areas[13], Areas[14] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[4], { Areas[13], Areas[14], Areas[23] }))
		return false;
	// Node 5.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[5], { Areas[1] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[5], { Areas[0], Areas[1] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[5], { Areas[15] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[5], { Areas[15], Areas[24] }))
		return false;
	// Node 6.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[6], { Areas[1] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[6], { Areas[0], Areas[1] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[6], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[6], { }))
		return false;
	// Node 7.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[7], { Areas[2] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[7], { Areas[0], Areas[2] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[7], { Areas[16] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[7], { Areas[16], Areas[25] }))
		return false;
	// Node 8.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[8], { Areas[2] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[8], { Areas[0], Areas[2] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[8], { Areas[17] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[8], { Areas[17] }))
		return false;
	// Node 9.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[9], { Areas[2] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[9], { Areas[0], Areas[2] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[9], { Areas[18] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[9], { Areas[18], Areas[26] }))
		return false;
	// Node 10.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[10], { Areas[3] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[10], { Areas[0], Areas[3] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[10], { Areas[19] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[10], { Areas[19] }))
		return false;
	// Node 11.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[11], { Areas[3] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[11], { Areas[0], Areas[3] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[11], { Areas[20] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[11], { Areas[20], Areas[27] }))
		return false;
	// Node 12.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[12], { Areas[3] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[12], { Areas[0], Areas[3] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[12], { Areas[21], Areas[22] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[12], { Areas[21], Areas[22], Areas[28], Areas[29] }))
		return false;

	// Depth Level 3.
	// Node 13.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[13], { Areas[4] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[13], { Areas[0], Areas[1], Areas[4] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[13], { Areas[23] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[13], { Areas[23] }))
		return false;
	// Node 14.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[14], { Areas[4] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[14], { Areas[0], Areas[1], Areas[4] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[14], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[14], { }))
		return false;
	// Node 15.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[15], { Areas[5] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[15], { Areas[0], Areas[1], Areas[5] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[15], { Areas[24] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[15], { Areas[24] }))
		return false;
	// Node 16.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[16], { Areas[7] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[16], { Areas[0], Areas[2], Areas[7] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[16], { Areas[25] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[16], { Areas[25] }))
		return false;
	// Node 17.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[17], { Areas[8] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[17], { Areas[0], Areas[2], Areas[8] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[17], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[17], { }))
		return false;
	// Node 18.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[18], { Areas[9] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[18], { Areas[0], Areas[2], Areas[9] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[18], { Areas[26] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[18], { Areas[26] }))
		return false;
	// Node 19.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[19], { Areas[10] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[19], { Areas[0], Areas[3], Areas[10] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[19], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[19], { }))
		return false;
	// Node 20.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[20], { Areas[11] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[20], { Areas[0], Areas[3], Areas[11] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[20], { Areas[27] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[20], { Areas[27] }))
		return false;
	// Node 21.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[21], { Areas[12] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[21], { Areas[0], Areas[3], Areas[12] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[21], { Areas[28] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[21], { Areas[28] }))
		return false;
	// Node 22.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[22], { Areas[12] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[22], { Areas[0], Areas[3], Areas[12] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[22], { Areas[29] }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[22], { Areas[29] }))
		return false;

	// Depth Level 4.
	// Node 23.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[23], { Areas[13] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[23], { Areas[0], Areas[1], Areas[4], Areas[13] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[23], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[23], { }))
		return false;
	// Node 24.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[24], { Areas[15] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[24], { Areas[0], Areas[1], Areas[5], Areas[15] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[24], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[24], { }))
		return false;
	// Node 25.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[25], { Areas[16] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[25], { Areas[0], Areas[2], Areas[7], Areas[16] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[25], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[25], { }))
		return false;
	// Node 26.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[26], { Areas[18] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[26], { Areas[0], Areas[2], Areas[9], Areas[18] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[26], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[26], { }))
		return false;
	// Node 27.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[27], { Areas[20] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[27], { Areas[0], Areas[3], Areas[11], Areas[20] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[27], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[27], { }))
		return false;
	// Node 28.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[28], { Areas[21] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[28], { Areas[0], Areas[3], Areas[12], Areas[21] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[28], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[28], { }))
		return false;
	// Node 29.
	if (!TEST_TOOLS.VerifyImmediateUpstreamAreas(Areas[29], { Areas[22] }))
		return false;
	if (!TEST_TOOLS.VerifyAllUpstreamAreas(Areas[29], { Areas[0], Areas[3], Areas[12], Areas[22] }))
		return false;
	if (!TEST_TOOLS.VerifyImmediateDownstreamAreas(Areas[29], { }))
		return false;
	if (!TEST_TOOLS.VerifyAllDownstreamAreas(Areas[29], { }))
		return false;

	return true;
}

// ************************ Execution flow test tools ************************

NodeVariableSupportedType TestTools::GetRandomNodeVariableType()
{
	int RandomValue = rand() % 9;
	switch (RandomValue)
	{
		case 0:
			return NodeVariableSupportedType::BOOL;
		case 1:
			return NodeVariableSupportedType::INT;
		case 2:
			return NodeVariableSupportedType::FLOAT;
		case 3:
			return NodeVariableSupportedType::VEC2;
		case 4:
			return NodeVariableSupportedType::BVEC2;
		case 5:
			return NodeVariableSupportedType::VEC3;
		case 6:
			return NodeVariableSupportedType::BVEC3;
		case 7:
			return NodeVariableSupportedType::VEC4;
		case 8:
			return NodeVariableSupportedType::BVEC4;
	}

	return NodeVariableSupportedType::BOOL;
}

bool TestTools::GetRandomBoolValue()
{
	return rand() % 2 == 0;
}

BoolLiteralNode* TestTools::CreateBoolLiteralNode(bool bValue)
{
	BoolLiteralNode* NewNode = new BoolLiteralNode();
	NewNode->SetData(bValue);
	return NewNode;
}

BoolVariableNode* TestTools::CreateBoolVariableNode(bool bValue)
{
	BoolVariableNode* NewNode = new BoolVariableNode();
	NewNode->SetData(bValue);
	return NewNode;
}

int TestTools::GetRandomIntValue()
{
	return rand() % 10;
}

IntegerLiteralNode* TestTools::CreateIntegerLiteralNode(int Value)
{
	IntegerLiteralNode* NewNode = new IntegerLiteralNode();
	NewNode->SetData(Value);
	return NewNode;
}

IntegerVariableNode* TestTools::CreateIntegerVariableNode(int Value)
{
	IntegerVariableNode* NewNode = new IntegerVariableNode();
	NewNode->SetData(Value);
	return NewNode;
}

float TestTools::GetRandomFloatValue()
{
	return static_cast<float>(rand() % 1000) / 10.0f;
}

FloatLiteralNode* TestTools::CreateFloatLiteralNode(float Value)
{
	FloatLiteralNode* NewNode = new FloatLiteralNode();
	NewNode->SetData(Value);
	return NewNode;
}

FloatVariableNode* TestTools::CreateFloatVariableNode(float Value)
{
	FloatVariableNode* NewNode = new FloatVariableNode();
	NewNode->SetData(Value);
	return NewNode;
}

glm::vec2 TestTools::GetRandomVec2Value()
{
	glm::vec2 RandomValue;
	RandomValue.x = TEST_TOOLS.GetRandomFloatValue();
	RandomValue.y = TEST_TOOLS.GetRandomFloatValue();
	return RandomValue;
}

Vec2LiteralNode* TestTools::CreateVec2LiteralNode(glm::vec2 Value)
{
	Vec2LiteralNode* NewNode = new Vec2LiteralNode();
	NewNode->SetData(Value);
	return NewNode;
}

Vec2VariableNode* TestTools::CreateVec2VariableNode(glm::vec2 Value)
{
	Vec2VariableNode* NewNode = new Vec2VariableNode();
	NewNode->SetData(Value);
	return NewNode;
}

glm::bvec2 TestTools::GetRandomBVec2Value()
{
	glm::bvec2 RandomValue;
	RandomValue.x = TEST_TOOLS.GetRandomBoolValue();
	RandomValue.y = TEST_TOOLS.GetRandomBoolValue();
	return RandomValue;
}

BoolVec2LiteralNode* TestTools::CreateBVec2LiteralNode(glm::bvec2 Value)
{
	BoolVec2LiteralNode* NewNode = new BoolVec2LiteralNode();
	NewNode->SetData(Value);
	return NewNode;
}

BoolVec2VariableNode* TestTools::CreateBVec2VariableNode(glm::bvec2 Value)
{
	BoolVec2VariableNode* NewNode = new BoolVec2VariableNode();
	NewNode->SetData(Value);
	return NewNode;
}

LogicalNodeOperatorType TestTools::GetRandomLogicalOperatorType()
{
	int RandomValue = rand() % 4;
	switch (RandomValue)
	{
	case 0:
		return LogicalNodeOperatorType::AND;
	case 1:
		return LogicalNodeOperatorType::OR;
	case 2:
		return LogicalNodeOperatorType::XOR;
	case 3:
		return LogicalNodeOperatorType::NOT;
	}

	return LogicalNodeOperatorType::AND;
}

BaseLogicalOperatorNode* TestTools::CreateBaseLogicalOperatorNode(LogicalNodeOperatorType Type)
{
	BaseLogicalOperatorNode* NewNode = nullptr;

	switch (Type)
	{
	case LogicalNodeOperatorType::AND:
		NewNode = new LogicalANDOperatorNode();
		break;

	case LogicalNodeOperatorType::OR:
		NewNode = new LogicalOROperatorNode();
		break;

	case LogicalNodeOperatorType::XOR:
		NewNode = new LogicalXOROperatorNode();
		break;

	case LogicalNodeOperatorType::NOT:
		NewNode = new LogicalNOTOperatorNode();
		break;
	}

	return NewNode;
}

bool TestTools::GetResultFromLogicalOperator(LogicalNodeOperatorType Type, bool A, bool B)
{
	switch (Type)
	{
	case LogicalNodeOperatorType::AND:
		return A && B;
	case LogicalNodeOperatorType::OR:
		return A || B;
	case LogicalNodeOperatorType::XOR:
		return A != B;
	case LogicalNodeOperatorType::NOT:
		return !A;
	}

	return false;
}

ComparisonNodeOperatorType TestTools::GetRandomComparisonOperatorType()
{
	int RandomValue = rand() % 6;
	switch (RandomValue)
	{
	case 0:
		return ComparisonNodeOperatorType::EQUAL;
	case 1:
		return ComparisonNodeOperatorType::NOT_EQUAL;
	case 2:
		return ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL;
	case 3:
		return ComparisonNodeOperatorType::GREATER_THAN;
	case 4:
		return ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL;
	case 5:
		return ComparisonNodeOperatorType::LESS_THAN;
	}

	return ComparisonNodeOperatorType::EQUAL;
}

BaseComparisonOperatorNode* TestTools::CreateBaseComparisonOperatorNode(ComparisonNodeOperatorType Type)
{
	BaseComparisonOperatorNode* NewNode = nullptr;
	switch (Type)
	{
	case ComparisonNodeOperatorType::EQUAL:
		NewNode = new EqualNode();
		break;
	case ComparisonNodeOperatorType::NOT_EQUAL:
		NewNode = new NotEqualNode();
		break;
	case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
		NewNode = new GreaterThanOrEqualNode();
		break;
	case ComparisonNodeOperatorType::GREATER_THAN:
		NewNode = new GreaterThanNode();
		break;
	case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
		NewNode = new LessThanOrEqualNode();
		break;
	case ComparisonNodeOperatorType::LESS_THAN:
		NewNode = new LessThanNode();
		break;
	}
	return NewNode;
}

bool TestTools::GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, int A, int B)
{
	switch (Type)
	{
	case ComparisonNodeOperatorType::EQUAL:
		return A == B;
	case ComparisonNodeOperatorType::NOT_EQUAL:
		return A != B;
	case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
		return A >= B;
	case ComparisonNodeOperatorType::GREATER_THAN:
		return A > B;
	case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
		return A <= B;
	case ComparisonNodeOperatorType::LESS_THAN:
		return A < B;
	}

	return false;
}

bool TestTools::GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, float A, float B)
{
	switch (Type)
	{
	case ComparisonNodeOperatorType::EQUAL:
		return A == B;
	case ComparisonNodeOperatorType::NOT_EQUAL:
		return A != B;
	case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
		return A >= B;
	case ComparisonNodeOperatorType::GREATER_THAN:
		return A > B;
	case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
		return A <= B;
	case ComparisonNodeOperatorType::LESS_THAN:
		return A < B;
	}

	return false;
}

glm::bvec2 TestTools::GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, glm::vec2 A, glm::vec2 B)
{
	switch (Type)
	{
	case ComparisonNodeOperatorType::EQUAL:
		return glm::equal(A, B);
	case ComparisonNodeOperatorType::NOT_EQUAL:
		return glm::notEqual(A, B);
	case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
		return glm::greaterThanEqual(A, B);
	case ComparisonNodeOperatorType::GREATER_THAN:
		return glm::greaterThan(A, B);
	case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
		return glm::lessThanEqual(A, B);
	case ComparisonNodeOperatorType::LESS_THAN:
		return glm::lessThan(A, B);
	}

	return glm::bvec2(false);
}

ArithmeticOperationType TestTools::GetRandomArithmeticOperatorType()
{
	int RandomValue = rand() % 6;
	switch (RandomValue)
	{
	case 0:
		return ArithmeticOperationType::ADD;
	case 1:
		return ArithmeticOperationType::SUBTRACT;
	case 2:
		return ArithmeticOperationType::MULTIPLY;
	case 3:
		return ArithmeticOperationType::DIVIDE;
	case 4:
		return ArithmeticOperationType::MODULUS;
	case 5:
		return ArithmeticOperationType::POWER;

	}

	return ArithmeticOperationType::ADD;
}

BaseArithmeticOperatorNode* TestTools::CreateBaseArithmeticOperatorNode(ArithmeticOperationType Type)
{
	BaseArithmeticOperatorNode* NewNode = nullptr;
	switch (Type)
	{
	case ArithmeticOperationType::ADD:
		NewNode = new ArithmeticAddNode();
		break;
	case ArithmeticOperationType::SUBTRACT:
		NewNode = new ArithmeticSubtractNode();
		break;
	case ArithmeticOperationType::MULTIPLY:
		NewNode = new ArithmeticMultiplyNode();
		break;
	case ArithmeticOperationType::DIVIDE:
		NewNode = new ArithmeticDivideNode();
		break;
	case ArithmeticOperationType::MODULUS:
		NewNode = new ArithmeticModulusNode();
		break;
	case ArithmeticOperationType::POWER:
		NewNode = new ArithmeticPowerNode();
		break;
	}

	return NewNode;
}

int TestTools::GetResultFromArithmeticOperator(ArithmeticOperationType Type, int A, int B)
{
	switch (Type)
	{
	case ArithmeticOperationType::ADD:
		return A + B;
	case ArithmeticOperationType::SUBTRACT:
		return A - B;
	case ArithmeticOperationType::MULTIPLY:
		return A * B;
	case ArithmeticOperationType::DIVIDE:
		if (B == 0)
			return A;
		return A / B;
	case ArithmeticOperationType::MODULUS:
		if (B == 0)
			return A;
		return A % B;
	case ArithmeticOperationType::POWER:
		return static_cast<int>(std::pow(static_cast<double>(A), static_cast<double>(B)));
	}

	return 0;
}

float TestTools::GetResultFromArithmeticOperator(ArithmeticOperationType Type, float A, float B)
{
	switch (Type)
	{
	case ArithmeticOperationType::ADD:
		return A + B;
	case ArithmeticOperationType::SUBTRACT:
		return A - B;
	case ArithmeticOperationType::MULTIPLY:
		return A * B;
	case ArithmeticOperationType::DIVIDE:
		if (B == 0.0f)
			return A;
		return A / B;
	case ArithmeticOperationType::MODULUS:
		if (B == 0.0f)
			return A;
		return std::fmod(A, B);
	case ArithmeticOperationType::POWER:
		return static_cast<float>(std::pow(static_cast<double>(A), static_cast<double>(B)));
	}

	return 0.0f;
}

glm::vec2 TestTools::GetResultFromArithmeticOperator(ArithmeticOperationType Type, glm::vec2 A, glm::vec2 B)
{
	switch (Type)
	{
	case ArithmeticOperationType::ADD:
		return A + B;
	case ArithmeticOperationType::SUBTRACT:
		return A - B;
	case ArithmeticOperationType::MULTIPLY:
		return A * B;
	case ArithmeticOperationType::DIVIDE:
		if (B == glm::vec2(0.0f))
			return A;
		return A / B;
	case ArithmeticOperationType::MODULUS:
		if (B == glm::vec2(0.0f))
			return A;
		return glm::mod(A, B);
	case ArithmeticOperationType::POWER:
		return glm::pow(A, B);
	}

	return glm::vec2(0.0f);
}