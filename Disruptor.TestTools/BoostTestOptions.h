#pragma once

#include <boost/test/tree/traverse.hpp>
#include <boost/test/tree/decorator.hpp>

namespace Disruptor
{
namespace Tests
{

    static const boost::unit_test::label Manual("MANUAL");

    struct BoostTestsListerVisitor : boost::unit_test::test_tree_visitor
    {
        void visit(const boost::unit_test::test_case& test)
        {
            static const auto& master = boost::unit_test::framework::master_test_suite();
            static const auto& masterName = master.p_name.get();

            auto fullName = test.full_name();
            auto pos = fullName.find(masterName);
            if (pos != std::string::npos)
                fullName.erase(pos, masterName.length());

            if (test.has_label("MANUAL"))
                std::cout << "MANUAL_";
            std::cout << fullName << std::endl;
        }
    };

    inline void addAdditionalCommandLineOptions()
    {
        int argc = boost::unit_test::framework::master_test_suite().argc;
        for (int i = 0; i < argc; i++)
        {
            std::string argument(boost::unit_test::framework::master_test_suite().argv[i]);
            if (argument == "--list_tests")
            {
                BoostTestsListerVisitor visitor;
                boost::unit_test::traverse_test_tree(boost::unit_test::framework::master_test_suite(), visitor);
                exit(0);
            }
        }
    }

}
}
