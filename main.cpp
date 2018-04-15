#include "Loggator.hpp"
#include <thread>
namespace test
{
    void test()
    {
std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
}

int     main(void)
{
    using namespace Loggator;

    test::test();
    Log log_Debug("Debug", "debug.log", std::ofstream::trunc);
    Log log_Trace("Trace", "trace.log", std::ofstream::trunc);
    Log log_Info("Info ", "info.log", std::ofstream::trunc);
    Log log_Warning("Warng", "warning.log", std::ofstream::trunc);
    Log log_Error("Error", "error.log", std::ofstream::trunc);
    Loggator::Log log_test;

    // log_test.setMuted(true);

    // LOGGATOR(log_test, Trace, "eioiwheov");
    log_test.addChild(log_Debug).addChild(log_Trace).addChild(log_Info).addChild(log_Warning).addChild(log_Error);

    // log_Debug.setFormat("{time:%Y/%m/%d %X.%N} ");
    log_Debug.setFormat("{type} {time:%Y/%m/%d %X.%N} {name} {func} {path}:{line}: ");
    // Loggator::Log log_Global(log_Error);
    // log_1.add(log_2);
    // log_2.add(log_1);
    log_Debug.listen(log_Trace).listen(log_Info).listen(log_Warning).listen(log_Error);
    log_Trace.listen(log_Info).listen(log_Warning).listen(log_Error);
    log_Info.listen(log_Warning).listen(log_Error);
    log_Warning.listen(log_Error);

    log_test.setFilter(eFilterLog::EqError);
    log_Debug.setFilter(eFilterLog::All);
    log_Trace.setFilter(eFilterLog::SupEqTrace);
    log_Info.setFilter(eFilterLog::SupEqInfo);
    log_Warning.setFilter(eFilterLog::SupEqWarning);
    log_Error.setFilter(eFilterLog::EqError);
    log_Error.setFormat("{time:%Y/%m/%d %X.%N} {name}: ");

    std::thread t1([&]{
        log_test << "test";
        log_test << "test";
        log_test << "test";
        log_test << "test";
        log_test << "test";
        log_test << "test";
        log_test << "test";
        // log_test.send(eTypeLog::Trace, SOURCE_INFOS);
    // LOGGATOR(log_test, Trace, "First");
    // LOGGATOR(log_test, Trace, "Debug*********");
    // for (int i = 0 ; i < 10000; i++)
        // LOGGATOR(log_Debug, Trace, i);
    // LOGGATOR(log_test, Trace, "********Debug");
        log_test.resetCache();
    });

    std::thread t2([&]{
        // for (int i = 0 ; i < 10000; i++)
        // LOGGATOR(log_test, Debug, i);
            // for (int i = 10000 ; i < 20000; i++)
        // LOGGATOR(log_Trace, Trace, i);
    LOGGATOR(log_test, Debug, "Debug" << 2);
    LOGGATOR(log_test, Debug, "Debug" << __PRETTY_FUNCTION__);
    LOGGATOR(log_test, Trace, "Trace");
    LOGGATOR(log_test, Info, "Info");
    LOGGATOR(log_test, Warning, "Warning");
    LOGGATOR(log_test, Error, "Error");
    });

    std::thread t3([&]{
        // for (int i = 0 ; i < 10000; i++)
        // LOGGATOR(log_test, Debug, i);
            // for (int i = 20000 ; i < 30000; i++)
        // LOGGATOR(log_Debug, Debug, i);
    LOGGATOR(log_test, Debug, "Debug" << 3);
    LOGGATOR(log_test, Debug, "Debug");
    LOGGATOR(log_test, Trace, "Trace");
    LOGGATOR(log_test, Info, "Info");
    LOGGATOR(log_test, Warning, "Warning");
    LOGGATOR(log_test, Error, "Error");
    });

    std::thread t4([&]{
        // for (int i = 0 ; i < 10000; i++)
        // LOGGATOR(log_test, Debug, i);
            // for (int i = 30000 ; i < 40000; i++)
        // LOGGATOR(log_Debug, Debug, i);
    LOGGATOR(log_test, Debug, "Debug");
    LOGGATOR(log_test, Trace, "Trace");
    LOGGATOR(log_test, Info, "Info");
    LOGGATOR(log_test, Warning, "Warning");
    LOGGATOR(log_test, Error, "Error");
    });

    std::thread t5([&]{
        Log log_test2(log_test);
        log_test2.setName("Newiugigi iug ");
        std::cout << "ici" <<std::endl;
        // for (int i = 0 ; i < 10000; i++)
        LOGGATOR(log_test2, Error, 0);
            // for (int i = 40000 ; i < 50000; i++)
        // LOGGATOR(log_Debug, Debug, i);
    LOGGATOR(log_test, Debug, "Debug");
    LOGGATOR(log_test, Trace, "Trace");
    LOGGATOR(log_test, Info, "Info");
    LOGGATOR(log_test, Warning, "Warning");
    LOGGATOR(log_test, Error, "Errorest");
    });
    
    t1.join();
    // std::cerr << "t1" << std::endl;
    t2.join();
    // std::cerr << "t2" << std::endl;
    t3.join();
    // std::cerr << "t3" << std::endl;
    t4.join();
    // std::cerr << "t4" << std::endl;
    t5.join();
    // std::cerr << "t5" << std::endl;
    return 0;
}