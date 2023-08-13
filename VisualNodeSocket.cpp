#include "VisualNodeSocket.h"
using namespace VisNodeSys;

std::unordered_map<std::string, ImColor> NodeSocket::SocketTypeToColorAssosiations;

NodeSocket::NodeSocket(Node* Parent, const std::string Type, const std::string Name, bool bOutput, std::function<void* ()> OutputDataFunction)
{
	this->Parent = Parent;
	this->Type = Type;
	this->Name = Name;
	this->ID = FocalEngine::APPLICATION.GetUniqueHexID();
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

std::string NodeSocket::GetType() const
{
	return Type;
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