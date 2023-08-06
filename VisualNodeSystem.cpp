#include "VisualNodeSystem.h"

VisualNodeSystem* VisualNodeSystem::Instance = nullptr;

VisualNodeSystem::VisualNodeSystem() {}
VisualNodeSystem::~VisualNodeSystem() {}

VisualNodeArea* VisualNodeSystem::CreateNodeArea()
{
	CreatedAreas.push_back(new VisualNodeArea());
	return CreatedAreas.back();
}

void VisualNodeSystem::DeleteNodeArea(const VisualNodeArea* NodeArea)
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

void VisualNodeSystem::MoveNodesTo(VisualNodeArea* SourceNodeArea, VisualNodeArea* TargetNodeArea, const bool SelectMovedNodes)
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

std::vector<std::pair<std::string, ImColor>> VisualNodeSystem::GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color)
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

void VisualNodeSystem::AssociateSocketTypeToColor(std::string SocketType, ImColor Color)
{
	NodeSocket::SocketTypeToColorAssosiations[SocketType] = Color;
}