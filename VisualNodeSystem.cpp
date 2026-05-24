#include "VisualNodeSystem.h"
using namespace VisNodeSys;
#include <unordered_set>

#ifdef VISUAL_NODE_SYSTEM_SHARED
extern "C" __declspec(dllexport) void* GetNodeSystem()
{
	return NodeSystem::GetInstancePointer();
}
#endif

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
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
#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
	RegisterStandardNodes();
#endif
}

NodeSystem::~NodeSystem() {}

#include "VersionInfo/VISUAL_NODE_SYSTEM_Version.h"
#include "VersionInfo/FEVersionInfo.h"
FE_DEFINE_VERSION_INFO(VISUAL_NODE_SYSTEM_)

std::string NodeSystem::GetVersion()
{
	return GetVISUAL_NODE_SYSTEM_VersionInfo().GetVersion();
}

std::string NodeSystem::GetFullVersion()
{
	return "Visual Node System " + GetVISUAL_NODE_SYSTEM_VersionInfo().GetFullVersionString();
}

void NodeSystem::Initialize(bool bTestMode)
{
	NODE_CORE.bIsInTestMode = bTestMode;
	NODE_CORE.InitializeFonts();

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
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
	Areas.push_back(new NodeArea());
	return Areas.back();
}

bool NodeSystem::IsAlreadyConnected(NodeSocket* FirstSocket, NodeSocket* SecondSocket, const std::vector<Connection*>& Connections)
{
	for (size_t i = 0; i < Connections.size(); i++)
	{
		// If the node is connected to a node that is not in this list, just ignore.
		if (Connections[i]->In == FirstSocket && Connections[i]->Out == SecondSocket)
			return true;
	}

	return false;
}

void NodeSystem::ProcessConnections(const std::vector<NodeSocket*>& Sockets,
									std::unordered_map<NodeSocket*, NodeSocket*>& OldToNewSocket,
									NodeArea* TargetArea, size_t NodeShift, const std::vector<Node*>& SourceNodes)
{
	NodeArea* SourceArea = SourceNodes[0]->GetParentArea();

	for (size_t i = 0; i < Sockets.size(); i++)
	{
		NodeSocket* CurrentSocket = Sockets[i];
		for (const auto& ConnectedSocket : CurrentSocket->ConnectedSockets)
		{
			if (Node::IsNodeWithIDInList(ConnectedSocket->GetParent()->GetID(), SourceNodes))
			{
				// Check if we have already established this connection.
				if (!IsAlreadyConnected(OldToNewSocket[CurrentSocket], OldToNewSocket[ConnectedSocket], TargetArea->Connections))
				{
					std::unordered_map<RerouteNode*, RerouteNode*> OldToNewRerouteNode;
					// Get connection info from old node area.
					Connection* OldConnection = SourceArea->GetConnection(CurrentSocket, ConnectedSocket);

					if (!TargetArea->TryToConnect(OldToNewSocket[CurrentSocket]->GetParent(), OldToNewSocket[CurrentSocket]->GetID(), OldToNewSocket[ConnectedSocket]->GetParent(), OldToNewSocket[ConnectedSocket]->GetID()))
						continue;

					Connection* NewConnection = TargetArea->Connections.back();
					// First pass to fill OldToNewRerouteNode map and other information that does not depend on OldToNewRerouteNode map.
					for (size_t j = 0; j < OldConnection->RerouteNodes.size(); j++)
					{
						RerouteNode* OldReroute = OldConnection->RerouteNodes[j];
						RerouteNode* NewReroute = new RerouteNode();
						NewReroute->ID = NODE_CORE.GetUniqueHexID();
						NewReroute->Parent = NewConnection;
						NewReroute->Position = OldReroute->Position;

						if (OldReroute->BeginSocket != nullptr)
							NewReroute->BeginSocket = OldToNewSocket[OldReroute->BeginSocket];
						if (OldReroute->EndSocket != nullptr)
							NewReroute->EndSocket = OldToNewSocket[OldReroute->EndSocket];

						// Associate old to new
						OldToNewRerouteNode[OldConnection->RerouteNodes[j]] = NewReroute;

						NewConnection->RerouteNodes.push_back(NewReroute);
					}

					// Second pass to fill all other info.
					for (size_t j = 0; j < OldConnection->RerouteNodes.size(); j++)
					{
						RerouteNode* OldReroute = OldConnection->RerouteNodes[j];

						if (OldReroute->BeginReroute != nullptr)
							OldToNewRerouteNode[OldReroute]->BeginReroute = OldToNewRerouteNode[OldReroute->BeginReroute];
						if (OldReroute->EndReroute != nullptr)
							OldToNewRerouteNode[OldReroute]->EndReroute = OldToNewRerouteNode[OldReroute->EndReroute];
					}
				}
			}
		}
	}
}

void NodeSystem::CopyNodesInternal(const std::vector<Node*>& SourceNodes, NodeArea* TargetArea, const size_t NodeShift)
{
	// Copy all nodes to new node area.
	std::unordered_map<Node*, Node*> OldToNewNode;
	std::unordered_map<NodeSocket*, NodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < SourceNodes.size(); i++)
	{
		if (SourceNodes[i] == nullptr)
			continue;

		Node* CopyOfNode = NODE_FACTORY.CopyNode(SourceNodes[i]->GetType(), *SourceNodes[i]);

		if (CopyOfNode == nullptr)
			CopyOfNode = new Node(*SourceNodes[i]);

		TargetArea->AddNode(CopyOfNode);

		// Associate old to new
		OldToNewNode[SourceNodes[i]] = CopyOfNode;

		for (size_t j = 0; j < SourceNodes[i]->Input.size(); j++)
		{
			OldToNewSocket[SourceNodes[i]->Input[j]] = CopyOfNode->Input[j];
		}

		for (size_t j = 0; j < SourceNodes[i]->Output.size(); j++)
		{
			OldToNewSocket[SourceNodes[i]->Output[j]] = CopyOfNode->Output[j];
		}
	}

	// Recreate all connections.
	for (size_t i = 0; i < SourceNodes.size(); i++)
	{
		if (SourceNodes[i] == nullptr)
			continue;

		ProcessConnections(SourceNodes[i]->Output, OldToNewSocket, TargetArea, NodeShift + i, SourceNodes);
	}
}

NodeArea* NodeSystem::CreateNodeArea(const std::vector<Node*> Nodes, const std::vector<GroupComment*> GroupComments)
{
	NodeArea* NewArea = CreateNodeArea();
	CopyNodesInternal(Nodes, NewArea);

	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i] == nullptr)
			continue;

		GroupComment* CopyOfGroupComment = new GroupComment(*GroupComments[i]);
		NewArea->AddGroupComment(CopyOfGroupComment);
	}

	return NewArea;
}

void NodeSystem::CopyNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea)
{
	if (SourceNodeArea == nullptr || TargetNodeArea == nullptr)
		return;

	const size_t NodeShift = TargetNodeArea->Nodes.size();
	CopyNodesInternal(SourceNodeArea->Nodes, TargetNodeArea, NodeShift);

	for (size_t i = 0; i < SourceNodeArea->GroupComments.size(); i++)
	{
		GroupComment* CopyOfGroupComment = new GroupComment(*SourceNodeArea->GroupComments[i]);
		TargetNodeArea->AddGroupComment(CopyOfGroupComment);
	}
}

