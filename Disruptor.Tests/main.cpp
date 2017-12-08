#define BOOST_TEST_MODULE Disruptor.Tests


#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

#include <gmock/gmock.h>

#if _MSC_VER
# pragma warning (disable: 4231) // nonstandard extension used : 'extern' before template explicit instantiation
#endif

struct GlobalFixture
{
    GlobalFixture()
    {
        // all passed test names are printed to the output
        boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_test_units);
        // a very small report is printed to the output (passed tests count / total tests count)
        boost::unit_test::results_reporter::set_level(boost::unit_test::SHORT_REPORT);

        testing::GTEST_FLAG(throw_on_failure) = true;
        testing::InitGoogleMock(&boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);
    }

    ~GlobalFixture()
    {
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);

