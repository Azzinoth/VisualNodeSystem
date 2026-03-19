#pragma once
#include "../CustomNodes/CustomNode.h"
#include "../CustomNodes/CustomNode2.h"
#include "../CustomNodes/CustomNode3.h"
#include "../CustomNodes/CustomNode4.h"
#include "../CustomNodes/CustomNode5.h"
#include "../CustomNodes/CustomNodeStyleDemonstration.h"

enum class NodeVariableSupportedType
{
	BOOL,
	INT,
	FLOAT,
	VEC2,
	BVEC2,
	VEC3,
	BVEC3,
	VEC4,
	BVEC4
};

class TestTools
{
	SINGLETON_PRIVATE_PART(TestTools)


public:
	SINGLETON_PUBLIC_PART(TestTools)

	// Will delete original NodeArea and create a new one from the saved file, then compare some of the properties of the new NodeArea to the original one.
	bool VerifyNodeAreaSaveLoadCycle_BasicChecks(VisNodeSys::NodeArea* NodeArea);

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

	// Verifies that no LinkNodes remain in the given area.
	bool VerifyNoLinkNodes(VisNodeSys::NodeArea* Area);

	// Verifies that no dangling LinkNodes remain in the given area.
	bool VerifyNoDanglingLinkNodes(VisNodeSys::NodeArea* Area);

	// Creates 30 NodeAreas linked in the same 5-level tree hierarchy.
	std::vector<VisNodeSys::NodeArea*> CreateSmallLinkedNodeAreaGraph();
	std::vector<VisNodeSys::NodeArea*> GetOrderedAreasFromSmallLinkedNodeAreaGraph();
	bool VerifyLinksInSmallNodeAreaGraph();

	// Given existing NodeAreas produced by CreateSmallLinkedNodeAreaGraph() establishes connection bettween begin node in 0th node and link execute in others.
	void ConnectSmallLinkedNodeAreaGraph();

	// ************************ Execution flow test tools ************************
	NodeVariableSupportedType GetRandomNodeVariableType();

	bool GetRandomBoolValue();
	BoolLiteralNode* CreateBoolLiteralNode(bool bValue);
	BoolVariableNode* CreateBoolVariableNode(bool bValue);

	int GetRandomIntValue();
	IntegerLiteralNode* CreateIntegerLiteralNode(int Value);
	IntegerVariableNode* CreateIntegerVariableNode(int Value);

	float GetRandomFloatValue();
	FloatLiteralNode* CreateFloatLiteralNode(float Value);
	FloatVariableNode* CreateFloatVariableNode(float Value);

	glm::vec2 GetRandomVec2Value();
	Vec2LiteralNode* CreateVec2LiteralNode(glm::vec2 Value);
	Vec2VariableNode* CreateVec2VariableNode(glm::vec2 Value);

	glm::bvec2 GetRandomBVec2Value();
	BoolVec2LiteralNode* CreateBVec2LiteralNode(glm::bvec2 Value);
	BoolVec2VariableNode* CreateBVec2VariableNode(glm::bvec2 Value);

	LogicalNodeOperatorType GetRandomLogicalOperatorType();
	BaseLogicalOperatorNode* CreateBaseLogicalOperatorNode(LogicalNodeOperatorType Type);
	bool GetResultFromLogicalOperator(LogicalNodeOperatorType Type, bool A, bool B);

	ComparisonNodeOperatorType GetRandomComparisonOperatorType();
	BaseComparisonOperatorNode* CreateBaseComparisonOperatorNode(ComparisonNodeOperatorType Type);
	bool GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, int A, int B);
	bool GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, float A, float B);
	glm::bvec2 GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, glm::vec2 A, glm::vec2 B);

	ArithmeticOperationType GetRandomArithmeticOperatorType();
	BaseArithmeticOperatorNode* CreateBaseArithmeticOperatorNode(ArithmeticOperationType Type);
	int GetResultFromArithmeticOperator(ArithmeticOperationType Type, int A, int B);
	float GetResultFromArithmeticOperator(ArithmeticOperationType Type, float A, float B);
	glm::vec2 GetResultFromArithmeticOperator(ArithmeticOperationType Type, glm::vec2 A, glm::vec2 B);
};

#define TEST_TOOLS TestTools::GetInstance()