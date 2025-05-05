#include "VisualNodeSocket.h"
using namespace VisNodeSys;

std::unordered_map<std::string, ImColor> NodeSocket::SocketTypeToColorAssociations;

NodeSocket::NodeSocket(Node* Parent, const std::string Type, const std::string Name, bool bOutput, std::function<void* ()> OutputDataFunction)
{
	this->Parent = Parent;
	this->AllowedTypes.push_back(Type);
	this->Name = Name;
	this->ID = NODE_CORE.GetUniqueHexID();
	this->bOutput = bOutput;
	this->OutputData = OutputDataFunction;
}

NodeSocket::NodeSocket(Node* Parent, const std::vector<std::string> Types, const std::string Name, bool bOutput, std::function<void* ()> OutputDataFunction)
{
	this->Parent = Parent;
	this->AllowedTypes = Types;
	this->Name = Name;
	this->ID = NODE_CORE.GetUniqueHexID();
	this->bOutput = bOutput;
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

std::vector<NodeSocket*> NodeSocket::GetConnectedSockets()
{
	return ConnectedSockets;
}

std::string NodeSocket::GetName() const
{
	return Name;
}

std::vector<std::string> NodeSocket::GetAllowedTypes() const
{
	return AllowedTypes;
}

void NodeSocket::SetFunctionToOutputData(std::function<void* ()> NewFunction)
{
	OutputData = NewFunction;
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