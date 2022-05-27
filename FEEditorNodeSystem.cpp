#include "FEEditorNodeSystem.h"

FEEditorNodeSystem* FEEditorNodeSystem::_instance = nullptr;

FEEditorNodeSystem::FEEditorNodeSystem() {}
FEEditorNodeSystem::~FEEditorNodeSystem() {}

FEEditorNodeArea* FEEditorNodeSystem::createNodeArea()
{
	createdAreas.push_back(new FEEditorNodeArea());
	return createdAreas.back();
}

void FEEditorNodeSystem::deleteNodeArea(FEEditorNodeArea* nodeArea)
{
	for (size_t i = 0; i < createdAreas.size(); i++)
	{
		if (createdAreas[i] == nodeArea)
		{
			delete createdAreas[i];
			createdAreas.erase(createdAreas.begin() + i, createdAreas.begin() + i + 1);
			return;
		}
	}
}

void FEEditorNodeSystem::moveNodesTo(FEEditorNodeArea* sourceNodeArea, FEEditorNodeArea* targetNodeArea, bool selectMovedNodes)
{
	if (sourceNodeArea == nullptr || targetNodeArea == nullptr)
		return;

	for (size_t i = 0; i < sourceNodeArea->nodes.size(); i++)
	{
		//targetNodeArea->nodes.push_back(sourceNodeArea->nodes[i]);
		targetNodeArea->addNode(sourceNodeArea->nodes[i]);
	}
	size_t sourceNodeCount = sourceNodeArea->nodes.size();
	sourceNodeArea->nodes.clear();

	for (size_t i = 0; i < sourceNodeArea->connections.size(); i++)
	{
		targetNodeArea->connections.push_back(sourceNodeArea->connections[i]);
	}
	sourceNodeArea->connections.clear();
	sourceNodeArea->clear();

	// Select moved nodes.
	if (selectMovedNodes)
	{
		targetNodeArea->selected.clear();
		for (size_t i = targetNodeArea->nodes.size() - sourceNodeCount; i < targetNodeArea->nodes.size(); i++)
		{
			targetNodeArea->selected.push_back(targetNodeArea->nodes[i]);
		}
	}
}

//FEEditorNodeArea* FEEditorNodeSystem::createNodeArea(std::vector<FEEditorNode*> nodes)
//{
//	createdAreas.push_back(new FEEditorNodeArea());
//
//	// Copy all nodes to new node area.
//	std::unordered_map<FEEditorNode*, FEEditorNode*> oldToNewNode;
//	std::unordered_map<FEEditorNodeSocket*, FEEditorNodeSocket*> oldToNewSocket;
//	for (size_t i = 0; i < nodes.size(); i++)
//	{
//		FEEditorNode* copyOfNode = FEEditorNode::copyChild(nodes[i]->getType(), nodes[i]);
//		if (copyOfNode == nullptr)
//			copyOfNode = new FEEditorNode(*nodes[i]);
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
//						createdAreas.back()->connections.push_back(new FEEditorNodeConnection(oldToNewSocket[nodes[i]->input[j]->connections[k]], oldToNewSocket[nodes[i]->input[j]]));
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
//						createdAreas.back()->connections.push_back(new FEEditorNodeConnection(oldToNewSocket[nodes[i]->output[j]], oldToNewSocket[nodes[i]->output[j]->connections[k]]));
//				}
//			}
//		}
//	}
//
//	return createdAreas.back();
//}