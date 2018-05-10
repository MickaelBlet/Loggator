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
    logTest.setFormat("{TYPE} {NAME:%s: }{FILE:%s: }{customKey:%s: }");
    logTest.LINFO("test1");

    // CHECK OUTPUT
    EXPECT_EQ(oss.str(), "INFO  test1: setKey.cpp: test1\n");
    CLEAR_STRINGSTREAM(oss);

    logTest.setKey("customKey", "testKey");
    logTest.LINFO("test2");

    // CHECK OUTPUT
    EXPECT_EQ(oss.str(), "INFO  test1: setKey.cpp: testKey: test2\n");
    CLEAR_STRINGSTREAM(oss);

    logTest.setFormat("{TIME:%y/%m/%d %H:%M:%S.%N: }");
    logTest.LWARN("test3");
    std::string stringCpy = oss.str();
    // std::cout << stringCpy;
    // CHECK COUNT OUTPUT
    EXPECT_EQ(2, std::count(stringCpy.begin(), stringCpy.end(), '/'));
    // CHECK COUNT OUTPUT
    EXPECT_EQ(3, std::count(stringCpy.begin(), stringCpy.end(), ':'));
    // CHECK COUNT OUTPUT
    EXPECT_EQ(1, std::count(stringCpy.begin(), stringCpy.end(), '.'));
}