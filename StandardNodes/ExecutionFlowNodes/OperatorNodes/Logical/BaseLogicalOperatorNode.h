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
	bool bLocalBool = false;

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

		bool bFirstBool = false;
		bool bSecondBool = false;
		if (OperatorType == LogicalNodeOperatorType::NOT)
		{
			if (AData == nullptr)
				return;

			bFirstBool = *reinterpret_cast<bool*>(AData);
		}
		else
		{
			if (AData == nullptr || BData == nullptr)
				return;

			bFirstBool = *reinterpret_cast<bool*>(AData);
			bSecondBool = *reinterpret_cast<bool*>(BData);
		}

		switch (OperatorType)
		{
			case LogicalNodeOperatorType::AND:
				bLocalBool = bFirstBool && bSecondBool;
				break;

			case LogicalNodeOperatorType::OR:
				bLocalBool = bFirstBool || bSecondBool;
				break;

			case LogicalNodeOperatorType::XOR:
				bLocalBool = bFirstBool ^ bSecondBool;
				break;

			case LogicalNodeOperatorType::NOT:
				bLocalBool = !bFirstBool;
				break;

			default:
				bLocalBool = bFirstBool && bSecondBool;
				break;
		}
	}

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &bLocalBool;
	};

public:
	BaseLogicalOperatorNode(bool bNeedBInput = true);
	BaseLogicalOperatorNode(const BaseLogicalOperatorNode& Other);
	virtual ~BaseLogicalOperatorNode() = default;

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};