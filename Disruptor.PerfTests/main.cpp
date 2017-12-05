#include "stdafx.h"

#include <boost/algorithm/string.hpp>

#include "LatencyTestSession.h"
#include "TestRepository.h"
#include "ThroughputTestSession.h"

using namespace Disruptor::PerfTests;

void runAllTests(const TestRepository& testRepository);
void runOneTest(const TestRepository& testRepository, const std::string& testName);

int main(int, char**)
{
    auto& testRepository = TestRepository::instance();

    std::string testName;

    std::cout << "Test name (ALL by default):  " << testName << " ?" << std::endl;

    std::getline(std::cin, testName);

    boost::algorithm::trim(testName);

    if (boost::algorithm::iequals(testName, "ALL") || testName.empty())
    {
        runAllTests(testRepository);
    }
    else
    {
        runOneTest(testRepository, testName);
    }

    return 0;
}

void runAllTests(const TestRepository& testRepository)
{
    for (auto&& info : testRepository.allLatencyTests())
    {
        LatencyTestSession session(info);
        session.run();
    }

    for (auto&& info : testRepository.allThrougputTests())
    {
        ThroughputTestSession session(info);
        session.run();
    }
}

void runOneTest(const TestRepository& testRepository, const std::string& testName)
{
    LatencyTestInfo latencyTestInfo;
    if (testRepository.tryGetLatencyTest(testName, latencyTestInfo))
    {
        LatencyTestSession session(latencyTestInfo);
        session.run();
    }
    else
    {
        ThroughputTestInfo throughputTestInfo;
        if (testRepository.tryGetThroughputTest(testName, throughputTestInfo))
        {
            ThroughputTestSession session(throughputTestInfo);
            session.run();
        }
    }
}