NodeArea* NodeSystem::GetNodeAreaByID(const std::string& NodeAreaID) const
{
	for (size_t i = 0; i < Areas.size(); i++)
	{
		if (Areas[i]->GetID() == NodeAreaID)
			return Areas[i];
	}

	return nullptr;
}

std::vector<NodeArea*> NodeSystem::GetNodeAreasByName(const std::string& Name) const
{
	std::vector<NodeArea*> Result;
	for (size_t i = 0; i < Areas.size(); i++)
	{
		if (Areas[i]->GetName() == Name)
			Result.push_back(Areas[i]);
	}

	return Result;
}

void NodeSystem::OnNodeDeletion(Node* DeletedNode)
{
	if (DeletedNode->GetType() != "LinkNode")
		return;

	// We care only about nodes that are part of a link between areas.
	NodeAreaLinkRecord* Data = GetLinkDataByNodeID(DeletedNode->GetID());
	if (Data == nullptr)
		return;

	std::string LinkID = Data->ID;
	LinkNode* CurrentLinkNode = static_cast<LinkNode*>(DeletedNode);
	NodeArea* LinkedArea = CurrentLinkNode->GetLinkedArea();
	if (LinkedArea != nullptr)
	{
		Node* PartnerNode = LinkedArea->GetNodeByID(CurrentLinkNode->PartnerNodeID);
		if (PartnerNode != nullptr)
		{
			LinkNode* CastedPartnerNode = static_cast<LinkNode*>(PartnerNode);
			if (!CastedPartnerNode->bIsInProcessOfBeingDestroyed)
			{
				CastedPartnerNode->bIsInProcessOfBeingDestroyed = true;
				LinkedArea->Delete(CastedPartnerNode);
			}
		}
	}
		
	DeleteLinkRecord(LinkID);
}

void NodeSystem::DeleteNodeArea(const NodeArea* NodeAreaToDelete)
{
	if (NodeAreaToDelete == nullptr)
		return;

	// This area could be owned by a SubAreaNode in another area.
	// If it is, delete the SubAreaNode that owns this area, which will also delete this area and all its child areas.
	SubAreaNode* ParentSubAreaNode = FindOwnerSubAreaNode(NodeAreaToDelete->GetID());
	if (ParentSubAreaNode != nullptr)
	{
		NodeArea* ParentArea = ParentSubAreaNode->GetParentArea();
		if (ParentArea != nullptr)
		{
			ParentArea->Delete(ParentSubAreaNode);
			return;
		}
	}

	for (size_t i = 0; i < Areas.size(); i++)
	{
		if (Areas[i] == NodeAreaToDelete)
		{
			NodeArea* AreaToDelete = Areas[i];
			// Remove from the vector before deleting.
			// During deletion, potential SubAreaNodes in this area will destroy their owned child areas, which modifies the Areas vector.
			Areas.erase(Areas.begin() + i);
			delete AreaToDelete;
			return;
		}
	}
}

void NodeSystem::DeleteNodeAreaByID(const std::string& NodeAreaID)
{
	NodeArea* AreaToDelete = GetNodeAreaByID(NodeAreaID);
	if (AreaToDelete != nullptr)
		DeleteNodeArea(AreaToDelete);
}

size_t NodeSystem::GetNodeAreaCount() const
{
	return Areas.size();
}

size_t NodeSystem::GetTotalNodeCount(std::vector<std::string> AreaIDFilter) const
{
	if (!AreaIDFilter.empty())
		AreaIDFilter = DeduplicateIDList(AreaIDFilter);

	size_t Result = 0;
	if (AreaIDFilter.empty())
	{
		for (size_t i = 0; i < Areas.size(); i++)
			Result += Areas[i]->GetNodeCount();
	}
	else
	{
		for (size_t i = 0; i < AreaIDFilter.size(); i++)
		{
			NodeArea* Area = GetNodeAreaByID(AreaIDFilter[i]);
			if (Area != nullptr)
				Result += Area->GetNodeCount();
		}
	}
	
	return Result;
}

size_t NodeSystem::GetTotalConnectionCount(std::vector<std::string> AreaIDFilter) const
{
	if (!AreaIDFilter.empty())
		AreaIDFilter = DeduplicateIDList(AreaIDFilter);

	size_t Result = 0;
	if (AreaIDFilter.empty())
	{
		for (size_t i = 0; i < Areas.size(); i++)
			Result += Areas[i]->GetConnectionCount();
	}
	else
	{
		for (size_t i = 0; i < AreaIDFilter.size(); i++)
		{
			NodeArea* Area = GetNodeAreaByID(AreaIDFilter[i]);
			if (Area != nullptr)
				Result += Area->GetConnectionCount();
		}
	}

	return Result;
}

size_t NodeSystem::GetGroupCommentCount(std::vector<std::string> AreaIDFilter) const
{
	if (!AreaIDFilter.empty())
		AreaIDFilter = DeduplicateIDList(AreaIDFilter);

	size_t Result = 0;
	if (AreaIDFilter.empty())
	{
		for (size_t i = 0; i < Areas.size(); i++)
			Result += Areas[i]->GetGroupCommentCount();
	}
	else
	{
		for (size_t i = 0; i < AreaIDFilter.size(); i++)
		{
			NodeArea* Area = GetNodeAreaByID(AreaIDFilter[i]);
			if (Area != nullptr)
				Result += Area->GetGroupCommentCount();
		}
	}

	return Result;
}

size_t NodeSystem::GetRerouteConnectionCount(std::vector<std::string> AreaIDFilter) const
{
	if (!AreaIDFilter.empty())
		AreaIDFilter = DeduplicateIDList(AreaIDFilter);

	size_t Result = 0;
	if (AreaIDFilter.empty())
	{
		for (size_t i = 0; i < Areas.size(); i++)
			Result += Areas[i]->GetRerouteConnectionCount();
	}
	else
	{
		for (size_t i = 0; i < AreaIDFilter.size(); i++)
		{
			NodeArea* Area = GetNodeAreaByID(AreaIDFilter[i]);
			if (Area != nullptr)
				Result += Area->GetRerouteConnectionCount();
		}
	}

	return Result;
}

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
std::unordered_map<std::string, std::vector<Node*>> NodeSystem::GetLastExecutedNodes(std::string StartingAreaID) const
{
	std::unordered_map<std::string, std::vector<Node*>> Result;
	if (StartingAreaID.empty())
	{
		for (size_t i = 0; i < Areas.size(); i++)
		{
			std::vector<Node*> ExecutedNodes = Areas[i]->GetLastExecutedNodes();
			if (!ExecutedNodes.empty())
				Result[Areas[i]->GetID()] = ExecutedNodes;
		}
	}
	else
	{
		NodeArea* StartingArea = GetNodeAreaByID(StartingAreaID);
		if (StartingArea == nullptr)
			return Result;

		std::vector<NodeArea*> AreasToCheck = { StartingArea };
		std::vector<NodeArea*> AllDownstreamAreas = NODE_SYSTEM.GetAllDownstreamAreas(StartingAreaID);
		AreasToCheck.insert(AreasToCheck.end(), AllDownstreamAreas.begin(), AllDownstreamAreas.end());
		for (size_t i = 0; i < AreasToCheck.size(); i++)
		{
			std::vector<Node*> ExecutedNodes = AreasToCheck[i]->GetLastExecutedNodes();
			if (!ExecutedNodes.empty())
				Result[AreasToCheck[i]->GetID()] = ExecutedNodes;
		}
	}

	return Result;
}
#endif

