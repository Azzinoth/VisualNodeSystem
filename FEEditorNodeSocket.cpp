#include "FEEditorNodeSocket.h"

FEEditorNodeSocket::FEEditorNodeSocket(FEEditorNode* parent, FEEditorNodeSocketType type, std::string name)
{
	this->parent = parent;
	this->type = type;
	this->name = name;
	this->ID = APPLICATION.getUniqueHexID();
}

std::string FEEditorNodeSocket::getID()
{
	return ID;
}

FEEditorNode* FEEditorNodeSocket::getParent()
{
	return parent;
}

std::vector<FEEditorNodeSocket*> FEEditorNodeSocket::getConnections()
{
	return connections;
}

std::string FEEditorNodeSocket::getName()
{
	return name;
}

FEEditorNodeSocketType FEEditorNodeSocket::getType()
{
	return type;
}

bool FEEditorNodeSocket::getForcedConnectionColor(ImColor& color)
{
	if (forceColor == nullptr)
		return false;

	color = *forceColor;

	return true;
}

void FEEditorNodeSocket::setForcedConnectionColor(ImColor* newValue)
{
	forceColor = newValue;
}

FEEditorNodeConnection::FEEditorNodeConnection(FEEditorNodeSocket* out, FEEditorNodeSocket* in)
{
	this->out = out;
	this->in = in;
}