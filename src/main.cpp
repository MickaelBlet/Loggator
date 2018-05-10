/**
 * @brief 
 * 
 * @file main.cpp
 * @author Blet Mickael
 * @date 2018-05-01
 */

#include "Loggator.hpp"
#include <chrono>

int     main(void)
{
    using namespace Log;
    Loggator main("main", std::cout);
    Loggator *main2 = new Loggator("main2", main);
    delete main2;
    Loggator main3("main2", main);

    std::thread thread1([&]{
        std::cout << std::hex << (unsigned long)&Loggator::getInstance("main2") << std::endl;
    std::cout << std::endl;
    {
        Loggator logExample("example", std::cout);
        logExample.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{THREAD_ID}: ");
        logExample.LINFO("test1");
        logExample.info("test2");
    }
    std::cout << std::endl;
    {
        Loggator logExample("example", std::cout);
        logExample.setFormat("{TYPE} {NAME:%s: }{customKey:%s: }");
        logExample.warn("test1");
        logExample.setKey("customKey", "myKey");
        logExample.warning("test2");
        logExample.setName("");
        logExample.error("test3");
    }
    std::cout << std::endl;
    {
        Loggator logExample1("example1", "example1.log", std::ios::trunc, eFilterLog::ALL);
        Loggator logExample2("example2", "example2.log", std::ios::trunc, eFilterLog::EQUAL_INFO);
        logExample2.setFormat("{TIME:%X.%N} {NAME}: ");
        logExample1.addChild(logExample2);
        logExample1.info("test1");
        logExample1.warn("test2");
        logExample1.subChild(logExample2);
        logExample1.info("test3");
        logExample2.listen(logExample1);
        logExample1.info("test4");
        logExample2.unlisten(logExample1);
        logExample1.info("test5");
        logExample2.info("test6");
    }
    std::string line;
    std::ifstream fileTestStream1("example1.log");
    std::cout << "example1.log" << std::endl;
    while (std::getline(fileTestStream1, line))
        std::cout << line << std::endl;
    std::cout << std::endl;
    std::ifstream fileTestStream2("example2.log");
    std::cout << "example2.log" << std::endl;
    while (std::getline(fileTestStream2, line))
        std::cout << line << std::endl;
    std::cout << std::endl;
    {
        Loggator logExample("example", std::cout);
        logExample.setFormat("{TYPE}: {LINE:%s: }");
        logExample.send() << "test1";
        logExample.send(eTypeLog::INFO) << "test2";
        logExample.send(eTypeLog::INFO, "test3");
        logExample.send(eTypeLog::INFO, "%s", "test4");
        logExample.send(eTypeLog::INFO, "%s%i", "test", 5) << " extra.";
        logExample.LSEND() << "test6";
        logExample.LSEND(INFO) << "test7";
        logExample.LSEND(INFO, "%s%i", "test", 8) << " extra."; // limited 19 args
        logExample.LSENDF(INFO, "%s%i", "test", 9) << " extra."; // unlimited args
    }
    std::cout << std::endl;
    {
        Loggator logExample("example", std::cout);
        logExample.setFormat("{TYPE}: {LINE:%s: }");
        logExample.debug() << "test1"; 
        logExample.info() << "test2";
        logExample.info("test3");
        logExample.info("%s", "test4");
        logExample.info("%s%i", "test", 5) << " extra.";
        logExample.LDEBUG() << "test6";
        logExample.LINFO() << "test7";
        logExample.LINFO("%s%i", "test", 8) << " extra."; // limited 19 args
        logExample.LINFOF("%s%i", "test", 9) << " extra."; // unlimited args
    }
    std::cout << std::endl;
    {
        Loggator logExample("example", std::cout);
        logExample.setFormat("{TYPE}: {LINE:%s: }");
        logExample("%s", "test1");
        logExample(eTypeLog::INFO) << "test2";
        logExample(eTypeLog::INFO, "test3");
        logExample(eTypeLog::INFO, "%s", "test4");
        logExample(eTypeLog::INFO, "%s%i", "test", 5) << " extra.";
        logExample << "test6";
        logExample[eTypeLog::INFO] << "test7";
    }
    });
    std::thread thread2([&]{
        Loggator &testSingleton2 = Loggator::getInstance("main2");
        testSingleton2 << "TEST: " << 2;
        Loggator testSingleton22 = Loggator::getInstance("main2");
        testSingleton22 << "TEST: " << 22;
    });
    std::thread thread3([&]{
        Loggator testSingleton3("test", Loggator::getInstance("main2"));
        testSingleton3 << "TEST: " << 3;
        Loggator testSingleton33("test", Loggator::getInstance("test"));
        testSingleton3 << "TEST: " << 33;
    });
    std::thread thread4([&]{
        Loggator testSingleton4(Loggator::getInstance("main2"));
        testSingleton4 << "TEST: " << 4;
        Loggator testSingleton44(testSingleton4);
        testSingleton44 << "TEST: " << 44;
    });

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();

    // main.fatal("test");

    // Loggator logTest;
    // logTest("cerr");
    // logTest.setOutStream(std::cout);
    // logTest("cout");
    // logTest.setName("new");
    // logTest("new name.");
    // logTest.setFormat("{TYPE} {TIME:%x %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{customKey:%s: }");
    // logTest("new format.");
    // logTest.setKey("customKey", "test");
    // logTest("new key.");
    // logTest.setKey("customKey", "");
    // logTest("empty key.");

    // logTest.send() << "Default send function";
    // logTest.send(eTypeLog::Info) << "Type send function";
    // logTest.send(eTypeLog::Info, "Type send function format without args");
    // logTest.send(eTypeLog::Info, "%s", "Type send function format");
    // logTest.send(eTypeLog::Info, "%s", "Type send function format") << " add";

    // logTest.LSEND() << "Default LSEND macro";
    // logTest.LSEND(Info) << "Type LSEND macro";
    // logTest.LSEND(Info, "Type LSEND macro format without args");
    // logTest.LSEND(Info, "%s", "Type LSEND macro format");
    // logTest.LSEND(Info, "%s", "Type LSEND macro format") << " add";

    // logTest.info() << "Default type function";
    // logTest.info("type function format without args");
    // logTest.info("%s", "type function format");
    // logTest.info("%s", "type function format") << " add";

    // logTest.LINFO() << "Default type macro";
    // logTest.LINFO("type macro format without args");
    // logTest.LINFO("%s", "type macro format");
    // logTest.LINFO("%s", "type macro format") << " add";

    // logTest("Default operator () format without args");
    // logTest("%s", "Default operator () format");
    // logTest(eTypeLog::Info, "Type operator () format without args");
    // logTest(eTypeLog::Info, "%s", "Type operator () format");

    // logTest("Default operator () format without args") << " add";
    // logTest("%s", "Default operator () format") << " add";
    // logTest(eTypeLog::Info) << "Type operator ()" << " add";
    // logTest(eTypeLog::Info, "Type operator () format without args") << " add";
    // logTest(eTypeLog::Info, "%s", "Type operator () format") << " add";

    // logTest << "Default operator <<" << " with multiple args";
    // logTest[eTypeLog::Info] << "operator [] test type info";

    // // test file

    // logTest.open("test1.log", std::ios::trunc);
    // logTest.setFilter(eFilterLog::EqualError);
    // logTest.LERROR() << "error test1";
    // logTest.LWARN() << "warn no output"; // no output

    // logTest.open("test2.log", std::ios::trunc); // close preview file
    // logTest.setFilter(eFilterLog::LessError);
    // logTest.LERROR() << "error no output"; // no output
    // logTest.LWARN() << "warn test2.log";
    // logTest.setFilter(eFilterLog::All);
    // logTest.LERROR() << "error test2.log";

    // Loggator logTestChild("", "test1.log"); // default open in append mode
    // logTestChild = logTest;
    // logTest.addChild(logTestChild);
    // logTest.listen(logTestChild);
    // logTest.LERROR() << "error test1 and test2";
    // logTestChild.setFilter(eFilterLog::EqualDebug);
    // logTest.LERROR() << "error only test2.log";
    // logTest.LDEBUG() << "debug test1 and test2";
    // logTestChild.LDEBUG() << "debug test1 and test2";

    // // test multi thread

    // logTest.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{THREAD_ID}: ");
    // logTestChild.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{THREAD_ID}: ");
    // std::thread thread[4];
    // for (int i = 0; i < 4; i++)
    // {
    //     thread[i] = std::thread([&](int t)
    //     {
    //         for (int j = 0; j < 5; j++)
    //             logTest << "test in thread" << t;
    //     }, i);
    // }
    // for (int i = 0; i < 4; i++)
    // {
    //     thread[i].join();
    // }

    // logTest.close();
    // logTestChild.close();

    // // look files

    // std::string line;

    // std::ifstream fileTestStream1("test1.log");
    // std::cout << "\ntest1.log" << std::endl;
    // while (std::getline(fileTestStream1, line))
    //     std::cout << line << std::endl;

    // std::ifstream fileTestStream2("test2.log");
    // std::cout << "\ntest2.log" << std::endl;
    // while (std::getline(fileTestStream2, line))
    //     std::cout << line << std::endl;

    return 0;
}