bool NodeSystem::MoveNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea, const bool bSelectNodesAfterMovement)
{
	if (SourceNodeArea == nullptr || TargetNodeArea == nullptr || SourceNodeArea == TargetNodeArea)
		return false;

	for (size_t i = 0; i < SourceNodeArea->Nodes.size(); i++)
		TargetNodeArea->AddNode(SourceNodeArea->Nodes[i]);

	const size_t SourceNodeCount = SourceNodeArea->Nodes.size();
	SourceNodeArea->Nodes.clear();

	for (size_t i = 0; i < SourceNodeArea->Connections.size(); i++)
		TargetNodeArea->Connections.push_back(SourceNodeArea->Connections[i]);

	SourceNodeArea->Connections.clear();
	SourceNodeArea->Clear();

	// Select moved nodes.
	if (bSelectNodesAfterMovement)
	{
		TargetNodeArea->SelectedNodes.clear();
		for (size_t i = TargetNodeArea->Nodes.size() - SourceNodeCount; i < TargetNodeArea->Nodes.size(); i++)
			TargetNodeArea->SelectedNodes.push_back(TargetNodeArea->Nodes[i]);
	}

	return true;
}

NodeSystem::NodeAreaLinkRecord* NodeSystem::GetLinkDataByNodeID(const std::string& NodeID)
{
	auto RecordIterator = NodeAreaLinkRecords.begin();
	while (RecordIterator != NodeAreaLinkRecords.end())
	{
		if (RecordIterator->second.InNodeID == NodeID || RecordIterator->second.OutNodeID == NodeID)
			return &RecordIterator->second;

		RecordIterator++;
	}

	return nullptr;
}

std::vector<NodeSystem::NodeAreaLinkRecord*> NodeSystem::GetLinkDataByAreaID(const std::string& AreaID)
{
	auto RecordIterator = NodeAreaLinkRecords.begin();
	std::vector<NodeAreaLinkRecord*> Result;
	while (RecordIterator != NodeAreaLinkRecords.end())
	{
		if (RecordIterator->second.InAreaID == AreaID || RecordIterator->second.OutAreaID == AreaID)
			Result.push_back(&RecordIterator->second);
		
		RecordIterator++;
	}
	
	return Result;
}

bool NodeSystem::LinkNodeAreas(const std::string& UpstreamAreaID, const std::string& DownstreamAreaID, std::pair<std::string, std::string>* CreatedLinkNodeIDs)
{
	NodeArea* UpstreamArea = GetNodeAreaByID(UpstreamAreaID);
	NodeArea* DownstreamArea = GetNodeAreaByID(DownstreamAreaID);
	if (UpstreamArea == nullptr || DownstreamArea == nullptr || UpstreamArea == DownstreamArea)
		return false;

	LinkNode* InNode = CreateLinkNodeInternal(true);
	if (!UpstreamArea->AddNode(InNode))
	{
		delete InNode;
		return false;
	}

	LinkNode* OutNode = CreateLinkNodeInternal(false);
	if (!DownstreamArea->AddNode(OutNode))
	{
		UpstreamArea->Delete(InNode);
		delete OutNode;
		return false;
	}

	InNode->PartnerNodeID = OutNode->GetID();
	InNode->bHaveInput = true;
	InNode->bHaveOutput = false;
	InNode->LinkedAreaID = DownstreamArea->GetID();

	OutNode->PartnerNodeID = InNode->GetID();
	OutNode->bHaveInput = false;
	OutNode->bHaveOutput = true;
	OutNode->LinkedAreaID = UpstreamArea->GetID();

	NodeAreaLinkRecord NewRecord;
	NewRecord.ID = InNode->GetID() + "_" + OutNode->GetID();
	NewRecord.InNodeID = InNode->GetID();
	NewRecord.OutNodeID = OutNode->GetID();
	NewRecord.InAreaID = UpstreamArea->GetID();
	NewRecord.OutAreaID = DownstreamArea->GetID();

	NodeAreaLinkRecords[NewRecord.ID] = NewRecord;

	if (CreatedLinkNodeIDs != nullptr)
		*CreatedLinkNodeIDs = std::make_pair(InNode->GetID(), OutNode->GetID());
	return true;
}

bool NodeSystem::IsLinked(const std::string& FirstAreaID, const std::string& SecondAreaID) const
{
	NodeArea* FirstArea = GetNodeAreaByID(FirstAreaID);
	NodeArea* SecondArea = GetNodeAreaByID(SecondAreaID);
	if (FirstArea == nullptr || SecondArea == nullptr)
		return false;

	auto RecordIterator = NodeAreaLinkRecords.begin();
	while (RecordIterator != NodeAreaLinkRecords.end())
	{
		NodeAreaLinkRecord Record = RecordIterator->second;
		if (Record.InAreaID == FirstArea->GetID() && Record.OutAreaID == SecondArea->GetID() ||
			Record.InAreaID == SecondArea->GetID() && Record.OutAreaID == FirstArea->GetID())
			return true;

		RecordIterator++;
	}

	return false;
}

bool NodeSystem::UnlinkNodeAreas(const std::string& FirstAreaID, const std::string& SecondAreaID)
{
	NodeArea* FirstArea = GetNodeAreaByID(FirstAreaID);
	NodeArea* SecondArea = GetNodeAreaByID(SecondAreaID);
	if (FirstArea == nullptr || SecondArea == nullptr)
		return false;

	// First pass: collect matching records.
	std::vector<std::string> RecordIDsToRemove;
	auto RecordIterator = NodeAreaLinkRecords.begin();
	while (RecordIterator != NodeAreaLinkRecords.end())
	{
		NodeAreaLinkRecord Record = RecordIterator->second;
		if ((Record.InAreaID == FirstArea->GetID() && Record.OutAreaID == SecondArea->GetID()) ||
			(Record.InAreaID == SecondArea->GetID() && Record.OutAreaID == FirstArea->GetID()))
		{
			RecordIDsToRemove.push_back(RecordIterator->first);
		}
		
		RecordIterator++;
	}

	// Second pass: delete nodes (which internally cleans up records too).
	for (const auto& RecordID : RecordIDsToRemove)
	{
		// Record might already be gone if a previous Delete removed it.
		auto LinkRecordIterator = NodeAreaLinkRecords.find(RecordID);
		if (LinkRecordIterator == NodeAreaLinkRecords.end())
			continue;

		NodeAreaLinkRecord Record = LinkRecordIterator->second;
		std::string RecordID = LinkRecordIterator->first;
		NodeArea* InArea = GetNodeAreaByID(Record.InAreaID);
		if (InArea != nullptr)
		{
			Node* InNode = InArea->GetNodeByID(Record.InNodeID);
			if (InNode != nullptr)
				InArea->Delete(InNode);
		}

		DeleteLinkRecord(RecordID);
	}

	return !RecordIDsToRemove.empty();
}

