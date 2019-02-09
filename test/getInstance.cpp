#include <gtest/gtest.h>

#include "loggator.hpp"

#ifndef CLEAR_STRINGSTREAM
# define CLEAR_STRINGSTREAM(ss) ss.clear(); ss.str("")
#endif

// TEST(Loggator, DISABLED_getInstance)
TEST(Loggator, getInstance)
{
    using namespace Log;

    std::ostringstream oss1;
    std::ostringstream oss2;

    Loggator *logTest1 = new Loggator("test1", oss1);
    Loggator logTest2("test2", oss2);

    Loggator &fromInstance1 = Loggator::getInstance("test1");
    Loggator &fromInstance2 = Loggator::getInstance("test2");

    // CHECK INSTANCE 1
    EXPECT_EQ(&fromInstance1, logTest1);
    // CHECK INSTANCE 2
    EXPECT_EQ(&fromInstance2, &logTest2);

    // CHECK NO THROW
    EXPECT_NO_THROW(Loggator::getInstance("test1"));

    delete logTest1;

    // CHECK THROW
    EXPECT_ANY_THROW(Loggator::getInstance("test1"));

    // CHECK NO THROW
    EXPECT_NO_THROW(Loggator::getInstance("test2"));

    // CHECK THROW
    EXPECT_ANY_THROW(Loggator::getInstance("another"));
}