#pragma once

#include "../../BaseExecutionFlowNode.h"

enum class LogicalNodeOperatorType
{
	AND,
	NOT,
	OR,
	XOR
};

class BaseLogicalOperatorNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

protected:
	LogicalNodeOperatorType OperatorType = LogicalNodeOperatorType::AND;
private:
	bool LocalBoolData = false;

	void Execute()
	{
		// If we don't have both A and B inputs connected, we can't do anything.
		if (Input[1]->GetConnectedSockets().empty() &&
			Input[2]->GetConnectedSockets().empty())
			return;

		void* AData = nullptr;
		if (!Input[1]->GetConnectedSockets().empty())
			AData = Input[1]->GetConnectedSockets()[0]->GetData();

		void* BData = nullptr;
		if (Input.size() >= 3 && !Input[2]->GetConnectedSockets().empty())
			BData = Input[2]->GetConnectedSockets()[0]->GetData();

		if (AData == nullptr && BData == nullptr)
			return;

		bool A = false;
		bool B = false;
		if (OperatorType == LogicalNodeOperatorType::NOT)
		{
			if (AData == nullptr)
				return;

			A = *reinterpret_cast<bool*>(AData);
		}
		else
		{
			if (AData == nullptr || BData == nullptr)
				return;

			A = *reinterpret_cast<bool*>(AData);
			B = *reinterpret_cast<bool*>(BData);
		}

		switch (OperatorType)
		{
			case LogicalNodeOperatorType::AND:
				LocalBoolData = A && B;
				break;

			case LogicalNodeOperatorType::OR:
				LocalBoolData = A || B;
				break;

			case LogicalNodeOperatorType::XOR:
				LocalBoolData = A ^ B;
				break;

			case LogicalNodeOperatorType::NOT:
				LocalBoolData = !A;
				break;

			default:
				LocalBoolData = A && B;
				break;
		}
	}

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &LocalBoolData;
	};

public:
	BaseLogicalOperatorNode(bool bNeedBInput = true);
	BaseLogicalOperatorNode(const BaseLogicalOperatorNode& Other);
	virtual ~BaseLogicalOperatorNode() = default;

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};