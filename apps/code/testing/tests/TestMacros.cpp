#include "testing/Test.h"
#include "MockTestResult.h"

static int g_firstArgCallCount = 0;
static int g_secondArgCallCount = 0;

static int FirstArg()
{
	return g_firstArgCallCount++;
}

static int SecondArg()
{
	return ++g_secondArgCallCount;
}

class TestCaseEvalCheckEqual : public testing::Test
{
public:
    TestCaseEvalCheckEqual() : Test("EmptyTestCase", __FILE__, __LINE__) {}
protected:
    virtual void RunTest (testing::TestResult& result_)
    {
        EXPECT_EQ(FirstArg(), SecondArg());
    }
};

TEST (CheckEqualsOnlyEvaluatesArgumentsOnce)
{
	MockTestResult result;
	TestCaseEvalCheckEqual testCase;
	testCase.Run(result);

	EXPECT_EQ(1, g_firstArgCallCount);
	EXPECT_EQ(1, g_secondArgCallCount);
}

bool CheckPasses(bool condition)
{
	const char * m_name = "Test";
        testing::TestResult result_;
	EXPECT_TRUE(condition);
	return result_.FailureCount() == 0;
}

TEST (CheckPassesWithTrue)
{
    EXPECT_TRUE(CheckPasses(true));
}

TEST (CheckFailsWithFalse)
{
    EXPECT_TRUE(!CheckPasses(false));
}

bool CheckEqualPasses(int a, int b)
{
	const char * m_name = "Test";
	testing::TestResult result_;
	EXPECT_EQ(a, b);
	return result_.FailureCount() == 0;
}

TEST (CheckEqualWorksWithSameNumber)
{
	EXPECT_TRUE(CheckEqualPasses(2,2));
}

TEST (CheckEqualFailsWithDifferentNumbers)
{
	EXPECT_TRUE(!CheckEqualPasses(2,3));
}


bool CheckArray2DClosePasses(const int a[4][3], const int b[4][3], int rows, int columns)
{
	const char * m_name = "Test";
	testing::TestResult result_;
	EXPECT_ARRAY2D_CLOSE (a, b, rows, columns, 0.00001f);
	return result_.FailureCount() == 0;
}

TEST (CheckArray2DClosePassesWithSameArray)
{
	const int array[4][3] =
	{
		{ 1, 2, 3 },
		{ 4, 5, 6 },
		{ 7, 8, 9 },
		{ 1, 2, 8 }
	};

	EXPECT_TRUE(CheckArray2DClosePasses(array, array, 4, 3));
}

TEST (CheckArray2DCloseFailsWithDifferentArrays)
{
	const int array1[4][3] =
	{
		{ 1, 2, 3 },
		{ 4, 5, 6 },
		{ 7, 8, 9 },
		{ 1, 2, 8 }
	};
	const int array2[4][3] =
	{
		{ 1, 2, 3 },
		{ 4, 5, 6 },
		{ 7, 8, 3 },
		{ 1, 2, 8 }
	};

	EXPECT_TRUE(!CheckArray2DClosePasses(array1, array2, 4, 3));
}
