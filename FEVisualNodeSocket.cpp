#include "FEVisualNodeSocket.h"

FEVisualNodeSocket::FEVisualNodeSocket(FEVisualNode* parent, FEVisualNodeSocketType type, std::string name)
{
	this->parent = parent;
	this->type = type;
	this->name = name;
	this->ID = APPLICATION.getUniqueHexID();
}

std::string FEVisualNodeSocket::getID()
{
	return ID;
}

FEVisualNode* FEVisualNodeSocket::getParent()
{
	return parent;
}

std::vector<FEVisualNodeSocket*> FEVisualNodeSocket::getConnections()
{
	return connections;
}

std::string FEVisualNodeSocket::getName()
{
	return name;
}

FEVisualNodeSocketType FEVisualNodeSocket::getType()
{
	return type;
}

bool FEVisualNodeSocket::getForcedConnectionColor(ImColor& color)
{
	if (forceColor == nullptr)
		return false;

	color = *forceColor;

	return true;
}

void FEVisualNodeSocket::setForcedConnectionColor(ImColor* newValue)
{
	forceColor = newValue;
}

FEVisualNodeConnection::FEVisualNodeConnection(FEVisualNodeSocket* out, FEVisualNodeSocket* in)
{
	this->out = out;
	this->in = in;
}