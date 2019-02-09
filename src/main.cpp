#include "../single_include/loggator.hpp"

#ifndef VERSION
# define VERSION "1.0"
#endif

using namespace Log;

void test()
{
    LOGGATOR("example") << "macro LOGGATOR";
    LOGGATOR("example").setName("main");
}

int main()
{
    Loggator::openConfig("./loggator.ini");
    Loggator logExample("example", std::cout);
    logExample.setFormat("{TYPE:%-5s} {TIME:%y/%m/%d %X.%N}: {NAME:%s: }{FILE:%s:}{LINE:%s: }");
    logExample.debug() << "function";
    logExample.debug(LOGGATOR_SOURCEINFOS) << "source infos macro";
    test();
    LOGGATOR("main", WARN) << "macro LOGGATOR with type" << std::endl;
    LOGGATOR(logExample, INFO, "style %s ", "printf") << "LOGGATOR" << " " << VERSION;
    return (0);
}