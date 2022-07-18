#include "FEVisualNodeSocket.h"

FEVisualNodeSocket::FEVisualNodeSocket(FEVisualNode* Parent, const FE_VISUAL_NODE_SOCKET_TYPE Type, const std::string Name)
{
	this->Parent = Parent;
	this->Type = Type;
	this->Name = Name;
	this->ID = APPLICATION.GetUniqueHexID();
}

std::string FEVisualNodeSocket::GetID()
{
	return ID;
}

FEVisualNode* FEVisualNodeSocket::GetParent() const
{
	return Parent;
}

std::vector<FEVisualNodeSocket*> FEVisualNodeSocket::GetConnections()
{
	return Connections;
}

std::string FEVisualNodeSocket::GetName()
{
	return Name;
}

FE_VISUAL_NODE_SOCKET_TYPE FEVisualNodeSocket::GetType() const
{
	return Type;
}

bool FEVisualNodeSocket::GetForcedConnectionColor(ImColor& Color) const
{
	if (ForceColor == nullptr)
		return false;

	Color = *ForceColor;

	return true;
}

void FEVisualNodeSocket::SetForcedConnectionColor(ImColor* NewValue)
{
	ForceColor = NewValue;
}

FEVisualNodeConnection::FEVisualNodeConnection(FEVisualNodeSocket* Out, FEVisualNodeSocket* In)
{
	this->Out = Out;
	this->In = In;
}