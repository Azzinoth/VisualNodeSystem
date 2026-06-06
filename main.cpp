#include "BasicTests/Basic.h"
using namespace VisNodeSys;

int main(int ArgumentsCount, char** Arguments)
{
	NODE_SYSTEM.Initialize(true);

	testing::GTEST_FLAG(output) = "xml:Results.xml";
	testing::InitGoogleTest(&ArgumentsCount, Arguments);
	int TestResult = RUN_ALL_TESTS();

	return TestResult;
}