#include <gtest/gtest.h>
#include <algorithm>

#include "Loggator.hpp"

#ifndef CLEAR_STRINGSTREAM
# define CLEAR_STRINGSTREAM(ss) ss.clear(); ss.str("")
#endif

// TEST(Loggator, DISABLED_setKey)
TEST(Loggator, setKey)
{
    using namespace Log;

    std::ostringstream oss;

    Loggator logTest("test1", oss);
    logTest.setFormat("{TYPE} {NAME:%s: }{FILE:%s: }{customKey1:%s: }{customKey2:%s: }");
    logTest.LINFO("test1");

    // CHECK OUTPUT
    EXPECT_EQ(oss.str(), "INFO test1: setKey.cpp: test1\n");
    CLEAR_STRINGSTREAM(oss);

    logTest.setKey("customKey1", "testKey");
    logTest.LINFO("test2");

    // CHECK OUTPUT
    EXPECT_EQ(oss.str(), "INFO test1: setKey.cpp: testKey: test2\n");
    CLEAR_STRINGSTREAM(oss);

    logTest.setKey("customKey2", "testKey");
    logTest.LINFO("test3");

    // CHECK OUTPUT
    EXPECT_EQ(oss.str(), "INFO test1: setKey.cpp: testKey: testKey: test3\n");
    CLEAR_STRINGSTREAM(oss);
}