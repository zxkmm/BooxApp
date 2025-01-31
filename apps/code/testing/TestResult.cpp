#include "TestResult.h"
#include "Failure.h"
#include <ctime>

namespace testing {

TestResult::TestResult()
        : m_failureCount (0),
          m_testCount(0),
          m_startTime(clock()),
          m_secondsElapsed(0.0f)
{
}

TestResult::~TestResult()
{
}

void TestResult::TestWasRun()
{
    m_testCount++;
}

void TestResult::StartTests ()
{
}

void TestResult::AddFailure (const Failure & )
{
    m_failureCount++;
}

void TestResult::EndTests ()
{
    m_secondsElapsed = (clock() - m_startTime)/float(CLOCKS_PER_SEC);
}

int TestResult::FailureCount() const
{
    return m_failureCount;
}

int TestResult::TestCount() const
{
    return m_testCount;
}

}  // namespace testing
