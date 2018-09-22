# Loggator

**Logger** for **C++11**.

## Log Types

|Log Types|DEBUG |INFO |WARNING |ERROR |CRITICAL |ALERT |EMERGENCY |FATAL
|---------|:----:|:---:|:------:|:----:|:-------:|:----:|:--------:|:----:
|Function |debug |info |warning |error |critical |alert |emergency |fatal
|Macro    |LDEBUG|LINFO|LWARNING|LERROR|LCRITICAL|LALERT|LEMERGENCY|LFATAL

## Custom Format

Set output format [**setFormat**]:
```cpp
#include "Loggator.hpp"

using namespace Log;

int main()
{
    Loggator logExample("example", std::cout);
    logExample.setFormat("{TYPE:%-5s} {TIME:%x %X.%N}: {NAME:%s: }{FILE:%s:}{LINE:%s: }");
    logExample.debug() << "function";
    logExample.LDEBUG() << "macro";
    return (0);
}

// output:
// DEBUG 01/01/70 01:23:45.678910: example: function
// DEBUG 01/01/70 01:23:45.678910: example: main.cpp:9: macro
```

|Key|Description|Exemple|Output
|---|-----------|:-----:|------|
|**TIME**|Time from **strftime**|`{TIME:%x %X.%N}`|`01/01/70 01:23:45.678910`|
|**TYPE**|Type of log|`{TYPE}`|`DEBUG`/`INFO`/`WARN`/`ERROR`/ `CRIT`/`ALERT`/`FATAL`/`EMERG`|
|**NAME**|Name of loggator|`{NAME}`|`loggator`|
|**FUNC**|Name of scope function|`{FUNC}`|`main`|
|**PATH**|Path of source file|`{PATH}`|`src/main.cpp`|
|**FILE**|Filename of source file|`{FILE}`|`main.cpp`|
|**LINE**|Line at source file|`{LINE}`|`42`|
|**THREAD_ID**|Thread id|`{THREAD_ID}`|`7F4768D40700`|

## Filter

Use bit composition (example):
```cpp
Loggator logExample(eFilterLog::EQUAL_DEBUG | eFilterLog::EQUAL_WARN | eFilterLog::EQUAL_FATAL);
logExample(eTypeLog::DEBUG) << "example Debug"; // OK
logExample(eTypeLog::INFO)  << "example Info";  //    KO
logExample(eTypeLog::WARN)  << "example Warn";  // OK
logExample(eTypeLog::ERROR) << "example Error"; //    KO
logExample(eTypeLog::CRIT)  << "example Crit";  //    KO
logExample(eTypeLog::ALERT) << "example Alert"; //    KO
logExample(eTypeLog::EMERG) << "example Emerg"; //    KO
logExample(eTypeLog::FATAL) << "example Fatal"; // OK
```

## Wiki

[Loggator Wiki](https://github.com/MickaelBlet/Loggator/wiki)
