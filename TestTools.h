#pragma once
#include "../CustomNodes/CustomNode.h"
#include "../CustomNodes/CustomNode2.h"
#include "../CustomNodes/CustomNode3.h"
#include "../CustomNodes/CustomNode4.h"
#include "../CustomNodes/CustomNode5.h"
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

	// Checks if all node IDs in the first list are present in the second list. Order does not matter.
	bool IsFirstIDsListSubsetOfSecond(const std::vector<std::string>& FirstList, const std::vector<std::string>& SecondList);

	// Verifies that Area directly references exactly the areas in expected list (order-independent).
	bool VerifyImmediateDownstreamAreas(VisNodeSys::NodeArea* Area, const std::vector<VisNodeSys::NodeArea*>& Expected);
	// Verifies that recursive references from Area match Expected (order-independent).
	bool VerifyAllDownstreamAreas(VisNodeSys::NodeArea* Area, const std::vector<VisNodeSys::NodeArea*>& Expected);
	// Verifies that Area is referenced by exactly the areas in Expected (order-independent).
	bool VerifyImmediateUpstreamAreas(VisNodeSys::NodeArea* Area, const std::vector<VisNodeSys::NodeArea*>& Expected);
	// Verifies that recursive references to Area match Expected (order-independent).
	bool VerifyAllUpstreamAreas(VisNodeSys::NodeArea* Area, const std::vector<VisNodeSys::NodeArea*>& Expected);

	// Verifies that no VisualLinkNodes remain in the given area.
	bool VerifyNoLinkNodes(VisNodeSys::NodeArea* Area);

	// Verifies that no dangling VisualLinkNodes remain in the given area.
	bool VerifyNoDanglingLinkNodes(VisNodeSys::NodeArea* Area);

	// Creates 30 NodeAreas linked in the same 5-level tree hierarchy.
	std::vector<VisNodeSys::NodeArea*> CreateSmallLinkedNodeAreaGraph();
};

#define TEST_TOOLS TestTools::GetInstance()