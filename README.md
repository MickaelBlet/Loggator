# Loggator


This is a little **logger** for **c++11**.


## Log Types

4  reals types  **Warn** and **Warning** has the same type.

|Log Type|Debug|Info|Warn|Warning|Error|
|----------|-----|----|----|-------|-----|
|**Functions**|debug|info|warn|warning|error|
|**Macros**|LDEBUG|LINFO|LWARN|LWARNING|LERROR|



## Format
You can set a output format.
*Example:* [**setFormat**]
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE} {TIME:%x %X.%N}: {NAME:%s: }");
logExample("test");
// output:
// "DEBUG 01/01/70 01:23:45.678910: example: test"
```


|Key|Description|Exemple|Output
|---|-----------|-------|------|
|**TIME**|Time from `strftime`|`"{TIME:%x %X.%N}"`|`01/01/70 01:23:45.678910`|
|**TYPE**|Type of log|`"{TYPE}"`|`DEBUG`/`INFO`/`WARN`/`ERROR`|
|**NAME**|Name of loggator|`"{NAME}"`|`loggator`|
|**FUNC**|Name of scope function|`"{FUNC}"`|`main`|
|**PATH**|Path of source file|`"{PATH}"`|`src/main.cpp`|
|**FILE**|Filename of source file|`"{FILE}"`|`main.cpp`|
|**LINE**|Line at source file|`"{LINE}"`|`42`|
|**THREAD_ID**|Thread id|`"{THREAD_ID}"`|`7F4768D40700`|

*Advance Example:* [**setFormat**]
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{THREAD_ID}: ");
logExample.LINFO("test");
// output:
// "INFO  70/01/01 01:23:45.678910 example: main: main.cpp:42 : 7F4768D40700: test"
```