std::vector<std::pair<std::string, std::string>> NodeSystem::GetLinkingNodesForAreas(const std::string& FirstAreaID, const std::string& SecondAreaID) const
{
	NodeArea* FirstArea = GetNodeAreaByID(FirstAreaID);
	NodeArea* SecondArea = GetNodeAreaByID(SecondAreaID);
	std::vector<std::pair<std::string, std::string>> Result;
	if (FirstArea == nullptr || SecondArea == nullptr)
		return Result;

	auto RecordIterator = NodeAreaLinkRecords.begin();
	while (RecordIterator != NodeAreaLinkRecords.end())
	{
		NodeAreaLinkRecord Record = RecordIterator->second;
		if (Record.InAreaID == FirstArea->GetID() && Record.OutAreaID == SecondArea->GetID() ||
			Record.InAreaID == SecondArea->GetID() && Record.OutAreaID == FirstArea->GetID())
		{
			Node* InNode = GetNodeAreaByID(Record.InAreaID)->GetNodeByID(Record.InNodeID);
			Node* OutNode = GetNodeAreaByID(Record.OutAreaID)->GetNodeByID(Record.OutNodeID);

			if (InNode != nullptr && OutNode != nullptr)
				Result.push_back({ Record.InNodeID,Record.OutNodeID });
		}

		RecordIterator++;
	}

	return Result;
}

bool NodeSystem::DeleteLinkRecord(const std::string& LinkID)
{
	auto RecordIterator = NodeAreaLinkRecords.find(LinkID);
	if (RecordIterator == NodeAreaLinkRecords.end())
		return false;

	NodeAreaLinkRecords.erase(RecordIterator);
	return true;
}

std::vector<std::pair<std::string, ImColor>> NodeSystem::GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color)
{
	std::vector<std::pair<std::string, ImColor>> Result;
	auto ColorAssociationIterator = NodeSocket::SocketTypeToColorAssociations.begin();
	while (ColorAssociationIterator != NodeSocket::SocketTypeToColorAssociations.end())
	{
		Result.push_back(std::make_pair(ColorAssociationIterator->first, ColorAssociationIterator->second));
		ColorAssociationIterator++;
	}

	return Result;
}

void NodeSystem::AssociateSocketTypeToColor(std::string SocketType, ImColor Color)
{
	NodeSocket::SocketTypeToColorAssociations[SocketType] = Color;
}

void NodeSystem::OnNodeAreaFocusChanging(NodeArea* CurrentNodeArea, bool bNewFocusValue)
{
	if (CurrentNodeArea == nullptr)
		return;

	if (bNewFocusValue)
	{
		for (size_t i = 0; i < Areas.size(); i++)
		{
			if (Areas[i] != CurrentNodeArea)
				Areas[i]->SetFocusedInternal(false);
		}
	}
	else
	{
		// Focus lost.
	}
}

std::string NodeSystem::ToJson() const
{
	Json::Value Root;

	// Socket type to color associations.
	Json::Value SocketTypeToColorAssociationsJson(Json::objectValue);
	auto SocketTypeToColorIterator = NodeSocket::SocketTypeToColorAssociations.begin();
	while (SocketTypeToColorIterator != NodeSocket::SocketTypeToColorAssociations.end())
	{
		Json::Value ColorJson;
		ColorJson["R"] = SocketTypeToColorIterator->second.Value.x;
		ColorJson["G"] = SocketTypeToColorIterator->second.Value.y;
		ColorJson["B"] = SocketTypeToColorIterator->second.Value.z;
		ColorJson["A"] = SocketTypeToColorIterator->second.Value.w;
		SocketTypeToColorAssociationsJson[SocketTypeToColorIterator->first] = ColorJson;
		SocketTypeToColorIterator++;
	}
	Root["SocketTypeToColorAssociations"] = SocketTypeToColorAssociationsJson;

	// Finally we need to save all node areas.
	Json::Value NodeAreasJson(Json::objectValue);
	for (size_t i = 0; i < Areas.size(); i++)
		NodeAreasJson[Areas[i]->GetID()] = Areas[i]->ToJson();
	Root["NodeAreas"] = NodeAreasJson;

	Json::StreamWriterBuilder Builder;
	// Tweak the builder settings for a more compact JSON output.
	Builder.settings_["indentation"] = "";
	Builder.settings_["commentStyle"] = "None";
	const std::string JsonText = Json::writeString(Builder, Root);

	return JsonText;
}

bool NodeSystem::SaveToFile(const std::string& FilePath) const
{
	const std::string JsonFile = ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FilePath);
	if (!SaveFile.is_open())
		return false;
		
	SaveFile << JsonFile;
	SaveFile.close();
	return true;
}

bool NodeSystem::LoadFromJson(const std::string& JsonText)
{
	Clear();

	if (JsonText.find("{") == std::string::npos || JsonText.find("}") == std::string::npos || JsonText.find(":") == std::string::npos)
		return false;

	Json::Value Root;
	JSONCPP_STRING Error;
	Json::CharReaderBuilder Builder;

	const std::unique_ptr<Json::CharReader> Reader(Builder.newCharReader());
	if (!Reader->parse(JsonText.c_str(), JsonText.c_str() + JsonText.size(), &Root, &Error))
		return false;

	if (!Root.isObject())
		return false;

	if (!Root.isMember("SocketTypeToColorAssociations") || !Root.isMember("NodeAreas"))
		return false;

	if (!Root["NodeAreas"].isNull() && !Root["NodeAreas"].isObject())
		return false;

	if (!Root["SocketTypeToColorAssociations"].isNull() && !Root["SocketTypeToColorAssociations"].isObject())
		return false;

	// Socket type to color associations.
	std::vector<Json::String> SocketTypeToColorAssociationList = Root["SocketTypeToColorAssociations"].getMemberNames();
	for (size_t i = 0; i < SocketTypeToColorAssociationList.size(); i++)
	{
		if (!Root["SocketTypeToColorAssociations"][SocketTypeToColorAssociationList[i]].isObject())
			continue;

		const Json::Value& ColorJson = Root["SocketTypeToColorAssociations"][SocketTypeToColorAssociationList[i]];
		if (!ColorJson.isMember("R") || !ColorJson.isMember("G") || !ColorJson.isMember("B") || !ColorJson.isMember("A"))
			continue;

		if (!ColorJson["R"].isNumeric() || !ColorJson["G"].isNumeric() ||
			!ColorJson["B"].isNumeric() || !ColorJson["A"].isNumeric())
			continue;

		std::string SocketType = SocketTypeToColorAssociationList[i];
		float R = ColorJson["R"].asFloat();
		float G = ColorJson["G"].asFloat();
		float B = ColorJson["B"].asFloat();
		float A = ColorJson["A"].asFloat();
		ImColor Color(R, G, B, A);
		AssociateSocketTypeToColor(SocketType, Color);
	}

	std::vector<Json::String> NodeAreaListKeys = Root["NodeAreas"].getMemberNames();
	for (size_t i = 0; i < NodeAreaListKeys.size(); i++)
	{
		std::string NodeAreaID = NodeAreaListKeys[i];
		// We should check if that node was already loaded as a part of SubAreaNode to avoid loading it twice.
		NodeArea* AlreadyLoadedArea = GetNodeAreaByID(NodeAreaID);
		if (AlreadyLoadedArea != nullptr)
			continue;

		const Json::Value& AreaValue = Root["NodeAreas"][NodeAreaListKeys[i]];
		if (!AreaValue.isString())
			continue;

		NodeArea* NewNodeArea = CreateNodeArea();
		if (!NewNodeArea->LoadFromJson(AreaValue.asCString()))
			DeleteNodeArea(NewNodeArea);
	}

	// Breaking any cycles in sub-area ownership.
	BreakSubAreaOwnershipCycles();

	std::vector<LinkNode*> DanglingLinkNodes = GetDanglingLinkNodes();
	std::vector<LinkNode*> RecoveredDanglingLinkNodes = TryToFixAllDanglingLinkNodes();

	// Create a list of all link records that are still dangling after recovery attempt, to clean them up.
	std::vector<std::string> LinkRecordsToDelete;
	for (LinkNode* LinkNode : DanglingLinkNodes)
	{
		if (std::find(RecoveredDanglingLinkNodes.begin(), RecoveredDanglingLinkNodes.end(), LinkNode) == RecoveredDanglingLinkNodes.end())
		{
			NodeAreaLinkRecord* Record = GetLinkDataByNodeID(LinkNode->GetID());
			if (Record != nullptr)
				LinkRecordsToDelete.push_back(Record->ID);
		}
	}

	// FE_TO_DO: Should we notify user or leave dangling nodes in case user willing to fix them manually after loading?
	
	return true;
}

