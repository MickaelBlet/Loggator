# Loggator

**Logger** for **C++11**.

## Example

```cpp
#include "loggator.hpp"

using namespace Log;

void test()
{
    LOGGATOR("example") << "macro LOGGATOR";
    LOGGATOR("example").setName("main");
}

int main()
{
    Loggator logExample("example", std::cout);
    logExample.setFormat("{TYPE:%-5s} {TIME:%y/%m/%d %X.%N}: {NAME:%s: }{FILE:%s:}{LINE:%s: }");
    logExample.debug() << "function";
    logExample.debug(LOGGATOR_SOURCEINFOS) << "source infos macro";
    test();
    LOGGATOR("main", WARN) << "macro LOGGATOR with type" << std::endl;
    LOGGATOR(logExample, INFO, "style %s ", "printf") << "LOGGATOR";
    return (0);
}

// output:
// DEBUG 70/01/01 01:23:45.678910: example: function
// DEBUG 70/01/01 01:23:45.678910: example: main.cpp:16: source infos macro
// DEBUG 70/01/01 01:23:45.678910: example: macro LOGGATOR
// DEBUG 70/01/01 01:23:45.678910: main: main.cpp:18: macro LOGGATOR with type
// INFO  70/01/01 01:23:45.678910: main: main.cpp:19: style printf LOGGATOR
```

## Log Types

|Log Types            |methodes             |
|:-------------------:|:-------------------:|
|**DEBUG**            |debug()              |
|**INFO**             |info()               |
|**WARN / WARNING**   |warn() / warning()   |
|**ERROR**            |error()              |
|**CRIT / CRITICAL**  |crit() / critical()  |
|**ALERT**            |alert()              |
|**EMERG / EMERGENCY**|emerg() / emergency()|
|**FATAL**            |fatal()              |

## Custom Format

|Key          |Description            |Exemple          |Output|
|-------------|-----------------------|:---------------:|------|
|**TIME**     |Time from **strftime** |`{TIME:%x %X.%N}`|`01/01/70 01:23:45.678910`|
|**TYPE**     |Type of log            |`{TYPE}`         |`DEBUG`/`INFO`/`WARN`/`ERROR`/ `CRIT`/`ALERT`/`FATAL`/`EMERG`|
|**NAME**     |Name of loggator       |`{NAME}`         |`loggator`|
|**FUNC**     |Name of scope function |`{FUNC}`         |`main`|
|**PATH**     |Path of source file    |`{PATH}`         |`src/main.cpp`|
|**FILE**     |Filename of source file|`{FILE}`         |`main.cpp`|
|**LINE**     |Line at source file    |`{LINE}`         |`42`|
|**THREAD_ID**|Thread id              |`{THREAD_ID}`    |`7F4768D40700`|

## Filter

Use bit composition:
```cpp
Loggator logExample();
logExample.setFilter(eFilterLog::EQUAL_DEBUG | eFilterLog::EQUAL_WARN | eFilterLog::EQUAL_FATAL);
LOGGATOR(logExample, DEBUG) << "example Debug"; // OK
LOGGATOR(logExample, INFO)  << "example Info";  //    KO
LOGGATOR(logExample, WARN)  << "example Warn";  // OK
LOGGATOR(logExample, DEBUG) << "example Error"; //    KO
LOGGATOR(logExample, CRIT)  << "example Crit";  //    KO
LOGGATOR(logExample, ALERT) << "example Alert"; //    KO
LOGGATOR(logExample, EMERG) << "example Emerg"; //    KO
LOGGATOR(logExample, FATAL) << "example Fatal"; // OK
```

## Configuration FILE

loggator.ini
```ini
# comment
[Loggator:debug]
Filter  = "EQUAL_DEBUG"
Format  = "{TIME:%y/%m/%d %X.%N: }"
File    = "debug.log"
OpenMode= TRUNCATE
Child   = "test" # call loggator "test" when "debug" is call
Listen  = "test" # call loggator "debug" when "test" is call

[Loggator:test]
Filter  = INFO, DEBUG, WARN ; separator |,+
File    = "test.log"
OpenMode= "TRUNC"

[loggator:default]
Name    = "main" ; change name "default" to "main"
Child   = "test,debug" ; separator |,+
Mute    = true
```
main.cpp
```cpp
#include "loggator.hpp"

using namespace Log;

int main()
{
    if (Loggator::openConfig("loggator.ini") == false)
    {
        std::cerr << "Open LOGGATOR config Fail" << std::endl;
        return (1);
    }
    Loggator &mainLog = LOGGATOR("main");
    LOGGATOR(mainLog, DEBUG) << "(main)  => test => debug"; // main is mute
    LOGGATOR("test",  DEBUG) << "test    => debug";
    LOGGATOR("debug", DEBUG) << "debug   => test";
    LOGGATOR("main",  INFO)  << "(main)  => test => (debug)"; // debug has not good filter
    LOGGATOR("test",  INFO)  << "test    => (debug)";
    LOGGATOR("debug", INFO)  << "(debug) => test";
    return (0);
}

// debug.log:
// 70/01/01 01:23:45.678910: (main)  => test => debug"
// 70/01/01 01:23:45.678910: test    => debug
// 70/01/01 01:23:45.678910: debug   => test

// test.log:
// 70/01/01 01:23:45.678910 [DEBUG] main.cpp:13:main: main: (main)  => test => debug
// 70/01/01 01:23:45.678910 [DEBUG] main.cpp:14:main: test: test    => debug
// 70/01/01 01:23:45.678910 [DEBUG] main.cpp:15:main: debug: debug   => test
// 70/01/01 01:23:45.678910 [INFO ] main.cpp:16:main: main: (main)  => test => (debug)
// 70/01/01 01:23:45.678910 [INFO ] main.cpp:17:main: test: test    => (debug)
// 70/01/01 01:23:45.678910 [INFO ] main.cpp:18:main: debug: (debug) => test
```
