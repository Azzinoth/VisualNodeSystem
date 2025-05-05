#include "VisualNodeSystem.h"
using namespace VisNodeSys;

#ifdef VISUAL_NODE_SYSTEM_SHARED
extern "C" __declspec(dllexport) void* GetNodeSystem()
{
	return NodeSystem::GetInstancePointer();
}
#endif

#ifdef VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES
	void NodeSystem::RegisterStandardNodes()
	{
		NODE_FACTORY.RegisterNodeType("BaseExecutionFlowNode",
			[]() -> Node* {
				return new BaseExecutionFlowNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const BaseExecutionFlowNode& NodeToCopy = static_cast<const BaseExecutionFlowNode&>(CurrentNode);
				return new BaseExecutionFlowNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BoolLiteralNode",
			[]() -> Node* {
				return new BoolLiteralNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const BoolLiteralNode& NodeToCopy = static_cast<const BoolLiteralNode&>(CurrentNode);
				return new BoolLiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("IntegerLiteralNode",
			[]() -> Node* {
				return new IntegerLiteralNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const IntegerLiteralNode& NodeToCopy = static_cast<const IntegerLiteralNode&>(CurrentNode);
				return new IntegerLiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("FloatLiteralNode",
			[]() -> Node* {
				return new FloatLiteralNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const FloatLiteralNode& NodeToCopy = static_cast<const FloatLiteralNode&>(CurrentNode);
				return new FloatLiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("Vec2LiteralNode",
			[]() -> Node* {
				return new Vec2LiteralNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const Vec2LiteralNode& NodeToCopy = static_cast<const Vec2LiteralNode&>(CurrentNode);
				return new Vec2LiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BoolVec2LiteralNode",
			[]() -> Node* {
				return new BoolVec2LiteralNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const BoolVec2LiteralNode& NodeToCopy = static_cast<const BoolVec2LiteralNode&>(CurrentNode);
				return new BoolVec2LiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("Vec3LiteralNode",
			[]() -> Node* {
				return new Vec3LiteralNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const Vec3LiteralNode& NodeToCopy = static_cast<const Vec3LiteralNode&>(CurrentNode);
				return new Vec3LiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BoolVec3LiteralNode",
			[]() -> Node* {
				return new BoolVec3LiteralNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const BoolVec3LiteralNode& NodeToCopy = static_cast<const BoolVec3LiteralNode&>(CurrentNode);
				return new BoolVec3LiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("Vec4LiteralNode",
			[]() -> Node* {
				return new Vec4LiteralNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const Vec4LiteralNode& NodeToCopy = static_cast<const Vec4LiteralNode&>(CurrentNode);
				return new Vec4LiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BoolVec4LiteralNode",
			[]() -> Node* {
				return new BoolVec4LiteralNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const BoolVec4LiteralNode& NodeToCopy = static_cast<const BoolVec4LiteralNode&>(CurrentNode);
				return new BoolVec4LiteralNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BoolVariableNode",
			[]() -> Node* {
				return new BoolVariableNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const BoolVariableNode& NodeToCopy = static_cast<const BoolVariableNode&>(CurrentNode);
				return new BoolVariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("IntegerVariableNode",
			[]() -> Node* {
				return new IntegerVariableNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const IntegerVariableNode& NodeToCopy = static_cast<const IntegerVariableNode&>(CurrentNode);
				return new IntegerVariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("FloatVariableNode",
			[]() -> Node* {
				return new FloatVariableNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const FloatVariableNode& NodeToCopy = static_cast<const FloatVariableNode&>(CurrentNode);
				return new FloatVariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("Vec2VariableNode",
			[]() -> Node* {
				return new Vec2VariableNode();
			},
		
			[](const Node& CurrentNode) -> Node* {
				const Vec2VariableNode& NodeToCopy = static_cast<const Vec2VariableNode&>(CurrentNode);
				return new Vec2VariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BoolVec2VariableNode",
			[]() -> Node* {
				return new BoolVec2VariableNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const BoolVec2VariableNode& NodeToCopy = static_cast<const BoolVec2VariableNode&>(CurrentNode);
				return new BoolVec2VariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("Vec3VariableNode",
			[]() -> Node* {
				return new Vec3VariableNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const Vec3VariableNode& NodeToCopy = static_cast<const Vec3VariableNode&>(CurrentNode);
				return new Vec3VariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BoolVec3VariableNode",
			[]() -> Node* {
				return new BoolVec3VariableNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const BoolVec3VariableNode& NodeToCopy = static_cast<const BoolVec3VariableNode&>(CurrentNode);
				return new BoolVec3VariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("Vec4VariableNode",
			[]() -> Node* {
				return new Vec4VariableNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const Vec4VariableNode& NodeToCopy = static_cast<const Vec4VariableNode&>(CurrentNode);
				return new Vec4VariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BoolVec4VariableNode",
			[]() -> Node* {
				return new BoolVec4VariableNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const BoolVec4VariableNode& NodeToCopy = static_cast<const BoolVec4VariableNode&>(CurrentNode);
				return new BoolVec4VariableNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BranchNode",
			[]() -> Node* {
				return new BranchNode();
			},
	
			[](const Node& CurrentNode) -> Node* {
				const BranchNode& NodeToCopy = static_cast<const BranchNode&>(CurrentNode);
				return new BranchNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("LoopNode",
			[]() -> Node* {
				return new LoopNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const LoopNode& NodeToCopy = static_cast<const LoopNode&>(CurrentNode);
				return new LoopNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("SequenceNode",
			[]() -> Node* {
				return new SequenceNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const SequenceNode& NodeToCopy = static_cast<const SequenceNode&>(CurrentNode);
				return new SequenceNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("WhileLoopNode",
			[]() -> Node* {
				return new WhileLoopNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const WhileLoopNode& NodeToCopy = static_cast<const WhileLoopNode&>(CurrentNode);
				return new WhileLoopNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("BaseArithmeticOperatorNode",
			[]() -> Node* {
				return new BaseArithmeticOperatorNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const BaseArithmeticOperatorNode& NodeToCopy = static_cast<const BaseArithmeticOperatorNode&>(CurrentNode);
				return new BaseArithmeticOperatorNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("ArithmeticAddNode",
			[]() -> Node* {
				return new ArithmeticAddNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const ArithmeticAddNode& NodeToCopy = static_cast<const ArithmeticAddNode&>(CurrentNode);
				return new ArithmeticAddNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("ArithmeticSubtractNode",
			[]() -> Node* {
				return new ArithmeticSubtractNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const ArithmeticSubtractNode& NodeToCopy = static_cast<const ArithmeticSubtractNode&>(CurrentNode);
				return new ArithmeticSubtractNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("ArithmeticMultiplyNode",
			[]() -> Node* {
				return new ArithmeticMultiplyNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const ArithmeticMultiplyNode& NodeToCopy = static_cast<const ArithmeticMultiplyNode&>(CurrentNode);
				return new ArithmeticMultiplyNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("ArithmeticDivideNode",
			[]() -> Node* {
				return new ArithmeticDivideNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const ArithmeticDivideNode& NodeToCopy = static_cast<const ArithmeticDivideNode&>(CurrentNode);
				return new ArithmeticDivideNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("ArithmeticPowerNode",
			[]() -> Node* {
				return new ArithmeticPowerNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const ArithmeticPowerNode& NodeToCopy = static_cast<const ArithmeticPowerNode&>(CurrentNode);
				return new ArithmeticPowerNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("ArithmeticModulusNode",
			[]() -> Node* {
				return new ArithmeticModulusNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const ArithmeticModulusNode& NodeToCopy = static_cast<const ArithmeticModulusNode&>(CurrentNode);
				return new ArithmeticModulusNode(NodeToCopy);
			}
		);


		NODE_FACTORY.RegisterNodeType("BaseComparisonOperatorNode",
			[]() -> Node* {
				return new BaseComparisonOperatorNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const BaseComparisonOperatorNode& NodeToCopy = static_cast<const BaseComparisonOperatorNode&>(CurrentNode);
				return new BaseComparisonOperatorNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("EqualNode",
			[]() -> Node* {
				return new EqualNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const EqualNode& NodeToCopy = static_cast<const EqualNode&>(CurrentNode);
				return new EqualNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("NotEqualNode",
			[]() -> Node* {
				return new NotEqualNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const NotEqualNode& NodeToCopy = static_cast<const NotEqualNode&>(CurrentNode);
				return new NotEqualNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("LessThanNode",
			[]() -> Node* {
				return new LessThanNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const LessThanNode& NodeToCopy = static_cast<const LessThanNode&>(CurrentNode);
				return new LessThanNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("LessThanOrEqualNode",
			[]() -> Node* {
				return new LessThanOrEqualNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const LessThanOrEqualNode& NodeToCopy = static_cast<const LessThanOrEqualNode&>(CurrentNode);
				return new LessThanOrEqualNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("GreaterThanNode",
			[]() -> Node* {
				return new GreaterThanNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const GreaterThanNode& NodeToCopy = static_cast<const GreaterThanNode&>(CurrentNode);
				return new GreaterThanNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("GreaterThanOrEqualNode",
			[]() -> Node* {
				return new GreaterThanOrEqualNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const GreaterThanOrEqualNode& NodeToCopy = static_cast<const GreaterThanOrEqualNode&>(CurrentNode);
				return new GreaterThanOrEqualNode(NodeToCopy);
			}
		);


		NODE_FACTORY.RegisterNodeType("BaseLogicalOperatorNode",
			[]() -> Node* {
				return new BaseLogicalOperatorNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const BaseLogicalOperatorNode& NodeToCopy = static_cast<const BaseLogicalOperatorNode&>(CurrentNode);
				return new BaseLogicalOperatorNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("LogicalANDOperatorNode",
			[]() -> Node* {
				return new LogicalANDOperatorNode();
			},

			[](const Node& CurrentNode) -> Node* {
				const LogicalANDOperatorNode& NodeToCopy = static_cast<const LogicalANDOperatorNode&>(CurrentNode);
				return new LogicalANDOperatorNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("LogicalOROperatorNode",
			[]() -> Node* {
				return new LogicalOROperatorNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const LogicalOROperatorNode& NodeToCopy = static_cast<const LogicalOROperatorNode&>(CurrentNode);
				return new LogicalOROperatorNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("LogicalXOROperatorNode",
			[]() -> Node* {
				return new LogicalXOROperatorNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const LogicalXOROperatorNode& NodeToCopy = static_cast<const LogicalXOROperatorNode&>(CurrentNode);
				return new LogicalXOROperatorNode(NodeToCopy);
			}
		);

		NODE_FACTORY.RegisterNodeType("LogicalNOTOperatorNode",
			[]() -> Node* {
				return new LogicalNOTOperatorNode();
			},
			[](const Node& CurrentNode) -> Node* {
				const LogicalNOTOperatorNode& NodeToCopy = static_cast<const LogicalNOTOperatorNode&>(CurrentNode);
				return new LogicalNOTOperatorNode(NodeToCopy);
			}
		);
	}
#endif


NodeSystem::NodeSystem()
{
#ifdef VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES
	RegisterStandardNodes();
#endif
}

NodeSystem::~NodeSystem() {}

void NodeSystem::Initialize(bool bTestMode)
{
	NODE_CORE.bIsInTestMode = bTestMode;
	NODE_CORE.InitializeFonts();

#ifdef VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES
	AssociateSocketTypeToColor("EXECUTE", ImColor(255, 255, 255));
	AssociateSocketTypeToColor("BOOL", ImColor(148, 0, 0));
	AssociateSocketTypeToColor("INT", ImColor(30, 221, 170));
	AssociateSocketTypeToColor("FLOAT", ImColor(56, 210, 0));
	AssociateSocketTypeToColor("VEC2", ImColor(244, 193, 34));
	AssociateSocketTypeToColor("BVEC2", ImColor(125, 62, 11));
	AssociateSocketTypeToColor("VEC3", ImColor(244, 193, 34));
	AssociateSocketTypeToColor("BVEC3", ImColor(125, 62, 11));
	AssociateSocketTypeToColor("VEC4", ImColor(244, 193, 34));
	AssociateSocketTypeToColor("BVEC4", ImColor(125, 62, 11));
#endif
}

NodeArea* NodeSystem::CreateNodeArea()
{
	CreatedAreas.push_back(new NodeArea());
	return CreatedAreas.back();
}

void NodeSystem::DeleteNodeArea(const NodeArea* NodeArea)
{
	for (size_t i = 0; i < CreatedAreas.size(); i++)
	{
		if (CreatedAreas[i] == NodeArea)
		{
			delete CreatedAreas[i];
			CreatedAreas.erase(CreatedAreas.begin() + i, CreatedAreas.begin() + i + 1);
			return;
		}
	}
}

void NodeSystem::MoveNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea, const bool SelectMovedNodes)
{
	if (SourceNodeArea == nullptr || TargetNodeArea == nullptr)
		return;

	for (size_t i = 0; i < SourceNodeArea->Nodes.size(); i++)
	{
		TargetNodeArea->AddNode(SourceNodeArea->Nodes[i]);
	}
	const size_t SourceNodeCount = SourceNodeArea->Nodes.size();
	SourceNodeArea->Nodes.clear();

	for (size_t i = 0; i < SourceNodeArea->Connections.size(); i++)
	{
		TargetNodeArea->Connections.push_back(SourceNodeArea->Connections[i]);
	}
	SourceNodeArea->Connections.clear();
	SourceNodeArea->Clear();

	// Select moved nodes.
	if (SelectMovedNodes)
	{
		TargetNodeArea->SelectedNodes.clear();
		for (size_t i = TargetNodeArea->Nodes.size() - SourceNodeCount; i < TargetNodeArea->Nodes.size(); i++)
		{
			TargetNodeArea->SelectedNodes.push_back(TargetNodeArea->Nodes[i]);
		}
	}
}

std::vector<std::pair<std::string, ImColor>> NodeSystem::GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color)
{
	std::vector<std::pair<std::string, ImColor>> Result;
	auto iterator = NodeSocket::SocketTypeToColorAssociations.begin();
	while (iterator != NodeSocket::SocketTypeToColorAssociations.end())
	{
		Result.push_back(std::make_pair(iterator->first, iterator->second));
		iterator++;
	}

	return Result;
}

void NodeSystem::AssociateSocketTypeToColor(std::string SocketType, ImColor Color)
{
	NodeSocket::SocketTypeToColorAssociations[SocketType] = Color;
}