bool NodeSystem::LoadFromFile(const std::string& FilePath)
{
	std::ifstream NodeSystemFile;
	NodeSystemFile.open(FilePath);
	if (!NodeSystemFile.is_open())
		return false;

	const std::string FileData((std::istreambuf_iterator<char>(NodeSystemFile)), std::istreambuf_iterator<char>());
	NodeSystemFile.close();

	return LoadFromJson(FileData);
}

void NodeSystem::Clear()
{
	for (size_t i = 0; i < Areas.size(); i++)
	{
		delete Areas[i];
		Areas.erase(Areas.begin() + i, Areas.begin() + i + 1);
		i--;
	}

	NodeAreaLinkRecords.clear();
	NodeSocket::SocketTypeToColorAssociations.clear();
}

bool NodeSystem::IsInAListOfAreas(const std::string& AreaID, const std::vector<std::string>& AreaIDList) const
{
	for (size_t i = 0; i < AreaIDList.size(); i++)
	{
		if (AreaIDList[i] == AreaID)
			return true;
	}

	return false;
}

bool NodeSystem::IsInAListOfAreas(const NodeArea* Area, const std::vector<NodeArea*>& AreaIDList) const
{
	for (size_t i = 0; i < AreaIDList.size(); i++)
	{
		if (AreaIDList[i] == Area)
			return true;
	}

	return false;
}

std::vector<NodeArea*> NodeSystem::GetImmediateDownstreamAreas(const std::string& AreaID)
{
	std::vector<NodeArea*> Result;
	NodeArea* CurrentArea = GetNodeAreaByID(AreaID);
	if (CurrentArea == nullptr)
		return Result;

	std::vector<NodeAreaLinkRecord*> LinkRecords = GetLinkDataByAreaID(CurrentArea->GetID());
	for (int i = 0; i < LinkRecords.size(); i++)
	{
		if (LinkRecords[i]->InAreaID == CurrentArea->GetID())
		{
			NodeArea* DownstreamArea = GetNodeAreaByID(LinkRecords[i]->OutAreaID);
			if (DownstreamArea != nullptr)
			{
				// Check if we have already added this area to result to avoid duplicates in case of multiple links between same areas.
				if (!IsInAListOfAreas(DownstreamArea, Result))
					Result.push_back(DownstreamArea);
			}
		}
	}

	return Result;
}

std::vector<NodeArea*> NodeSystem::GetAllDownstreamAreas(const std::string& AreaID)
{
	NodeArea* CurrentNodeArea = GetNodeAreaByID(AreaID);
	std::vector<NodeArea*> Result;
	if (CurrentNodeArea == nullptr)
		return Result;

	// Using visited map to avoid infinite loops in case of cycles in the graph.
	std::unordered_map<std::string, bool> Visited;
	// We should add current area to visited to avoid adding it to result in case of cycles.
	Visited[CurrentNodeArea->GetID()] = true;
	std::function<void(const NodeArea*)> Collect = [&](const NodeArea* CurrentArea) {
		if (CurrentArea == nullptr)
			return;

		for (NodeArea* DownstreamArea : GetImmediateDownstreamAreas(CurrentArea->GetID()))
		{
			if (Visited.find(DownstreamArea->GetID()) == Visited.end())
			{
				Visited[DownstreamArea->GetID()] = true;
				Result.push_back(DownstreamArea);
				Collect(DownstreamArea);
			}
		}
	};

	Collect(CurrentNodeArea);
	return Result;
}

std::vector<NodeArea*> NodeSystem::GetImmediateUpstreamAreas(const std::string& AreaID)
{
	std::vector<NodeArea*> Result;
	NodeArea* CurrentNodeArea = GetNodeAreaByID(AreaID);
	if (CurrentNodeArea == nullptr)
		return Result;

	std::vector<NodeAreaLinkRecord*> LinkRecords = GetLinkDataByAreaID(CurrentNodeArea->GetID());
	for (int i = 0; i < LinkRecords.size(); i++)
	{
		if (LinkRecords[i]->OutAreaID == CurrentNodeArea->GetID())
		{
			NodeArea* UpstreamArea = GetNodeAreaByID(LinkRecords[i]->InAreaID);
			if (UpstreamArea != nullptr)
			{
				// Check if we have already added this area to result to avoid duplicates in case of multiple links between same areas.
				if (!IsInAListOfAreas(UpstreamArea, Result))
					Result.push_back(UpstreamArea);
			}
		}
	}

	return Result;
}

std::vector<NodeArea*> NodeSystem::GetAllUpstreamAreas(const std::string& AreaID)
{
	std::vector<NodeArea*> Result;
	NodeArea* CurrentNodeArea = GetNodeAreaByID(AreaID);
	if (CurrentNodeArea == nullptr)
		return Result;

	// Using visited map to avoid infinite loops in case of cycles in the graph.
	std::unordered_map<std::string, bool> Visited;
	// We should add current area to visited to avoid adding it to result in case of cycles.
	Visited[CurrentNodeArea->GetID()] = true;
	std::function<void(const NodeArea*)> Collect = [&](const NodeArea* CurrentArea) {
		if (CurrentArea == nullptr)
			return;

		for (NodeArea* UpstreamArea : GetImmediateUpstreamAreas(CurrentArea->GetID()))
		{
			if (Visited.find(UpstreamArea->GetID()) == Visited.end())
			{
				Visited[UpstreamArea->GetID()] = true;
				Result.push_back(UpstreamArea);
				Collect(UpstreamArea);
			}
		}
	};

	Collect(CurrentNodeArea);
	return Result;
}

