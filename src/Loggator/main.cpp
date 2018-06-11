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

template <typename T>
std::string TypeOfVariable(const T &var)
{
    std::string retStr;
    char *demange = abi::__cxa_demangle(typeid(var).name(), nullptr, nullptr, nullptr);
    retStr = demange;
    if (demange)
        std::free(demange);
    return retStr;
}

class Test
{
    public:
        Test(const Log::Loggator &p_log)
        {
            std::map<int, int> m;
            std::cout << TypeOfVariable(m) << std::endl;
            std::cout << type_name<decltype(m)>() << std::endl;
            std::cout << type_name<decltype(p_log)>() << std::endl;
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
            i_log[Log::eTypeLog::INFO] << "test7";
            i_log << "test8" << Log::eTypeLog::INFO;
            i_log.LSEND() << "test9" << Log::eTypeLog::INFO << " " << __PRETTY_FUNCTION__;
        }

        Log::Loggator i_log;
};

int     main(void)
{
    using namespace Log;

    {
        Loggator logExample(eFilterLog::EQUAL_DEBUG | eFilterLog::EQUAL_WARN | eFilterLog::EQUAL_FATAL);
        logExample[eTypeLog::DEBUG] << "example Debug"; // OK
        logExample[eTypeLog::INFO]  << "example Info";  //    KO
        logExample[eTypeLog::WARN]  << "example Warn";  // OK
        logExample[eTypeLog::ERROR] << "example Error"; //    KO
        logExample[eTypeLog::CRIT]  << "example Crit";  //    KO
        logExample[eTypeLog::ALERT] << "example Alert"; //    KO
        logExample[eTypeLog::EMERG] << "example Emerg"; //    KO
        logExample[eTypeLog::FATAL] << "example Fatal"; // OK
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
    logInfo.setMuted(true);
    logError.setMuted(true);

    std::thread thread[4];
    for (int nbThread = 0; nbThread < 4; ++nbThread)
    {
        thread[nbThread] = std::thread([&]{
            Loggator logThread("Thread" + std::to_string(nbThread), Loggator::getInstance("debug"));
        for (int i = 0; i < 1000; ++i)
        {
            logThread.LDEBUG("%i: ", __LINE__) << "DEBUG test !!!";
            logThread.LINFO() << "INFO test !!!";
            logThread.LERROR() << "ERROR test !!!";
        }
        });
    }
    for (int nbThread = 0; nbThread < 4; ++nbThread)
    {
        thread[nbThread].join();
    }

    Loggator logExample("example", std::cout);
    Test t(logExample);
    logExample.setFormat("{TIME:%S.%N} {TYPE:[%5s]}: {LINE:%s: }");
    logExample("%s\n", "test1");
    logExample(eTypeLog::INFO) << "test2\n";
    logExample(eTypeLog::WARN, "test3\n");
    logExample(eTypeLog::ERROR, "%s\n", "test4");
    logExample(eTypeLog::INFO, "%s%i", "test", 5) << " extra.\n";
    logExample();
    logExample << "test6\n";
    logExample[eTypeLog::INFO] << "test7\n";
    logExample << "test8\n" << eTypeLog::INFO;
    logExample.LSEND() << "test9\n" << eTypeLog::INFO;

    t.test();

    Loggator logg("main", std::cout);
    logg.setFormat("{TIME} {TYPE:[%5s]}: {NAME} {testThreadKey} {THREAD_ID} {testMainKeyThread} {FUNC}{LINE::%s: }");
    logg.setKey("testMainKeyThread", "+-+");
    Loggator::getInstance("main") << "no thread key";
    logg.setName("main2");
    Loggator::getInstance("main2") << "no thread key";
    logg.setName("main");
    Loggator::getInstance("main") << "no thread key";
    Loggator::getInstance("main") << "no thread key";
    Loggator::getInstance("main") << "no thread key";
    Loggator::getInstance("main") << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    logg << "no thread key";
    LOGGATOR() << "0";
    LOGGATOR("main") << "1";
    LOGGATOR("main", INFO) << "2";
    LOGGATOR("main", INFO, "test") << "3";
    std::thread tthread[4];
    tthread[0] = std::thread([&]{
        logg.setKey("testThreadKey", "0");
        logg.setKey("testMainKeyThread", "+++");
        logg("%s\n%i", "test", 42);
        logg() << TypeOfVariable(tthread);
        logg();
    });
    tthread[1] = std::thread([&]{
        logg.setKey("testThreadKey", "1");
        logg();
        logg();
        logg();
    });
    tthread[2] = std::thread([&]{
        logg.setKey("testThreadKey", "2");
        logg();
        logg();
        logg();
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

    return 0;
}