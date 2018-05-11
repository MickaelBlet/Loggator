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
    try
    {
        Loggator &testTry = Loggator::getInstance("main2");
        (void)testTry;
    }
    catch (std::exception &e)
    {
        Loggator::getInstance("main") << eTypeLog::ERROR << e.what();
    }
    Loggator main3("main2", main);

    std::thread thread1([&]{
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
        logExample1.crit("test2");
        logExample1.fatal("test2");
        logExample1.alert("test2");
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
    Loggator logExample("example", Loggator::getInstance("main2"));
    logExample  << eTypeLog::ERROR << "error"
                << eTypeLog::FATAL << "fatal"
                << eTypeLog::EMERG << "emerg";
    logExample[eTypeLog::CRITICAL] << std::flush << std::hex << 66;
    logExample << std::endl;
    for (int i = 0; i < 10; i++)
    {
        logExample.LDEBUG() << i;
    }
    });
    std::thread thread2([&]{
        Loggator &testSingleton2 = Loggator::getInstance("main2");
        testSingleton2 << "TEST: " << 2;
        Loggator testSingleton22;
        testSingleton22 = testSingleton2;
        testSingleton22 << "TEST: " << 22;
    });
    std::thread thread3([&]{
        Loggator testSingleton3("test", Loggator::getInstance("main2"));
        testSingleton3 << "TEST: " << 3;
        Loggator testSingleton33("test", Loggator::getInstance("test"));
        testSingleton33 << "TEST: " << 33;
    });
    std::thread thread4([&]{
        Loggator testSingleton4(Loggator::getInstance("main2"));
        testSingleton4 << "TEST: " << 4;
        Loggator testSingleton44(testSingleton4);
        testSingleton44 << "TEST: " << 44;
        testSingleton44 << eTypeLog::ERROR << "TEST: " << 44;
    });

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();

    main.critical("%s", "aaaahhhhh!");

    main.critical(42);

    Loggator::getInstance("main")[eTypeLog::ERROR]("%s: %i", "yolo", 42)[eTypeLog::EMERGENCY] << " " << "yulu";

    return 0;
}