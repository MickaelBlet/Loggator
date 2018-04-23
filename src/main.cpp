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
    using namespace Log;

    test::test();
    Loggator log_Debug("Debug", "debug.log", std::ofstream::trunc);
    Loggator log_Trace("Trace", "trace.log", std::ofstream::trunc);
    Loggator log_Info("Info", "info.log", std::ofstream::trunc);
    Loggator log_Warning("Warng", "warning.log", std::ofstream::trunc);
    Loggator log_Error("Error", "error.log", std::ofstream::trunc);
    Loggator log_test;
    log_test.open("test.log");
    Loggator ll("ff");
    ll.addKey("test", "yolo");
    ll.setFormat("{TYPE} {TIME:%X.%N} {NAME:%-10s} {test:%-10s}: ");
    ll.SEND_FORMAT(Warning, "%s%i", "test", 6);
    ll.addKey("test", "yalaaa");
    ll.SEND_FORMAT(Warning, "%s%i", "test", 6);
    ll.addKey("test", "yuluuuu");
    ll.SEND_FORMAT(Warning, "test");
    // *(int*)0 = 0;

    log_test.setMuted(true);

    // LOGGATOR(log_test, Trace, "eioiwheov");
    log_test.addChild(log_Debug).addChild(log_Trace).addChild(log_Info).addChild(log_Warning).addChild(log_Error);

    // log_Debug.setFormat("{time:%Y/%m/%d %X.%N} ");
    log_Debug.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME:%-5s} {FUNC} {PATH}:{LINE:%-4s}: ");
    log_test.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME:%-5s} {FUNC} {PATH}:{LINE}: ");
    // Loggator::Log log_Global(log_Error);
    // log_1.add(log_2);
    // log_2.add(log_1);
    log_Debug.listen(log_Trace).listen(log_Info).listen(log_Warning).listen(log_Error);
    log_Trace.listen(log_Info).listen(log_Warning).listen(log_Error);
    log_Info.listen(log_Warning).listen(log_Error);
    log_Warning.listen(log_Error);

    log_test.setFilter(eFilterLog::All);
    log_Debug.setFilter(eFilterLog::All);
    log_Trace.setFilter(eFilterLog::SupEqTrace);
    log_Info.setFilter(eFilterLog::SupEqInfo);
    log_Warning.setFilter(eFilterLog::SupEqWarning);
    log_Error.setFilter(eFilterLog::EqError);
    log_Error.setFormat("{TIME:%Y/%m/%d %X} {NAME}: ");

    // Logg(log_test, eTypeLog::Debug, SOURCE_INFOS) << "test";
    // Logg(log_test, eTypeLog::Debug, SOURCE_INFOS) << "test";
    // Logg(log_test, eTypeLog::Debug, SOURCE_INFOS) << "test";
    // Logg(log_test, eTypeLog::Debug, SOURCE_INFOS) << "test";
    // Logg(log_test, eTypeLog::Debug, SOURCE_INFOS) << "test";
    // Logg(log_test, eTypeLog::Debug, SOURCE_INFOS) << "test";
    // Logg(log_test, eTypeLog::Debug, SOURCE_INFOS) << "test";
    // Logg(log_test, eTypeLog::Debug, SOURCE_INFOS) << "test";

    std::thread t1([&]{
        log_Debug.SEND(Debug) << "Debug";
        log_Trace.SEND(Debug) << "Debug";
        log_Info.SEND(Debug) << "Debug";
        log_Warning.SEND(Debug) << "Debug";
        log_Error.SEND(Debug) << "Debug";

        log_Debug.SEND(Trace) << "Trace";
        log_Trace.SEND(Trace) << "Trace";
        log_Info.SEND(Trace) << "Trace";
        log_Warning.SEND(Trace) << "Trace";
        log_Error.SEND(Trace) << "Trace";
    
        log_Debug.SEND(Info) << "Info";
        log_Trace.SEND(Info) << "Info";
        log_Info.SEND(Info) << "Info";
        log_Warning.SEND(Info) << "Info";
        log_Error.SEND(Info) << "Info";

        log_Debug.SEND(Warning) << "Warning";
        log_Trace.SEND(Warning) << "Warning";
        log_Info.SEND(Warning) << "Warning";
        log_Warning.SEND(Warning) << "Warning";
        log_Error.SEND(Warning) << "Warning";

        log_Debug.SEND(Error) << "Error";
        log_Trace.SEND(Error) << "Error";
        log_Info.SEND(Error) << "Error";
        log_Warning.SEND(Error) << "Error";
        log_Error.SEND(Error) << "Error";
    });

    std::thread t2([&]{
        log_Debug.SEND(Debug) << "Debug";
        log_Trace.SEND(Debug) << "Debug";
        log_Info.SEND(Debug) << "Debug";
        log_Warning.SEND(Debug) << "Debug";
        log_Error.SEND(Debug) << "Debug";

        log_Debug.SEND(Trace) << "Trace";
        log_Trace.SEND(Trace) << "Trace";
        log_Info.SEND(Trace) << "Trace";
        log_Warning.SEND(Trace) << "Trace";
        log_Error.SEND(Trace) << "Trace";
    
        log_Debug.SEND(Info) << "Info";
        log_Trace.SEND(Info) << "Info";
        log_Info.SEND(Info) << "Info";
        log_Warning.SEND(Info) << "Info";
        log_Error.SEND(Info) << "Info";

        log_Debug.SEND(Warning) << "Warning";
        log_Trace.SEND(Warning) << "Warning";
        log_Info.SEND(Warning) << "Warning";
        log_Warning.SEND(Warning) << "Warning";
        log_Error.SEND(Warning) << "Warning";

        log_Debug.SEND(Error) << "Error";
        log_Trace.SEND(Error) << "Error";
        log_Info.SEND(Error) << "Error";
        log_Warning.SEND(Error) << "Error";
        log_Error.SEND(Error) << "Error";
    });

    // std::thread t3([&]{
    //     // for (int i = 0 ; i < 10000; i++)
    //     // LOGGATOR(log_test, Debug, i);
    //         // for (int i = 20000 ; i < 30000; i++)
    //     // LOGGATOR(log_Debug, Debug, i);
    // LOGGATOR(log_test, Debug, "Debug" << 3);
    // LOGGATOR(log_test, Debug, "Debug");
    // LOGGATOR(log_test, Trace, "Trace");
    // LOGGATOR(log_test, Info, "Info");
    // LOGGATOR(log_test, Warning, "Warning");
    // LOGGATOR(log_test, Error, "Error");
    // });

    // std::thread t4([&]{
    //     // for (int i = 0 ; i < 10000; i++)
    //     // LOGGATOR(log_test, Debug, i);
    //         // for (int i = 30000 ; i < 40000; i++)
    //     // LOGGATOR(log_Debug, Debug, i);
    // LOGGATOR(log_test, Debug, "Debug");
    // LOGGATOR(log_test, Trace, "Trace");
    // LOGGATOR(log_test, Info, "Info");
    // LOGGATOR(log_test, Warning, "Warning");
    // LOGGATOR(log_test, Error, "Error");
    // });

    // std::thread t5([&]{
    //     Log log_test2(log_test);
    //     log_test2.setName("Newiugigi iug ");
    //     std::cout << "ici" <<std::endl;
    //     // for (int i = 0 ; i < 10000; i++)
    //     LOGGATOR(log_test2, Error, 0);
    //         // for (int i = 40000 ; i < 50000; i++)
    //     // LOGGATOR(log_Debug, Debug, i);
    // LOGGATOR(log_test, Debug, "Debug");
    // LOGGATOR(log_test, Trace, "Trace");
    // LOGGATOR(log_test, Info, "Info");
    // LOGGATOR(log_test, Warning, "Warning");
    // LOGGATOR(log_test, Error, "Errorest");
    // });
    
    t1.join();
    // std::cerr << "t1" << std::endl;
    t2.join();
    // // std::cerr << "t2" << std::endl;
    // t3.join();
    // // std::cerr << "t3" << std::endl;
    // t4.join();
    // // std::cerr << "t4" << std::endl;
    // t5.join();
    // // std::cerr << "t5" << std::endl;
    return 0;
}