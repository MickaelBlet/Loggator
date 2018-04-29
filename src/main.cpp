#include "Loggator.hpp"

int     main(void)
{
    using namespace Log;

    Loggator logTest;

    logTest("cerr");
    logTest.setOutStream(std::cout);
    logTest("cout");
    logTest.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{customKey:%s: }");
    logTest("new format.");
    logTest.setKey("customKey", "test");
    logTest("new key.");
    logTest.setName("new");
    logTest("new name.");

    logTest.setKey("customKey", "");

    logTest.send() << "Default send function";
    logTest.send(eTypeLog::Trace) << "Type send function";
    logTest.send(eTypeLog::Trace, "%s", "Type send function format");
    logTest.send(eTypeLog::Trace, "%s", "Type send function format") << " add";

    logTest.LSEND() << "Default LSEND macro";
    logTest.LSEND(Trace) << "Type LSEND macro";
    logTest.LSEND(Trace, "%s", "Type LSEND macro format");
    logTest.LSEND(Trace, "%s", "Type LSEND macro format") << " add";

    logTest.info() << "Default type function";
    logTest.info("%s", "type function format");
    logTest.info("%s", "type function format") << " add";

    logTest.LWARN() << "Default type macro";
    logTest.LWARN("%s", "type macro format");
    logTest.LWARN("%s", "type macro format") << " add";

    logTest("Default operator ()");
    logTest(eTypeLog::Trace, "operator () with type");
    logTest("%s", "Default operator () with format");
    logTest(eTypeLog::Trace, "%s", "operator () with type format");

    logTest.open("test1.log", std::ios::trunc);
    logTest.setFilter(eFilterLog::EqError);
    logTest.LERROR() << "error test1";
    logTest.LWARN() << "warn no output"; // no output

    logTest.open("test2.log", std::ios::trunc);
    logTest.setFilter(eFilterLog::LessError);
    logTest.LERROR() << "error no output"; // no output
    logTest.LWARN() << "warn test2.log";
    logTest.setFilter(eFilterLog::All);
    logTest.LERROR() << "error test2.log";

    Loggator logTestChild("", "test1.log");
    logTestChild = logTest;
    logTest.addChild(logTestChild);
    logTest.LERROR() << "error test1 and test2";
    logTestChild.setFilter(eFilterLog::EqDebug);
    logTest.LERROR() << "error only test2.log";
    logTest.LDEBUG() << "debug test1 and test2";

    return 0;
}