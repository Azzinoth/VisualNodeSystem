#include "Basic/Basic.h"
using namespace VisNodeSys;

int main()
{
	NODE_SYSTEM.Initialize(true);

	testing::GTEST_FLAG(output) = "xml:Results.xml";
	testing::InitGoogleTest();
	int TestResult = RUN_ALL_TESTS();

	return TestResult;
}