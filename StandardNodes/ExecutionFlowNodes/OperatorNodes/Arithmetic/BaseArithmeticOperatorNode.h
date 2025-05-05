#pragma once

#include "../../BaseExecutionFlowNode.h"

enum class ArithmeticOperationType
{
	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	MODULUS,
	POWER
};

class BaseArithmeticOperatorNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

protected:
	ArithmeticOperationType OperatorType = ArithmeticOperationType::ADD;
private:
	// TO-DO: use std::optional to save memory.
	int LocalIntegerData = 0;
	float LocalFloatData = 0.0f;
	glm::vec2 LocalVec2Data = glm::vec2(0.0f);
	glm::vec3 LocalVec3Data = glm::vec3(0.0f);
	glm::vec4 LocalVec4Data = glm::vec4(0.0f);

	// Generic arithmetic function for basic types (int, float)
	template <typename T>
	T PerformOperation(const T& A, const T& B)
	{
		switch (OperatorType)
		{
			case ArithmeticOperationType::ADD:
				return A + B;
			case ArithmeticOperationType::SUBTRACT:
				return A - B;
			case ArithmeticOperationType::MULTIPLY:
				return A * B;
			case ArithmeticOperationType::DIVIDE:
				if (B == T(0))
					return A;
				return A / B;
			default:
				return A;
		}
	}

	// Specialization for integer type.
	template <>
	int PerformOperation<int>(const int& A, const int& B)
	{
		switch (OperatorType)
		{
		case ArithmeticOperationType::ADD:
			return A + B;
		case ArithmeticOperationType::SUBTRACT:
			return A - B;
		case ArithmeticOperationType::MULTIPLY:
			return A * B;
		case ArithmeticOperationType::DIVIDE:
			if (B == 0)
				return A;
			return A / B;
		case ArithmeticOperationType::MODULUS:
			if (B == 0)
				return A;
			return A % B;
		case ArithmeticOperationType::POWER:
			return static_cast<int>(std::pow(static_cast<double>(A), static_cast<double>(B)));
		default:
			return A;
		}
	}

	// Specialization for floating-point type
	template <>
	float PerformOperation<float>(const float& A, const float& B)
	{
		switch (OperatorType)
		{
		case ArithmeticOperationType::ADD:
			return A + B;
		case ArithmeticOperationType::SUBTRACT:
			return A - B;
		case ArithmeticOperationType::MULTIPLY:
			return A * B;
		case ArithmeticOperationType::DIVIDE:
			if (B == 0.0f)
				return A;
			return A / B;
		case ArithmeticOperationType::POWER:
			return std::pow(A, B);
		case ArithmeticOperationType::MODULUS:
			if (B == 0.0f)
				return A;
			return std::fmod(A, B);
		default:
			return A;
		}
	}

	void Execute()
	{
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

	std::function<void* ()> ResultDataGetter = [this]() -> void* {
		std::string CurrentMode = GetActiveINDataType();
		if (CurrentMode.empty())
			return nullptr;

		// Return the appropriate data pointer based on the current mode.
		if (CurrentMode == "INT")
		{
			return &LocalIntegerData;
		}
		else if (CurrentMode == "FLOAT")
		{
			return &LocalFloatData;
		}
		else if (CurrentMode == "VEC2")
		{
			return &LocalVec2Data;
		}
		else if (CurrentMode == "VEC3")
		{
			return &LocalVec3Data;
		}
		else if (CurrentMode == "VEC4")
		{
			return &LocalVec4Data;
		}

		return nullptr;
	};

public:
	BaseArithmeticOperatorNode(std::vector<std::string> AllowedTypes = { "INT", "FLOAT", "VEC2", "VEC3", "VEC4" });
	BaseArithmeticOperatorNode(const BaseArithmeticOperatorNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	std::string GetActiveINDataType();
};