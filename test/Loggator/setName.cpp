#include <gtest/gtest.h>
#include <algorithm>

#include "Loggator.hpp"

#ifndef CLEAR_STRINGSTREAM
# define CLEAR_STRINGSTREAM(ss) ss.clear(); ss.str("")
#endif

// TEST(Loggator, DISABLED_setName)
TEST(Loggator, setName)
{
    using namespace Log;

    std::ostringstream oss;

    Loggator logTest("test", oss);

    Loggator &fromInstance = Loggator::getInstance("test");

    // CHECK INSTANCE
    EXPECT_EQ(&fromInstance, &logTest);

    // SET NAME
    logTest.setName("foo");

    // CHECK THROW
    EXPECT_ANY_THROW(Loggator::getInstance("test"));

    // CHECK NO THROW
    EXPECT_NO_THROW(Loggator::getInstance("foo"));
}