std::vector<std::string> NodeSystem::GetNodeAreaIDList() const
{
	std::vector<std::string> Result;
	for (size_t i = 0; i < Areas.size(); i++)
		Result.push_back(Areas[i]->GetID());

	return Result;
}

std::vector<LinkNode*> NodeSystem::GetDanglingLinkNodes() const
{
	std::vector<LinkNode*> Result;
	for (NodeArea* Area : Areas)
	{
		std::vector<LinkNode*> LinkNodes = Area->GetNodesByType<LinkNode>();
		for (LinkNode* LinkNode : LinkNodes)
		{
			if (LinkNode->IsDangling())
				Result.push_back(LinkNode);
		}
	}

	return Result;
}

bool NodeSystem::TryToFixDanglingLinkNode(LinkNode* LinkNodeToFix, bool bForceRestorePartner)
{
	if (LinkNodeToFix == nullptr)
		return false;

	if (LinkNodeToFix->GetLinkedArea() == nullptr)
		return false;

	// Partner node does not exist.
	if (LinkNodeToFix->GetPartnerNode() == nullptr)
	{
		// if bForceRestorePartner is false we should not try to restore partner node.
		if (!bForceRestorePartner)
			return false;

		NodeArea* PartnerNodeArea = LinkNodeToFix->GetLinkedArea();
		if (LinkNodeToFix->GetParentArea() == PartnerNodeArea)
			return false;

		bool bIsInputNode = LinkNodeToFix->IsInputNode();
		LinkNode* PartnerNode = CreateLinkNodeInternal(!bIsInputNode);
		if (!PartnerNodeArea->AddNode(PartnerNode))
		{
			delete PartnerNode;
			return false;
		}

		// Recreate all sockets for partner node based on the current link node.
		std::vector<NodeSocket*>& SocketsToCopy = bIsInputNode ? LinkNodeToFix->Input : LinkNodeToFix->Output;
		std::vector<NodeSocket*>& PartnerNodeSockets = bIsInputNode ? PartnerNode->Output : PartnerNode->Input;
		for (size_t i = 0; i < SocketsToCopy.size(); i++)
		{
			if (i == 0)
				continue;

			PartnerNode->AddSocketInternal(SocketsToCopy[i]->GetAllowedTypes(), SocketsToCopy[i]->GetName(), SocketsToCopy[i]->GetFlowDirection());
		}

		LinkNodeToFix->PartnerNodeID = PartnerNode->GetID();

		PartnerNode->PartnerNodeID = LinkNodeToFix->GetID();
		PartnerNode->LinkedAreaID = LinkNodeToFix->GetParentArea()->GetID();

		LinkNode* InNode = LinkNodeToFix->IsInputNode() ? LinkNodeToFix : PartnerNode;
		LinkNode* OutNode = LinkNodeToFix->IsInputNode() ? PartnerNode : LinkNodeToFix;

		NodeAreaLinkRecord NewRecord;
		NewRecord.ID = InNode->GetID() + "_" + OutNode->GetID();
		NewRecord.InNodeID = InNode->GetID();
		NewRecord.OutNodeID = OutNode->GetID();
		NewRecord.InAreaID = InNode->GetParentArea()->GetID();
		NewRecord.OutAreaID = OutNode->GetParentArea()->GetID();

		NodeAreaLinkRecords[NewRecord.ID] = NewRecord;

		return !LinkNodeToFix->IsDangling();
	}

	if (NODE_SYSTEM.GetLinkDataByNodeID(LinkNodeToFix->GetID()) == nullptr)
	{
		LinkNode* InNode = LinkNodeToFix->IsInputNode() ? LinkNodeToFix : reinterpret_cast<LinkNode*>(LinkNodeToFix->GetPartnerNode());
		LinkNode* OutNode = LinkNodeToFix->IsInputNode() ? reinterpret_cast<LinkNode*>(LinkNodeToFix->GetPartnerNode()) : LinkNodeToFix;

		NodeAreaLinkRecord NewRecord;
		NewRecord.ID = InNode->GetID() + "_" + OutNode->GetID();
		NewRecord.InNodeID = InNode->GetID();
		NewRecord.OutNodeID = OutNode->GetID();
		NewRecord.InAreaID = OutNode->LinkedAreaID;
		NewRecord.OutAreaID = InNode->LinkedAreaID;
		NodeAreaLinkRecords[NewRecord.ID] = NewRecord;

		return true;
	}

	return false;
}

std::vector<LinkNode*> NodeSystem::TryToFixAllDanglingLinkNodes()
{
	std::vector<LinkNode*> Result;
	std::vector<LinkNode*> DanglingLinkNodes = GetDanglingLinkNodes();
	for (LinkNode* CurrentLinkNode : DanglingLinkNodes)
	{
		if (TryToFixDanglingLinkNode(CurrentLinkNode))
		{
			LinkNode* Partner = reinterpret_cast<LinkNode*>(CurrentLinkNode->GetPartnerNode());
			if (Partner != nullptr)
			{
				Result.push_back(CurrentLinkNode);
				Result.push_back(Partner);
			}
		}
	}

	return Result;
}

Node* NodeSystem::GetNodeByID(const std::string& NodeID) const
{
	for (NodeArea* Area : Areas)
	{
		Node* FoundNode = Area->GetNodeByID(NodeID);
		if (FoundNode != nullptr)
			return FoundNode;
	}

	return nullptr;
}

std::vector<Node*> NodeSystem::GetNodesByName(const std::string& Name) const
{
	std::vector<Node*> Result;
	for (NodeArea* Area : Areas)
	{
		std::vector<Node*> FoundNodes = Area->GetNodesByName(Name);
		Result.insert(Result.end(), FoundNodes.begin(), FoundNodes.end());
	}

	return Result;
}

std::vector<Node*> NodeSystem::GetNodesByStringType(const std::string& Type) const
{
	std::vector<Node*> Result;
	for (NodeArea* Area : Areas)
	{
		std::vector<Node*> FoundNodes = Area->GetNodesByStringType(Type);
		Result.insert(Result.end(), FoundNodes.begin(), FoundNodes.end());
	}

	return Result;
}

LinkNode* NodeSystem::CreateLinkNodeInternal(bool bIsInputNode)
{
	LinkNode* NewNode = new LinkNode();
	NewNode->bHaveInput = bIsInputNode;

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
	NewNode->AddSocketInternal({ "EXECUTE" });
#endif

	return NewNode;
}

SocketMirrorNode* NodeSystem::GetAppropriatePartner(SocketMirrorNode* MirrorNode, NodeSocket::SocketFlow CurrentDirection)
{
	if (MirrorNode == nullptr)
		return nullptr;

	bool bBiDirectionalMirror = MirrorNode->bHaveInput && MirrorNode->bHaveOutput;
	NodeSocket::SocketFlow PartnerSocketDirection = !CurrentDirection;

	std::vector<Node*> MirrorPartners = MirrorNode->GetMirrorPartners();
	for (Node* Partner : MirrorPartners)
	{
		if (Partner == nullptr)
			continue;

		SocketMirrorNode* PartnerMirrorNode = dynamic_cast<SocketMirrorNode*>(Partner);
		if (PartnerMirrorNode == nullptr)
			continue;

		bool bPartnerHasCurrentDirection = (PartnerMirrorNode->bHaveInput && PartnerSocketDirection == NodeSocket::SocketFlow::Input) ||
										   (PartnerMirrorNode->bHaveOutput && PartnerSocketDirection == NodeSocket::SocketFlow::Output);
		if (!bPartnerHasCurrentDirection)
			continue;

		return PartnerMirrorNode;
	}

	return nullptr;
}

