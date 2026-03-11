#include "VisualNodeSystem.h"
using namespace VisNodeSys;

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
	CreatedAreas.push_back(new NodeArea());
	return CreatedAreas.back();
}

NodeArea* NodeSystem::GetNodeAreaByID(const std::string& NodeAreaID) const
{
	for (size_t i = 0; i < CreatedAreas.size(); i++)
	{
		if (CreatedAreas[i]->GetID() == NodeAreaID)
			return CreatedAreas[i];
	}

	return nullptr;
}

void NodeSystem::DeleteNodeArea(const NodeArea* NodeArea)
{
	for (size_t i = 0; i < CreatedAreas.size(); i++)
	{
		if (CreatedAreas[i] == NodeArea)
		{
			std::vector<ReferenceNodeData> RelatedReferenceNodes = GetReferenceNodeDataByReferencedAreaID(NodeArea->GetID());
			for (size_t j = 0; j < RelatedReferenceNodes.size(); j++)
				DeleteReferenceNodeRecord(RelatedReferenceNodes[j].NodeID);
			
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

NodeSystem::ReferenceNodeData NodeSystem::GetReferenceNodeDataByNodeID(const std::string& NodeID) const
{
	auto RecordIterator = ReferenceNodeRecords.find(NodeID);
	if (RecordIterator != ReferenceNodeRecords.end())
		return RecordIterator->second;
	
	return ReferenceNodeData();
}

std::vector<NodeSystem::ReferenceNodeData> NodeSystem::GetReferenceNodeDataByReferencedAreaID(const std::string& ReferencedAreaID) const
{
	auto RecordIterator = ReferenceNodeRecords.begin();
	std::vector<ReferenceNodeData> Result;
	while (RecordIterator != ReferenceNodeRecords.end())
	{
		if (RecordIterator->second.ReferencedAreaID == ReferencedAreaID)
			Result.push_back(RecordIterator->second);
		
		RecordIterator++;
	}
	
	return Result;
}

std::vector<NodeSystem::ReferenceNodeData> NodeSystem::GetReferenceNodeDataByParentAreaID(const std::string& ParentAreaID) const
{
	auto RecordIterator = ReferenceNodeRecords.begin();
	std::vector<ReferenceNodeData> Result;
	while (RecordIterator != ReferenceNodeRecords.end())
	{
		if (RecordIterator->second.ParentNodeAreaID == ParentAreaID)
			Result.push_back(RecordIterator->second);
		
		RecordIterator++;
	}
	
	return Result;
}

bool NodeSystem::CreateReferenceNodeRecord(const std::string& NodeID, const std::string& ParentNodeAreaID, const std::string& ReferencedAreaID)
{
	if (NodeID.empty() || ReferencedAreaID.empty())
		return false;

	ReferenceNodeData NewRecord;
	NewRecord.NodeID = NodeID;
	NewRecord.ParentNodeAreaID = ParentNodeAreaID;
	NewRecord.ReferencedAreaID = ReferencedAreaID;

	ReferenceNodeRecords[NodeID] = NewRecord;
	return true;
}

bool NodeSystem::UpdateReferenceNodeRecord(const std::string& NodeID, const std::string& ParentNodeAreaID, const std::string& ReferencedAreaID)
{
	if (NodeID.empty())
		return false;

	auto RecordIterator = ReferenceNodeRecords.find(NodeID);
	if (RecordIterator == ReferenceNodeRecords.end())
		return false;
		
	RecordIterator->second.ParentNodeAreaID = ParentNodeAreaID;
	RecordIterator->second.ReferencedAreaID = ReferencedAreaID;
	return true;
}

bool NodeSystem::DeleteReferenceNodeRecord(const std::string& NodeID)
{
	auto RecordIterator = ReferenceNodeRecords.find(NodeID);
	if (RecordIterator == ReferenceNodeRecords.end())
		return false;

	ReferenceNodeRecords.erase(RecordIterator);
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

void NodeSystem::OnNodeAreaFocusChanging(NodeArea* CurrentNodeArea, bool NewFocusValue)
{
	if (CurrentNodeArea == nullptr)
		return;

	if (NewFocusValue)
	{
		for (size_t i = 0; i < CreatedAreas.size(); i++)
		{
			if (CreatedAreas[i] != CurrentNodeArea)
				CreatedAreas[i]->SetFocusedInternal(false);
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

	// First of all we need to save the list of all node areas.
	Json::Value NodeAreaList;
	for (size_t i = 0; i < CreatedAreas.size(); i++)
		NodeAreaList[std::to_string(i)] = CreatedAreas[i]->GetID();
	
	Root["NodeAreaList"] = NodeAreaList;

	// Reference node records.
	Json::Value ReferenceNodeRecordsJson;
	auto RecordIterator = ReferenceNodeRecords.begin();
	while (RecordIterator != ReferenceNodeRecords.end())
	{
		Json::Value RecordJson;
		RecordJson["NodeID"] = RecordIterator->second.NodeID;
		RecordJson["ParentNodeAreaID"] = RecordIterator->second.ParentNodeAreaID;
		RecordJson["ReferencedAreaID"] = RecordIterator->second.ReferencedAreaID;
		ReferenceNodeRecordsJson[RecordIterator->first] = RecordJson;
		RecordIterator++;
	}
	Root["ReferenceNodeRecords"] = ReferenceNodeRecordsJson;

	// Socket type to color associations.
	Json::Value SocketTypeToColorAssociationsJson;
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
	Json::Value NodeAreasJson;
	for (size_t i = 0; i < CreatedAreas.size(); i++)
		NodeAreasJson[CreatedAreas[i]->GetID()] = CreatedAreas[i]->ToJson();
	Root["NodeAreas"] = NodeAreasJson;

	Json::StreamWriterBuilder Builder;
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
	if (JsonText.find("{") == std::string::npos || JsonText.find("}") == std::string::npos || JsonText.find(":") == std::string::npos)
		return false;

	Json::Value Root;
	JSONCPP_STRING Error;
	Json::CharReaderBuilder Builder;

	const std::unique_ptr<Json::CharReader> Reader(Builder.newCharReader());
	if (!Reader->parse(JsonText.c_str(), JsonText.c_str() + JsonText.size(), &Root, &Error))
		return false;

	if (!Root.isMember("NodeAreaList") || !Root.isMember("ReferenceNodeRecords") || !Root.isMember("SocketTypeToColorAssociations") || !Root.isMember("NodeAreas") || !Root["NodeAreas"].isObject())
		return false;

	//// First of all we need to load all node areas.
	//std::vector<std::string> NodeAreaIDs;
	//Json::Value NodeAreaList = Root["NodeAreaList"];
	//for (Json::Value::ArrayIndex i = 0; i < NodeAreaList.size(); i++)
	//{
	//	std::string NodeAreaID = NodeAreaList[std::to_string(i)].asString();
	//	NodeAreaIDs.push_back(NodeAreaID);
	//}

	// Reference node records.
	std::vector<Json::String> ReferenceNodeList = Root["ReferenceNodeRecords"].getMemberNames();
	std::vector<Json::String> ValidConnectionKeys;
	for (size_t i = 0; i < ReferenceNodeList.size(); i++)
	{
		if (!Root["ReferenceNodeRecords"][ReferenceNodeList[i]].isObject())
			continue;

		const Json::Value& ReferenceNodeData = Root["ReferenceNodeRecords"][ReferenceNodeList[i]];
		if (!ReferenceNodeData.isMember("NodeID") || !ReferenceNodeData.isMember("ParentNodeAreaID") || !ReferenceNodeData.isMember("ReferencedAreaID"))
			continue;

		std::string NodeID = ReferenceNodeData["NodeID"].asString();
		std::string ParentNodeAreaID = ReferenceNodeData["ParentNodeAreaID"].asString();
		std::string ReferencedAreaID = ReferenceNodeData["ReferencedAreaID"].asString();
		if (NodeID.empty() || ParentNodeAreaID.empty() || ReferencedAreaID.empty())
			continue;
	}

	// Socket type to color associations.
	std::vector<Json::String> SocketTypeToColorAssociationList = Root["SocketTypeToColorAssociations"].getMemberNames();
	for (size_t i = 0; i < SocketTypeToColorAssociationList.size(); i++)
	{
		if (!Root["SocketTypeToColorAssociations"][SocketTypeToColorAssociationList[i]].isObject())
			continue;

		const Json::Value& ColorJson = Root["SocketTypeToColorAssociations"][SocketTypeToColorAssociationList[i]];
		if (!ColorJson.isMember("R") || !ColorJson.isMember("G") || !ColorJson.isMember("B") || !ColorJson.isMember("A"))
			continue;

		std::string SocketType = SocketTypeToColorAssociationList[i];
		float R = ColorJson["R"].asFloat();
		float G = ColorJson["G"].asFloat();
		float B = ColorJson["B"].asFloat();
		float A = ColorJson["A"].asFloat();
		ImColor Color(R, G, B, A);
		AssociateSocketTypeToColor(SocketType, Color);
	}

	//const Json::Value& NodeAreasJson = Root["NodeAreas"];
	//// The keys are area IDs (strings), so iterate over all members
	//std::vector<std::string> AreaIDs = NodeAreasJson.getMemberNames();
	//for (const auto& AreaID : AreaIDs)
	//{
	//	const Json::Value& AreaJson = NodeAreasJson[AreaID];
	//	NodeArea* NewNodeArea = CreateNodeArea();
	//	NewNodeArea->LoadFromJson(AreaJson.asString());
	//}

	std::vector<Json::String> NodeAreaListKeys = Root["NodeAreas"].getMemberNames();
	for (size_t i = 0; i < NodeAreaListKeys.size(); i++)
	{
		NodeArea* NewNodeArea = CreateNodeArea();
		NewNodeArea->LoadFromJson(Root["NodeAreas"][NodeAreaListKeys[i]].asCString());
	}
	

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

std::vector<NodeArea*> NodeSystem::GetDirectlyReferencedAreas(const NodeArea* CurrentNodeArea) const
{
	std::vector<NodeArea*> Result;
	if (CurrentNodeArea == nullptr)
		return Result;

	std::vector<ReferenceNodeData> ReferenceNodes = GetReferenceNodeDataByParentAreaID(CurrentNodeArea->GetID());
	for (int i = 0; i < ReferenceNodes.size(); i++)
	{
		NodeArea* ReferencedArea = GetNodeAreaByID(ReferenceNodes[i].ReferencedAreaID);
		if (ReferencedArea != nullptr)
			Result.push_back(ReferencedArea);
	}

	return Result;
}

std::vector<NodeArea*> NodeSystem::GetAllReferencedAreasRecursive(const NodeArea* CurrentNodeArea) const
{
	std::vector<NodeArea*> Result;
	if (CurrentNodeArea == nullptr)
		return Result;

	// Using Visited map to avoid cycles.
	std::unordered_map<std::string, bool> Visited;
	std::function<void(const NodeArea*)> Collect = [&](const NodeArea* CurrentArea) {
		if (CurrentArea == nullptr)
			return;

		for (NodeArea* ReferencedArea : GetDirectlyReferencedAreas(CurrentArea))
		{
			if (Visited.find(ReferencedArea->GetID()) == Visited.end())
			{
				Visited[ReferencedArea->GetID()] = true;
				Result.push_back(ReferencedArea);
				Collect(ReferencedArea);
			}
		}
	};

	Collect(CurrentNodeArea);
	return Result;
}

std::vector<NodeArea*> NodeSystem::GetReferencingAreas(const NodeArea* CurrentNodeArea) const
{
	std::vector<NodeArea*> Result;
	if (CurrentNodeArea == nullptr)
		return Result;

	std::vector<ReferenceNodeData> ReferenceNodes = GetReferenceNodeDataByReferencedAreaID(CurrentNodeArea->GetID());
	for (int i = 0; i < ReferenceNodes.size(); i++)
	{
		NodeArea* ParentArea = GetNodeAreaByID(ReferenceNodes[i].ParentNodeAreaID);
		if (ParentArea != nullptr)
			Result.push_back(ParentArea);
	}

	return Result;
}

std::vector<NodeArea*> NodeSystem::GetAllReferencingAreasRecursive(const NodeArea* CurrentNodeArea) const
{
	std::vector<NodeArea*> Result;
	if (CurrentNodeArea == nullptr)
		return Result;

	// Using Visited map to avoid cycles.
	std::unordered_map<std::string, bool> Visited;
	std::function<void(const NodeArea*)> Collect = [&](const NodeArea* CurrentArea) {
		if (CurrentArea == nullptr)
			return;

		for (NodeArea* ReferencingArea : GetReferencingAreas(CurrentArea))
		{
			if (Visited.find(ReferencingArea->GetID()) == Visited.end())
			{
				Visited[ReferencingArea->GetID()] = true;
				Result.push_back(ReferencingArea);
				Collect(ReferencingArea);
			}
		}
	};

	Collect(CurrentNodeArea);
	return Result;
}

std::vector<std::string> NodeSystem::GetNodeAreaIDList() const
{
	std::vector<std::string> Result;
	for (size_t i = 0; i < CreatedAreas.size(); i++)
		Result.push_back(CreatedAreas[i]->GetID());

	return Result;
}