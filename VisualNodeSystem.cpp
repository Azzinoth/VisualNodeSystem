#include "VisualNodeSystem.h"
using namespace VisNodeSys;

#ifdef VISUAL_NODE_SYSTEM_SHARED
extern "C" __declspec(dllexport) void* GetNodeSystem()
{
	return NodeSystem::GetInstancePointer();
}
#endif

NodeSystem::NodeSystem() {}
NodeSystem::~NodeSystem() {}

void NodeSystem::Initialize(bool bTestMode)
{
	NODE_CORE.bIsInTestMode = bTestMode;
	NODE_CORE.InitializeFonts();
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
	auto iterator = NodeSocket::SocketTypeToColorAssosiations.begin();
	while (iterator != NodeSocket::SocketTypeToColorAssosiations.end())
	{
		Result.push_back(std::make_pair(iterator->first, iterator->second));
		iterator++;
	}

	return Result;
}

void NodeSystem::AssociateSocketTypeToColor(std::string SocketType, ImColor Color)
{
	NodeSocket::SocketTypeToColorAssosiations[SocketType] = Color;
}