std::pair<SocketMirrorNode*, NodeSocket*> NodeSystem::GetAppropriatePartnerAndSocket(SocketMirrorNode* MirrorNode, NodeSocket* CurrentNodeSocket)
{
	if (MirrorNode == nullptr || CurrentNodeSocket == nullptr)
		return std::make_pair(nullptr, nullptr);

	bool bBiDirectionalMirror = MirrorNode->bHaveInput && MirrorNode->bHaveOutput;
	NodeSocket::SocketFlow CurrentNodeSocketDirection = CurrentNodeSocket->GetFlowDirection();
	NodeSocket::SocketFlow PartnerSocketDirection = !CurrentNodeSocketDirection;
	size_t SocketIndex = MirrorNode->GetSocketIndexByID(CurrentNodeSocket->GetID());

	std::vector<Node*> MirrorPartners = MirrorNode->GetMirrorPartners();
	for (Node* Partner : MirrorPartners)
	{
		if (Partner == nullptr)
			continue;

		SocketMirrorNode* PartnerMirrorNode = dynamic_cast<SocketMirrorNode*>(Partner);
		if (PartnerMirrorNode == nullptr)
			continue;

		NodeSocket* PartnerNodeSocket = PartnerMirrorNode->GetSocketByIndex(SocketIndex, PartnerSocketDirection);
		if (PartnerNodeSocket == nullptr)
			continue;

		// Mirror nodes synchronize deletions with their partner.
		// The corresponding socket on the partner has the opposite direction,
		// so skip any socket whose direction is the same.
		if (PartnerNodeSocket->GetFlowDirection() == CurrentNodeSocketDirection)
			continue;

		return std::make_pair(PartnerMirrorNode, PartnerNodeSocket);
	}

	return std::make_pair(nullptr, nullptr);
}

bool NodeSystem::AddSocketToMirrorNode(const std::string& NodeID, std::vector<std::string> AllowedTypes, std::string Name, NodeSocket::SocketFlow SocketDirection)
{
	Node* CurrentNode = GetNodeByID(NodeID);
	if (CurrentNode == nullptr)
		return false;

	SocketMirrorNode* MirrorNode = dynamic_cast<SocketMirrorNode*>(CurrentNode);
	if (MirrorNode == nullptr)
		return false;

	if (MirrorNode->IsDangling())
		return false;

	bool bBiDirectionalMirror = MirrorNode->bHaveInput && MirrorNode->bHaveOutput;
	SocketMirrorNode* PartnerNode = GetAppropriatePartner(MirrorNode, SocketDirection);
	if (PartnerNode == nullptr)
		return false;

	if (PartnerNode->SocketIDBeingModified.empty())
	{
		NodeSocket* PartnerNodeSocket = new NodeSocket(PartnerNode, AllowedTypes, Name, !SocketDirection);
		if (!PartnerNode->AddSocket(PartnerNodeSocket))
		{
			delete PartnerNodeSocket;
			return false;
		}
	}

	return true;
}

bool NodeSystem::DeleteSocket(const std::string& NodeID, std::string SocketID)
{
	Node* Node = GetNodeByID(NodeID);
	if (Node == nullptr)
		return false;

	NodeSocket* SocketToDelete = Node->GetSocketByID(SocketID);
	if (SocketToDelete == nullptr)
		return false;

	NodeArea* ParentArea = Node->GetParentArea();
	if (ParentArea != nullptr)
		ParentArea->TryToDisconnect(Node, SocketID);
	
	std::vector<NodeSocket*>& SocketList = SocketToDelete->GetFlowDirection() == NodeSocket::SocketFlow::Output ? Node->Output : Node->Input;
	for (size_t i = 0; i < SocketList.size(); i++)
	{
		if (SocketList[i]->GetID() == SocketID)
		{
			delete SocketList[i];
			SocketList.erase(SocketList.begin() + i, SocketList.begin() + i + 1);

			return true;
		}
	}

	return false;
}

bool NodeSystem::DeleteSocket(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return false;

	Node* ParentNode = Socket->GetParent();
	if (ParentNode == nullptr)
	{
		delete Socket;
		return true;
	}
	else
	{
		return DeleteSocket(ParentNode->GetID(), Socket->GetID());
	}
}

bool NodeSystem::DeleteSocketFromMirrorNode(const std::string& NodeID, std::string SocketID)
{
	Node* CurrentNode = GetNodeByID(NodeID);
	if (CurrentNode == nullptr)
		return false;

	SocketMirrorNode* MirrorNode = dynamic_cast<SocketMirrorNode*>(CurrentNode);
	if (MirrorNode == nullptr)
		return false;

	if (MirrorNode->IsDangling())
		return false;

	NodeSocket* CurrentSocket = MirrorNode->GetSocketByID(SocketID);
	size_t SocketIndex = MirrorNode->GetSocketIndexByID(SocketID);
	if (SocketIndex == 0)
		return false; // We should never delete execution socket.

	std::pair<SocketMirrorNode*, NodeSocket*> PartnerData = GetAppropriatePartnerAndSocket(MirrorNode, CurrentSocket);
	if (PartnerData.first == nullptr || PartnerData.second == nullptr)
		return false;

	if (PartnerData.first->SocketIDBeingModified != PartnerData.second->GetID())
		PartnerData.first->DeleteSocket(PartnerData.second->GetID());

	return true;
}

bool NodeSystem::RevalidateSocketConnections(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return false;

	Node* ParentNode = Socket->GetParent();
	if (ParentNode == nullptr)
		return false;

	NodeArea* ParentArea = ParentNode->GetParentArea();
	if (ParentArea == nullptr)
		return false;

	bool bAnyDisconnected = false;
	std::vector<NodeSocket*> ConnectedSockets = Socket->GetConnectedSockets();
	for (size_t i = 0; i < ConnectedSockets.size(); i++)
	{
		NodeSocket* OtherSocket = ConnectedSockets[i];
		Node* OtherNode = OtherSocket->GetParent();
		if (OtherNode == nullptr)
			continue;

		// Use the nodes IsValidAsNewConnection to check type compatibility, which already handles the allowed-type intersection logic.
		if (ParentNode->IsValidAsNewConnection(Socket, OtherSocket))
			continue;

		// Connection is no longer valid, disconnect it.
		if (Socket->GetFlowDirection() == NodeSocket::SocketFlow::Output)
			ParentArea->TryToDisconnect(ParentNode, Socket->GetID(), OtherNode, OtherSocket->GetID());
		else
			ParentArea->TryToDisconnect(OtherNode, OtherSocket->GetID(), ParentNode, Socket->GetID());

		bAnyDisconnected = true;
	}

	return bAnyDisconnected;
}

