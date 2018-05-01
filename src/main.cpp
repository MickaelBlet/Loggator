#include "Loggator.hpp"
#include <chrono>

int     main(void)
{
    using namespace Log;

    Loggator logTest;

    logTest("cerr");
    logTest.setOutStream(std::cout);
    logTest("cout");
    logTest.setName("new");
    logTest("new name.");
    logTest.setFormat("{TYPE} {TIME} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{customKey:%s: }");
    logTest("new format.");
    logTest.setKey("customKey", "test");
    logTest("new key.");
    logTest.setKey("customKey", "");
    logTest("empty key.");

    logTest.send() << "Default send function";
    logTest.send(eTypeLog::Info) << "Type send function";
    logTest.send(eTypeLog::Info, "Type send function format without args");
    logTest.send(eTypeLog::Info, "%s", "Type send function format");
    logTest.send(eTypeLog::Info, "%s", "Type send function format") << " add";

    logTest.LSEND() << "Default LSEND macro";
    logTest.LSEND(Info) << "Type LSEND macro";
    logTest.LSEND(Info, "Type LSEND macro format without args");
    logTest.LSEND(Info, "%s", "Type LSEND macro format");
    logTest.LSEND(Info, "%s", "Type LSEND macro format") << " add";

    logTest.info() << "Default type function";
    logTest.info("type function format without args");
    logTest.info("%s", "type function format");
    logTest.info("%s", "type function format") << " add";

    logTest.LINFO() << "Default type macro";
    logTest.LINFO("type macro format without args");
    logTest.LINFO("%s", "type macro format");
    logTest.LINFO("%s", "type macro format") << " add";

    logTest("Default operator () format without args");
    logTest("%s", "Default operator () format");
    logTest(eTypeLog::Info, "Type operator () format without args");
    logTest(eTypeLog::Info, "%s", "Type operator () format");

    logTest("Default operator () format without args") << " add";
    logTest("%s", "Default operator () format") << " add";
    logTest(eTypeLog::Info) << "Type operator ()" << " add";
    logTest(eTypeLog::Info, "Type operator () format without args") << " add";
    logTest(eTypeLog::Info, "%s", "Type operator () format") << " add";

    logTest << "Default operator <<" << " with multiple args";
    logTest[eTypeLog::Info] << "operator [] test type info";

    // test file

    logTest.open("test1.log", std::ios::trunc);
    logTest.setFilter(eFilterLog::EqualError);
    logTest.LERROR() << "error test1";
    logTest.LWARN() << "warn no output"; // no output

    logTest.open("test2.log", std::ios::trunc); // close preview file
    logTest.setFilter(eFilterLog::LessError);
    logTest.LERROR() << "error no output"; // no output
    logTest.LWARN() << "warn test2.log";
    logTest.setFilter(eFilterLog::All);
    logTest.LERROR() << "error test2.log";

    Loggator logTestChild("", "test1.log"); // default open in append mode
    logTestChild = logTest;
    logTest.addChild(logTestChild);
    logTest.listen(logTestChild);
    logTest.LERROR() << "error test1 and test2";
    logTestChild.setFilter(eFilterLog::EqualDebug);
    logTest.LERROR() << "error only test2.log";
    logTest.LDEBUG() << "debug test1 and test2";
    logTestChild.LDEBUG() << "debug test1 and test2";

    // test multi thread

    logTest.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{THREAD_ID}: ");
    logTestChild.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{THREAD_ID}: ");
    std::thread thread[4];
    for (int i = 0; i < 4; i++)
    {
        thread[i] = std::thread([&](int t)
        {
            for (int j = 0; j < 5; j++)
                logTest << "test in thread" << t;
        }, i);
    }
    for (int i = 0; i < 4; i++)
    {
        thread[i].join();
    }

    logTest.close();
    logTestChild.close();

    // look files

    std::string line;

    std::ifstream fileTestStream1("test1.log");
    std::cout << "\ntest1.log" << std::endl;
    while (std::getline(fileTestStream1, line))
        std::cout << line << std::endl;

    std::ifstream fileTestStream2("test2.log");
    std::cout << "\ntest2.log" << std::endl;
    while (std::getline(fileTestStream2, line))
        std::cout << line << std::endl;

    return 0;
}