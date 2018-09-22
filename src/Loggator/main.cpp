/**
 * @brief 
 * 
 * @file main.cpp
 * @author Blet Mickael
 * @date 2018-05-01
 */

#include "Loggator.hpp"
#include <chrono>
#include <cxxabi.h>
#include <map>

#define StructTest(name, a, b, c, d, e, f) struct name { a b; c d; e f;}

StructTest(foo,
std::string, name,
std::string, type,
std::string, test);

template <typename T>
std::string
type_name()
{
    using TR = typename std::remove_reference<T>::type;
    std::unique_ptr<char, void(*)(void*)> own (
    #ifndef _MSC_VER
        abi::__cxa_demangle (typeid(TR).name(), nullptr,
            nullptr, nullptr),
    #else
            nullptr,
    #endif
            std::free
    );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

#ifndef _MSC_VER
template <typename T>
std::string TypeToString(T * var, int del = 0)
{
    char *demange = abi::__cxa_demangle(typeid(var).name(), nullptr, nullptr, nullptr);
    std::string retStr = demange;
    if (demange)
        std::free(demange);
    return retStr.substr(0, retStr.size() - del);
}
#else
template <typename T>
std::string TypeToString(T _x var, int del = 0)
{
    char *demange = typeid(var).name()
    std::string retStr = demange;
    return retStr.substr(0, retStr.size() - del);
}
#endif

template <typename T>
std::string TypeToString(T &var)
{
    return TypeToString(&var, 1);
}

class Test
{
    public:
        Test(const Log::Loggator &p_log)
        {
            // std::map<int, int> **m = nullptr;
            std::cout << TypeToString(p_log) << std::endl;
            // std::cout << type_name<decltype(&std::endl)>() << std::endl;
            // std::cout << type_name<decltype()>() << std::endl;
            // std::cout << type_name<decltype(p_log)>() << std::endl;
            // std::cout << TypeOfVariable(p_log) << std::endl;
            i_log = p_log;
            i_log.setFormat(LDEFAULT_FORMAT);
            i_log.setOutStream(std::cout);
            i_log.LSEND() << Log::eTypeLog::INFO << __PRETTY_FUNCTION__;
        }

        void test(void) const
        {
            i_log("%s", "test1");
            i_log(Log::eTypeLog::INFO) << "test2";
            i_log(Log::eTypeLog::INFO, "test3");
            i_log(Log::eTypeLog::INFO, "%s", "test4");
            i_log(Log::eTypeLog::INFO, "%s%i", "test", 5) << " extra.";
            i_log << "test6";
            i_log(Log::eTypeLog::INFO) << "test7";
            i_log << "test8" << Log::eTypeLog::INFO;
            i_log << "test9" << LSOURCEINFOS << Log::eTypeLog::INFO << " " << __PRETTY_FUNCTION__;
        }

        Log::Loggator i_log;
};

int     main(void)
{
    using namespace Log;

    {
        Loggator logExample(eFilterLog::EQUAL_DEBUG | eFilterLog::EQUAL_WARN | eFilterLog::EQUAL_FATAL);
        logExample(eTypeLog::DEBUG) << "example Debug"; // OK
        logExample(eTypeLog::INFO)  << "example Info";  //    KO
        logExample(eTypeLog::WARN)  << "example Warn";  // OK
        logExample(eTypeLog::ERROR) << "example Error"; //    KO
        logExample(eTypeLog::CRIT)  << "example Crit";  //    KO
        logExample(eTypeLog::ALERT) << "example Alert"; //    KO
        logExample(eTypeLog::EMERG) << "example Emerg"; //    KO
        logExample(eTypeLog::FATAL) << "example Fatal"; // OK
    }
    // Loggator logDebug("debug", "Debug.log", std::ios::trunc, eFilterLog::GREATER_EQUAL_DEBUG);
    // Loggator logInfo( "info",  "Info.log",  std::ios::trunc, eFilterLog::GREATER_EQUAL_INFO);
    // Loggator logError("error", "Error.log", std::ios::trunc, eFilterLog::GREATER_EQUAL_ERROR);
    // logDebug.addChild(logInfo).listen(logInfo).addChild(logError).listen(logError);
    // // logDebug.setMuted(true);
    // logInfo.setMuted(true);
    // logError.setMuted(true);

    // std::thread thread[8];
    // for (int nbThread = 0; nbThread < 8; ++nbThread)
    // {
    //     thread[nbThread] = std::thread([&]{
    //         Loggator logThread("Thread" + std::to_string(nbThread), Loggator::getInstance("debug"));
    //     for (int i = 0; i < 10000; ++i)
    //     {
    //         logThread.LDEBUG() << "DEBUG test !!!";
    //         logThread.LINFO() << "INFO test !!!";
    //         logThread.LERROR() << "ERROR test !!!";
    //     }
    //     });
    // }
    // for (int nbThread = 0; nbThread < 8; ++nbThread)
    // {
    //     thread[nbThread].join();
    // }

    Loggator logDebug("debug", "Debug.log", std::ios::trunc, eFilterLog::GREATER_EQUAL_DEBUG);
    // logDebug.setFormat("{TIME:%S.%N}: ");
    Loggator logInfo( "info",  "Info.log",  std::ios::trunc, eFilterLog::GREATER_EQUAL_INFO);
    Loggator logError("error", "Error.log", std::ios::trunc, eFilterLog::GREATER_EQUAL_ERROR);
    logDebug.addChild(logInfo).listen(logInfo).addChild(logError).listen(logError);
    logInfo.addChild(logDebug).listen(logDebug).addChild(logError).listen(logError);
    logError.addChild(logDebug).listen(logDebug).addChild(logInfo).listen(logInfo);
    // logDebug.setMuted(true);
    // logInfo.setMuted(true);
    // logError.setMuted(true);

    std::thread thread[4];
    for (int nbThread = 0; nbThread < 4; ++nbThread)
    {
        thread[nbThread] = std::thread([&]{
            Loggator logThread("Thread" + std::to_string(nbThread), Loggator::getInstance("debug"));
        for (int i = 0; i < 1000; ++i)
        {
            logThread.LSEND(DEBUG) << "DEBUG test !!!";
            logThread.LSEND(INFO)  << "INFO  test !!!";
            logThread.LSEND(WARN)  << "WARN  test !!!";
            logThread.LSEND(ERROR) << "ERROR test !!!";
            logThread.LSEND(CRIT)  << "CRIT  test !!!";
            logThread.LSEND(ALERT) << "ALERT test !!!";
            logThread.LSEND(EMERG) << "EMERG test !!!";
            logThread.LSEND(FATAL) << "FATAL test !!!";
            logThread.flush();
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        });
    }
    for (int nbThread = 0; nbThread < 4; ++nbThread)
    {
        thread[nbThread].join();
    }

    Loggator logExample("example", std::cout);
    Test t(logExample);
    logExample.setFormat("{TIME:%N} ");
    logExample("%s", "test1") << std::endl;
    logExample(eTypeLog::INFO, LSOURCEINFOS) << "test2" << std::endl;
    logExample(eTypeLog::WARN, "test3") << std::endl;
    logExample(eTypeLog::ERROR, "%s", "test4") << std::endl;
    logExample(eTypeLog::INFO, LSOURCEINFOS, "%s%i", "test", 5) << " extra." << std::endl;
    logExample() << std::endl;
    logExample << "test6" << std::endl;
    logExample(eTypeLog::INFO) << "test7" << LSOURCEINFOS << std::endl;
    logExample << "test8" << eTypeLog::INFO << std::endl;
    logExample << "test9" << eTypeLog::INFO << LSOURCEINFOS << std::endl;
    logExample.LCRIT(std::string("test10")) << std::endl;

    t.test();

    Loggator logg("main", std::cout);
    Loggator logg4((const Loggator&)(logg));
    Loggator logg5((const Loggator&)(logg4));
    logg << "YOOOOOOO";
    // logg4.setName("kk");
    // logg4.setOutStream(std::cout);
    logg4.LSEND() << "YOOOOOOO";
    // logg5.setOutStream(std::cout);
    logg5.LSEND() << "YOOOOOOO";
    logg.setFormat("{TIME:%Y/%m/%d %X.%N} {TYPE:[%-5s]}: {NAME:\\{%6s\\}} {testThreadKey:<%.3s>} {testMainKeyThread:<%.3s>} {FILE:%s:}{FUNC:%s:}{LINE:%s: }");
    logg.setKey("testMainKeyThread", "+++");
    Loggator::getInstance("main") << "with instance";
    Loggator::getInstance("main") << "no thread key";
    Loggator::getInstance("main").setName("main2");
    Loggator::getInstance("main2") << "no thread key";
    Loggator::getInstance("main2").setName("main");
    Loggator::getInstance("main") << "no thread key";
    Loggator::getInstance("main") << "no thread key";
    Loggator::getInstance("main") << "no thread key";
    Loggator::getInstance("main") << "no thread key";
    Loggator::getInstance("main") << "without instance";
    // logg[eTypeLog::INFO][eTypeLog::DEBUG] << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    LOGGATOR() << "0";
    LOGGATOR("main") << "1";
    LOGGATOR("main").setKey("testMainKeyThread", "+-+++", true);
    LOGGATOR("main", INFO) << "2";
    LOGGATOR("main", WARNING, "test") << "3";
    std::string str = "6";
    LOGGATOR("main", INFO, "%s", str.c_str()) << "3";
        // logg(eTypeLog::ALERT)(42)(' ')(4.5);

        logg("|%s\n|%i", str.c_str(), 42) << "\n" << "|Youhou";
    Loggator logg2("main2", logg);

    logg.setKey("testThreadKey", "o");
    std::thread tthread[4];
    tthread[0] = std::thread([&]{
        logg.setKey("testThreadKey", "0");
        logg.setKey("testMainKeyThread", "+++");
        logg() << TypeToString(tthread);
        logg(eTypeLog::ALERT, str);
    });
    tthread[1] = std::thread([&]{
        logg.setKey("testThreadKey", "1");
        logg();
        logg();
        logg();
    });
    tthread[2] = std::thread([&]{
        logg2.setKey("testThreadKey", "2");
        logg2();
        logg2();
        logg2();
    });
    tthread[3] = std::thread([&]{
        logg.setKey("testThreadKey", "3");
        logg();
        logg();
        logg.LINFO() << __PRETTY_FUNCTION__;
    });
    for (int nbThread = 0; nbThread < 4; ++nbThread)
    {
        tthread[nbThread].join();
    }
    LOGGATOR("main", INFO) << "brac";
    LOGGATOR("main", INFO) << "brac";
    LOGGATOR("main", INFO) << "brac";
    LOGGATOR("main", INFO) << "brac";
    LOGGATOR("main", INFO) << "brac";
    LOGGATOR("main").LINFO() << "test";
    LOGGATOR("main").LINFO() << "test";
    LOGGATOR("main").LINFO() << "test";
    LOGGATOR("main").LINFO() << "test";
    LOGGATOR("main").LINFO() << "test";
    LOGGATOR("main").LINFO("-est");
    LOGGATOR("main").LINFO("-est");
    LOGGATOR("main").LINFO("-est");
    LOGGATOR("main").LINFO("-est");
    Loggator &ttt = LOGGATOR("main");
    ttt.LINFO() << "2 test";
    ttt.LINFO() << "2 test";
    ttt.LINFO() << "2 test";
    ttt.LINFO() << "2 test";
    ttt.LINFO() << "2 test";
    ttt.LINFO() << "2 test";
    ttt.LINFO() << "2 test";
    ttt.LINFO() << "2 test";
    LOGGATOR("main").LINFO("-est");
    // *(int*)0 = 0;
    // for (int i=0;i<100000;i++)
        // logg();
    return 0;
}