bool NodeSystem::SyncMirrorNodeSocketAllowedTypes(const std::string& NodeID, std::string SocketID, std::vector<std::string> NewTypes)
{
	Node* CurrentNode = GetNodeByID(NodeID);
	if (CurrentNode == nullptr)
		return false;

	SocketMirrorNode* MirrorNode = dynamic_cast<SocketMirrorNode*>(CurrentNode);
	if (MirrorNode == nullptr)
		return false;

	if (MirrorNode->IsDangling())
		return false;

	NodeSocket* CurrentSocket = CurrentNode->GetSocketByID(SocketID);
	size_t SocketIndex = CurrentNode->GetSocketIndexByID(SocketID);
	if (SocketIndex == 0)
		return false; // We should never change execution socket types.

	std::pair<SocketMirrorNode*, NodeSocket*> PartnerData = GetAppropriatePartnerAndSocket(MirrorNode, CurrentSocket);
	if (PartnerData.first == nullptr || PartnerData.second == nullptr)
		return false;

	if (PartnerData.second->GetAllowedTypes() != NewTypes)
		PartnerData.second->SetAllowedTypes(NewTypes);

	return true;
}

void NodeSystem::SyncMirrorNodeSocketName(const std::string& NodeID, std::string SocketID, std::string NewName)
{
	Node* CurrentNode = GetNodeByID(NodeID);
	if (CurrentNode == nullptr)
		return;

	SocketMirrorNode* MirrorNode = dynamic_cast<SocketMirrorNode*>(CurrentNode);
	if (MirrorNode == nullptr)
		return;

	NodeSocket* CurrentSocket = CurrentNode->GetSocketByID(SocketID);
	size_t SocketIndex = CurrentNode->GetSocketIndexByID(SocketID);

	std::pair<SocketMirrorNode*, NodeSocket*> PartnerData = GetAppropriatePartnerAndSocket(MirrorNode, CurrentSocket);
	if (PartnerData.first == nullptr || PartnerData.second == nullptr)
		return;

	if (PartnerData.second->GetName() != NewName)
		PartnerData.second->SetName(NewName);
}

SubAreaNode* NodeSystem::CreateSubAreaNode(const std::string& ParentAreaID)
{
	NodeArea* ParentArea = GetNodeAreaByID(ParentAreaID);
	if (ParentArea == nullptr)
		return nullptr;

	NodeArea* OwnedArea = CreateNodeArea();
	SubAreaNode* Result = new SubAreaNode(OwnedArea);
#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
	Result->AddSocketInternal({ "EXECUTE" }, "", NodeSocket::SocketFlow::Input);
	Result->AddSocketInternal({ "EXECUTE" }, "", NodeSocket::SocketFlow::Output);
#endif
	ParentArea->AddNode(Result);

	float DistanceFromInOutNodes = 100.0f;

	SubAreaInputNode* InputNode = new SubAreaInputNode();
#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
	InputNode->AddSocketInternal({ "EXECUTE" }, "", NodeSocket::SocketFlow::Input);
#endif
	InputNode->OwnerSubAreaNodeID = Result->GetID();
	InputNode->SetPosition(ImVec2(-DistanceFromInOutNodes / 2.0f - InputNode->GetSize().x / 2.0f, 0.0f));
	OwnedArea->AddNode(InputNode);
	Result->SubAreaInputNodeID = InputNode->GetID();

	SubAreaOutputNode* OutputNode = new SubAreaOutputNode();
#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
	OutputNode->AddSocketInternal({ "EXECUTE" }, "", NodeSocket::SocketFlow::Output);
#endif
	OutputNode->OwnerSubAreaNodeID = Result->GetID();
	OutputNode->SetPosition(ImVec2(DistanceFromInOutNodes / 2.0f + OutputNode->GetSize().x / 2.0f, 0.0f));
	OwnedArea->AddNode(OutputNode);
	Result->SubAreaOutputNodeID = OutputNode->GetID();

	return Result;
}

void NodeSystem::BreakSubAreaOwnershipCycles()
{
	std::unordered_set<std::string> FullyVisited;
	std::unordered_set<std::string> OnCurrentPath;
	std::vector<SubAreaNode*> NodesToBreak;

	std::function<void(const std::string&)> DepthFirstSearch = [&](const std::string& AreaID) {
		if (FullyVisited.count(AreaID))
			return;
		// Should not happen when called from the top of the loop below, but guards against re-entry through a malformed graph.
		if (OnCurrentPath.count(AreaID))
			return;

		OnCurrentPath.insert(AreaID);

		NodeArea* Area = GetNodeAreaByID(AreaID);
		if (Area != nullptr)
		{
			// Snapshot the SubAreaNode list, we may push entries into NodesToBreak, but we do not mutate the area's node vector during the walk.
			std::vector<SubAreaNode*> SubAreaNodesInArea = Area->GetNodesByType<SubAreaNode>();
			for (SubAreaNode* CurrentSubAreaNode : SubAreaNodesInArea)
			{
				const std::string& OwnedID = CurrentSubAreaNode->OwnedAreaID;
				if (OwnedID.empty())
					continue;

				// A SubAreaNode pointing at a non-existent area is dangling, not a cycle, do nothing.
				if (GetNodeAreaByID(OwnedID) == nullptr)
					continue;

				if (OnCurrentPath.count(OwnedID))
				{
					// This SubAreaNode closes a cycle. Mark it for removal, do not descend further along this edge.
					NodesToBreak.push_back(CurrentSubAreaNode);
					continue;
				}

				DepthFirstSearch(OwnedID);
			}
		}

		OnCurrentPath.erase(AreaID);
		FullyVisited.insert(AreaID);
	};

	// Snapshot the area list as well, since deletes below mutate Areas.
	std::vector<std::string> AreaIDsToVisit;
	AreaIDsToVisit.reserve(Areas.size());
	for (NodeArea* Area : Areas)
		AreaIDsToVisit.push_back(Area->GetID());

	for (const std::string& AreaID : AreaIDsToVisit)
		DepthFirstSearch(AreaID);

	for (SubAreaNode* CurrentSubAreaNode : NodesToBreak)
	{
		// Clear OwnedAreaID first.
		CurrentSubAreaNode->OwnedAreaID = "";

		NodeArea* Parent = CurrentSubAreaNode->GetParentArea();
		if (Parent != nullptr)
		{
			CurrentSubAreaNode->bCouldBeDestroyedByUser = true;
			Parent->Delete(CurrentSubAreaNode);
		}
	}
}

SubAreaNode* NodeSystem::FindOwnerSubAreaNode(const std::string& AreaID) const
{
	for (size_t i = 0; i < Areas.size(); i++)
	{
		for (SubAreaNode* CurrentSubAreaNode : Areas[i]->GetNodesByType<SubAreaNode>())
		{
			if (CurrentSubAreaNode->GetOwnedArea() == nullptr)
				continue;

			if (CurrentSubAreaNode->GetOwnedArea()->GetID() == AreaID)
				return CurrentSubAreaNode;
		}
	}

	return nullptr;
}

std::vector<std::string> NodeSystem::DeduplicateIDList(const std::vector<std::string>& ListOfIDs) const
{
	std::unordered_set<std::string> SeenIDs(ListOfIDs.begin(), ListOfIDs.end());
	return std::vector<std::string>(SeenIDs.begin(), SeenIDs.end());
}