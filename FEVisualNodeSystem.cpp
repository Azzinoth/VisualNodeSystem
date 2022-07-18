#include "FEVisualNodeSystem.h"

FEVisualNodeSystem* FEVisualNodeSystem::Instance = nullptr;

FEVisualNodeSystem::FEVisualNodeSystem() {}
FEVisualNodeSystem::~FEVisualNodeSystem() {}

FEVisualNodeArea* FEVisualNodeSystem::CreateNodeArea()
{
	CreatedAreas.push_back(new FEVisualNodeArea());
	return CreatedAreas.back();
}

void FEVisualNodeSystem::DeleteNodeArea(const FEVisualNodeArea* NodeArea)
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

void FEVisualNodeSystem::MoveNodesTo(FEVisualNodeArea* SourceNodeArea, FEVisualNodeArea* TargetNodeArea, const bool SelectMovedNodes)
{
	if (SourceNodeArea == nullptr || TargetNodeArea == nullptr)
		return;

	for (size_t i = 0; i < SourceNodeArea->Nodes.size(); i++)
	{
		//targetNodeArea->nodes.push_back(sourceNodeArea->nodes[i]);
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
		TargetNodeArea->Selected.clear();
		for (size_t i = TargetNodeArea->Nodes.size() - SourceNodeCount; i < TargetNodeArea->Nodes.size(); i++)
		{
			TargetNodeArea->Selected.push_back(TargetNodeArea->Nodes[i]);
		}
	}
}

//FEVisualNodeArea* FEVisualNodeSystem::createNodeArea(std::vector<FEVisualNode*> nodes)
//{
//	createdAreas.push_back(new FEVisualNodeArea());
//
//	// Copy all nodes to new node area.
//	std::unordered_map<FEVisualNode*, FEVisualNode*> oldToNewNode;
//	std::unordered_map<FEVisualNodeSocket*, FEVisualNodeSocket*> oldToNewSocket;
//	for (size_t i = 0; i < nodes.size(); i++)
//	{
//		FEVisualNode* copyOfNode = FEVisualNode::copyChild(nodes[i]->getType(), nodes[i]);
//		if (copyOfNode == nullptr)
//			copyOfNode = new FEVisualNode(*nodes[i]);
//		copyOfNode->parentArea = createdAreas.back();
//
//		createdAreas.back()->nodes.push_back(copyOfNode);
//
//		// Associate old to new IDs
//		oldToNewNode[nodes[i]] = copyOfNode;
//
//		for (size_t j = 0; j < nodes[i]->input.size(); j++)
//		{
//			oldToNewSocket[nodes[i]->input[j]] = copyOfNode->input[j];
//		}
//
//		for (size_t j = 0; j < nodes[i]->output.size(); j++)
//		{
//			oldToNewSocket[nodes[i]->output[j]] = copyOfNode->output[j];
//		}
//	}
//
//	// Than we need to recreate all connections.
//	for (size_t i = 0; i < nodes.size(); i++)
//	{
//		for (size_t j = 0; j < nodes[i]->input.size(); j++)
//		{
//			for (size_t k = 0; k < nodes[i]->input[j]->connections.size(); k++)
//			{
//				// if node is connected to node that is not in this list just ignore.
//				if (isNodeIDInList(nodes[i]->input[j]->connections[k]->getParent()->getID(), nodes))
//				{
//					createdAreas.back()->nodes[i]->input[j]->connections.push_back(oldToNewSocket[nodes[i]->input[j]->connections[k]]);
//
//					// Add connection to node area.
//					// Maybe we already establish this connecton.
//					bool shouldAdd = true;
//					for (size_t l = 0; l < createdAreas.back()->connections.size(); l++)
//					{
//						if (createdAreas.back()->connections[l]->in == oldToNewSocket[nodes[i]->input[j]] &&
//							createdAreas.back()->connections[l]->out == oldToNewSocket[nodes[i]->input[j]->connections[k]])
//						{
//							shouldAdd = false;
//							break;
//						}
//					}
//
//					if (shouldAdd)
//						createdAreas.back()->connections.push_back(new FEVisualNodeConnection(oldToNewSocket[nodes[i]->input[j]->connections[k]], oldToNewSocket[nodes[i]->input[j]]));
//				}
//			}
//		}
//
//		for (size_t j = 0; j < nodes[i]->output.size(); j++)
//		{
//			for (size_t k = 0; k < nodes[i]->output[j]->connections.size(); k++)
//			{
//				// if node is connected to node that is not in this list just ignore.
//				if (isNodeIDInList(nodes[i]->output[j]->connections[k]->getParent()->getID(), nodes))
//				{
//					createdAreas.back()->nodes[i]->output[j]->connections.push_back(oldToNewSocket[nodes[i]->output[j]->connections[k]]);
//
//					// Add connection to node area.
//					// Maybe we already establish this connecton.
//					bool shouldAdd = true;
//					for (size_t l = 0; l < createdAreas.back()->connections.size(); l++)
//					{
//						if (createdAreas.back()->connections[l]->in == oldToNewSocket[nodes[i]->output[j]->connections[k]] &&
//							createdAreas.back()->connections[l]->out == oldToNewSocket[nodes[i]->output[j]])
//						{
//							shouldAdd = false;
//							break;
//						}
//					}
//
//					if (shouldAdd)
//						createdAreas.back()->connections.push_back(new FEVisualNodeConnection(oldToNewSocket[nodes[i]->output[j]], oldToNewSocket[nodes[i]->output[j]->connections[k]]));
//				}
//			}
//		}
//	}
//
//	return createdAreas.back();
//}