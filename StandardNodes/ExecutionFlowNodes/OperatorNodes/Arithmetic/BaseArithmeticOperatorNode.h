#pragma once

#include <cmath>
#include <climits>
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

	std::string ResolveConnectedSocketType(VisNodeSys::NodeSocket* ConnectedSocket) const;

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
			case ArithmeticOperationType::MODULUS:
			{
				T Result = A;
				for (glm::length_t i = 0; i < Result.length(); i++)
					Result[i] = (B[i] == 0.0f) ? A[i] : std::fmod(A[i], B[i]);
				return Result;
			}
			default:
				return A;
		}
	}

	void Execute();

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

// Specialization for integer type.
template <>
inline int BaseArithmeticOperatorNode::PerformOperation<int>(const int& A, const int& B)
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
	{
		if (B == 0)
			return A;
		if (A == INT_MIN && B == -1)
			return INT_MAX; // INT_MIN / -1 overflows int; saturate like the POWER case.
		return A / B;
	}
	case ArithmeticOperationType::MODULUS:
	{
		if (B == 0)
			return A;
		if (A == INT_MIN && B == -1)
			return 0; // INT_MIN % -1 is mathematically 0.
		return A % B;
	}
	case ArithmeticOperationType::POWER:
	{
		const double Result = std::pow(static_cast<double>(A), static_cast<double>(B));

		if (!std::isfinite(Result))
			return 0;

		if (Result >= static_cast<double>(INT_MAX))
			return INT_MAX;

		if (Result <= static_cast<double>(INT_MIN))
			return INT_MIN;

		return static_cast<int>(Result);
	}
	default:
		return A;
	}
}

// Specialization for floating-point type
template <>
inline float BaseArithmeticOperatorNode::PerformOperation<float>(const float& A, const float& B)
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
