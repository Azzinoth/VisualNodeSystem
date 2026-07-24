#include "BaseArithmeticOperatorNode.h"
#include "../../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BaseArithmeticOperatorNode::BaseArithmeticOperatorNode(std::vector<std::string> AllowedTypes) : BaseExecutionFlowNode()
{
	Type = "BaseArithmeticOperatorNode";

	SetStyle(DEFAULT);
	SetName("Base Arithmetic Operator");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, AllowedTypes, "A", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, AllowedTypes, "B", NodeSocket::SocketFlow::Input));

	AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, AllowedTypes, "Result", NodeSocket::SocketFlow::Output));
	Output[1]->SetFunctionToOutputData(ResultDataGetter);

	SetSize(ImVec2(130.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * std::max(Input.size(), Output.size()))));

	// All sockets are structural and must not be user-deletable.
	for (NodeSocket* Socket : Input)
		Socket->SetCanBeDeletedByUser(false);
	for (NodeSocket* Socket : Output)
		Socket->SetCanBeDeletedByUser(false);
}

BaseArithmeticOperatorNode::BaseArithmeticOperatorNode(const BaseArithmeticOperatorNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);

	OperatorType = Other.OperatorType;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() > 1 && Output[1] != nullptr)
		Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value BaseArithmeticOperatorNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	return Result;
}

bool BaseArithmeticOperatorNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);

	return true;
}

void BaseArithmeticOperatorNode::Draw()
{	
	Node::Draw();
}

std::string BaseArithmeticOperatorNode::ResolveConnectedSocketType(NodeSocket* ConnectedSocket) const
{
	const std::vector<std::string>& AllowedTypes = ConnectedSocket->GetAllowedTypes();
	if (AllowedTypes.empty())
		return "";

	if (AllowedTypes.size() == 1)
		return AllowedTypes[0];

	// If multi type socket is connected, we need to resolve the type based on the other socket.
	BaseArithmeticOperatorNode* ProducerNode = dynamic_cast<BaseArithmeticOperatorNode*>(ConnectedSocket->GetParent());
	if (ProducerNode != nullptr)
		return ProducerNode->GetActiveINDataType();

	return AllowedTypes[0];
}

std::string BaseArithmeticOperatorNode::GetActiveINDataType()
{
	// Both A and B input sockets are required to determine the active data type.
	if (Input.size() <= 2)
		return "";

	if (Input[1]->GetConnectedSockets().empty() && Input[2]->GetConnectedSockets().empty())
		return "";

	if (Input[1]->GetConnectedSockets().empty() && !Input[2]->GetConnectedSockets().empty())
		return ResolveConnectedSocketType(Input[2]->GetConnectedSockets()[0]);

	if (!Input[1]->GetConnectedSockets().empty() && Input[2]->GetConnectedSockets().empty())
		return ResolveConnectedSocketType(Input[1]->GetConnectedSockets()[0]);

	// If we have both A and B inputs connected, we can check their types.
	std::string AInputType = ResolveConnectedSocketType(Input[1]->GetConnectedSockets()[0]);
	std::string BInputType = ResolveConnectedSocketType(Input[2]->GetConnectedSockets()[0]);

	if (AInputType == BInputType)
		return AInputType;

	return "";
}

void BaseArithmeticOperatorNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		Execute();

		if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BaseArithmeticOperatorNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	std::vector<std::string> CandidateAllowedTypes = CandidateSocket->GetAllowedTypes();
	if (CandidateAllowedTypes.size() == 1)
	{
		if (CandidateAllowedTypes[0] != "EXECUTE")
		{
			if (Input.size() > 1 && !Input[1]->GetConnectedSockets().empty())
			{
				std::string AInputType = Input[1]->GetConnectedSockets()[0]->GetAllowedTypes()[0];
				if (AInputType != CandidateAllowedTypes[0])
					return false;
			}

			if (Input.size() > 2 && !Input[2]->GetConnectedSockets().empty())
			{
				std::string BInputType = Input[2]->GetConnectedSockets()[0]->GetAllowedTypes()[0];
				if (BInputType != CandidateAllowedTypes[0])
					return false;
			}
		}

		return true;
	}

	if (CandidateSocket->GetParent() == nullptr)
		return false;

	BaseArithmeticOperatorNode* CandidateNode = dynamic_cast<BaseArithmeticOperatorNode*>(CandidateSocket->GetParent());
	if (CandidateNode == nullptr)
		return false;
	
	std::string CandidateNodeActiveDataType = CandidateNode->GetActiveINDataType();
	// If node already have data type, check if it is the same as candidate node.
	if (!GetActiveINDataType().empty())
	{
		if (CandidateNodeActiveDataType != GetActiveINDataType())
			return false;
	}
	
	return true;
}

void BaseArithmeticOperatorNode::Execute()
{
	// Both A and B input sockets are required to perform the operation.
	if (Input.size() <= 2)
		return;

	std::string CurrentMode = GetActiveINDataType();
	if (CurrentMode.empty())
		return;

	// If we don't have both A and B inputs connected, we can't do anything.
	if (Input[1]->GetConnectedSockets().empty() &&
		Input[2]->GetConnectedSockets().empty())
		return;

	void* AData = nullptr;
	if (!Input[1]->GetConnectedSockets().empty())
		AData = Input[1]->GetConnectedSockets()[0]->GetData();

	void* BData = nullptr;
	if (!Input[2]->GetConnectedSockets().empty())
		BData = Input[2]->GetConnectedSockets()[0]->GetData();

	if (AData == nullptr && BData == nullptr)
		return;

	// Call the appropriate operation method based on the data type.
	if (CurrentMode == "INT")
	{
		int A = 0;
		if (AData != nullptr)
			A = *reinterpret_cast<int*>(AData);

		int B = 0;
		if (BData != nullptr)
			B = *reinterpret_cast<int*>(BData);

		LocalIntegerData = PerformOperation(A, B);
	}
	else if (CurrentMode == "FLOAT")
	{
		float A = 0.0f;
		if (AData != nullptr)
			A = *reinterpret_cast<float*>(AData);

		float B = 0.0f;
		if (BData != nullptr)
			B = *reinterpret_cast<float*>(BData);

		LocalFloatData = PerformOperation(A, B);
	}
	else if (CurrentMode == "VEC2")
	{
		glm::vec2 A = glm::vec2(0.0f);
		if (AData != nullptr)
			A = *reinterpret_cast<glm::vec2*>(AData);

		glm::vec2 B = glm::vec2(0.0f);
		if (BData != nullptr)
			B = *reinterpret_cast<glm::vec2*>(BData);

		LocalVec2Data = PerformOperation(A, B);
	}
	else if (CurrentMode == "VEC3")
	{
		glm::vec3 A = glm::vec3(0.0f);
		if (AData != nullptr)
			A = *reinterpret_cast<glm::vec3*>(AData);

		glm::vec3 B = glm::vec3(0.0f);
		if (BData != nullptr)
			B = *reinterpret_cast<glm::vec3*>(BData);

		LocalVec3Data = PerformOperation(A, B);
	}
	else if (CurrentMode == "VEC4")
	{
		glm::vec4 A = glm::vec4(0.0f);
		if (AData != nullptr)
			A = *reinterpret_cast<glm::vec4*>(AData);

		glm::vec4 B = glm::vec4(0.0f);
		if (BData != nullptr)
			B = *reinterpret_cast<glm::vec4*>(BData);

		LocalVec4Data = PerformOperation(A, B);
	}
}
