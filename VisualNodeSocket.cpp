#include "VisualNodeSocket.h"
#include "VisualNodeSystem.h"
using namespace VisNodeSys;

std::unordered_map<std::string, ImColor> NodeSocket::SocketTypeToColorAssociations;

NodeSocket::NodeSocket(Node* Parent, const std::string Type, const std::string Name, SocketFlow Flow, std::function<void* ()> OutputDataFunction)
{
	this->Parent = Parent;
	this->AllowedTypes.push_back(Type);
	this->Name = Name;
	this->ID = NODE_CORE.GetUniqueHexID();
	this->Flow = Flow;
	this->OutputData = OutputDataFunction;
}

NodeSocket::NodeSocket(Node* Parent, const std::vector<std::string> Types, const std::string Name, SocketFlow Flow, std::function<void* ()> OutputDataFunction)
{
	this->Parent = Parent;
	this->AllowedTypes = Types;
	this->Name = Name;
	this->ID = NODE_CORE.GetUniqueHexID();
	this->Flow = Flow;
	this->OutputData = OutputDataFunction;
}

std::string NodeSocket::GetID() const
{
	return ID;
}

Node* NodeSocket::GetParent() const
{
	return Parent;
}

const std::vector<NodeSocket*>& NodeSocket::GetConnectedSockets() const
{
	return ConnectedSockets;
}

std::string NodeSocket::GetName() const
{
	return Name;
}

void NodeSocket::SetName(std::string NewValue)
{
	Name = NewValue;
	SocketMirrorNode* MirrorParent = dynamic_cast<SocketMirrorNode*>(GetParent());
	if (MirrorParent != nullptr)
		NODE_SYSTEM.SyncSocketName(GetParent()->GetID(), GetID(), NewValue);
}

const std::vector<std::string>& NodeSocket::GetAllowedTypes() const
{
	return AllowedTypes;
}

NodeSocket::SocketFlow NodeSocket::GetFlowDirection() const
{
	return Flow;
}

//bool NodeSocket::IsOutput() const
//{ 
//	return bOutput;
//}
//
//bool NodeSocket::IsInput() const
//{ 
//	return !bOutput;
//}

void NodeSocket::SetFunctionToOutputData(std::function<void* ()> NewFunction)
{
	OutputData = NewFunction;
}

void* NodeSocket::GetData()
{
	return OutputData();
}

bool NodeSocket::SetAllowedTypes(std::vector<std::string> NewTypes)
{
	AllowedTypes = NewTypes;
	SocketMirrorNode* MirrorParent = dynamic_cast<SocketMirrorNode*>(GetParent());
	if (MirrorParent != nullptr)
		NODE_SYSTEM.SyncSocketAllowedTypes(GetParent()->GetID(), GetID(), NewTypes);

	return !NODE_SYSTEM.RevalidateSocketConnections(this);
}

Connection::Connection(NodeSocket* Out, NodeSocket* In)
{
	this->Out = Out;
	this->In = In;
}

Connection::~Connection()
{
	for (size_t i = 0; i < RerouteNodes.size(); i++)
		delete RerouteNodes[i];
}