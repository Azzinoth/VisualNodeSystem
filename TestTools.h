#pragma once
#include "../CustomNodes/CustomNode.h"
#include "../CustomNodes/CustomNode2.h"
#include "../CustomNodes/CustomNode3.h"
#include "../CustomNodes/CustomNode4.h"
#include "../CustomNodes/CustomNodeStyleDemonstration.h"

class TestTools
{
	SINGLETON_PRIVATE_PART(TestTools)


public:
	SINGLETON_PUBLIC_PART(TestTools)

	VisNodeSys::NodeArea* CreateTinyPopulatedNodeArea(std::vector<std::string>& NodesIDList, std::vector<std::string>& GroupCommentsIDList);
	// Creates a NodeArea with 30 nodes connected in a 5-level tree hierarchy.
	VisNodeSys::NodeArea* CreateSmallConnectedNodeArea(std::vector<std::string>& NodesIDList);

	void AddOutputSocketsToNode(VisNodeSys::Node* NodeToAddSockets, std::string SocketType, int OutputCount);
};

#define TEST_TOOLS TestTools::GetInstance()
