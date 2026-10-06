#include "UnsignedLiteralNode.h"
#include "imgui.h"
using namespace VisNodeSys;

UnsignedLiteralNode::UnsignedLiteralNode() : VisNodeSys::Node()
{
	Type = "UnsignedLiteralNode";

	SetStyle(DEFAULT);
	SetName("Unsigned Literal");

	TitleBackgroundColor = ImColor(30, 221, 170);
	TitleBackgroundColorHovered = ImColor(139, 235, 199);

	AddSocket(new NodeSocket(this, "UINT", "Out", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(170, NODE_HEIGHT_PER_SOCKET * 2));
	if (!Output.empty())
	{
		Output[0]->SetFunctionToOutputData(UIntDataGetter);
		Output[0]->SetCanBeDeletedByUser(false);
	}
}

UnsignedLiteralNode::UnsignedLiteralNode(const UnsignedLiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (!Output.empty())
		Output[0]->SetFunctionToOutputData(UIntDataGetter);
}

Json::Value UnsignedLiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Value"] = Data;
	return Result;
}

bool UnsignedLiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Value"))
		return false;

	if (!Json["Value"].isUInt())
		return false;

	Data = Json["Value"].asUInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(UIntDataGetter);

	return true;
}

void UnsignedLiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	float XPosition = ImGui::GetCursorScreenPos().x - 17.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 0.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(100 * Zoom);
	if (ImGui::InputScalar("##value", ImGuiDataType_U64, &Data))
	{
		if (Output.size() > 0) 
		{
			for (size_t i = 0; i < Output[0]->GetConnectedSockets().size(); ++i) 
			{
				ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[i], UPDATE);
			}
		}
	}
}

void UnsignedLiteralNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool UnsignedLiteralNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

unsigned int UnsignedLiteralNode::GetData() const
{
	return Data;
}

void UnsignedLiteralNode::SetData(unsigned int NewData)
{
	Data = NewData;
}
