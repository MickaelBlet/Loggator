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

    std::thread thread[1];
    for (int nbThread = 0; nbThread < 1; ++nbThread)
    {
        thread[nbThread] = std::thread([&]{
            Loggator logThread("Thread" + std::to_string(nbThread), Loggator::getInstance("debug"));
        for (int i = 0; i < 10000; ++i)
        {
            logThread.LDEBUG() << "DEBUG test !!!";
            logThread.LINFO() << "INFO test !!!";
            logThread.LERROR() << "ERROR test !!!";
        }
        });
    }
    for (int nbThread = 0; nbThread < 1; ++nbThread)
    {
        thread[nbThread].join();
    }
    return 0;
}