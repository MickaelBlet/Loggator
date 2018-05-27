/**
 * @brief 
 * 
 * @file main.cpp
 * @author Blet Mickael
 * @date 2018-05-01
 */

#include "Loggator.hpp"
#include <chrono>

class Test
{
    public:
        Test(Log::Loggator &p_log)
        {
            i_log = p_log;
            i_log.setFormat(LDEFAULT_FORMAT);
            i_log.setOutStream(std::cout);
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
            i_log.LSEND() << "test9" << Log::eTypeLog::INFO;
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
    logg.setFormat("{TIME} {TYPE:[%5s]}: {testThreadKey} {THREAD_ID} {testMainKeyThread} {LINE:%s: }");
    logg.setKey("testMainKeyThread", "+-+");
    std::thread tthread[4];
    tthread[0] = std::thread([&]{
        logg.setKey("testThreadKey", "0");
        logg("%s,%i", "test", 42);
        logg();
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
        logg();
    });
    for (int nbThread = 0; nbThread < 4; ++nbThread)
    {
        tthread[nbThread].join();
    }

    return 0;
}