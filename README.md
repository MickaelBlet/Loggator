# Loggator


**Logger** for **c++11**.


## Log Types

4  reals types  **Warn** and **Warning** has the same type.

|Log Type|Debug|Info|Warn|Warning|Error|
|----------|-----|----|----|-------|-----|
|**Functions**|debug|info|warn|warning|error|
|**Macros**|LDEBUG|LINFO|LWARN|LWARNING|LERROR|



## Format
Set a output format.
[**setFormat**]:
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE} {TIME:%x %X.%N}: {NAME:%s: }");
logExample("test");
// output:
// DEBUG 01/01/70 01:23:45.678910: example: test
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

Set with printf format the *output* of **keys**

No print *empty* **keys**
[**setFormat**]:
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE} {TIME:%Y/%m/%d %X.%N} {NAME}: {FUNC:%s: }{FILE:%s:}{LINE:%-3s: }{THREAD_ID}: ");
logExample.LINFO("test1");
logExample.info("test2");
// output:
// INFO  70/01/01 01:23:45.678910 example: main: main.cpp:42 : 7F4768D40700: test1
// INFO  70/01/01 01:23:45.678910 example: 7F4768D40700: test1
```
#### Custom Keys
[**setKey**]:
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE} {NAME:%s: }{customKey:%s: }");
logExample.warn("test1");
logExample.setKey("customKey", "myKey");
logExample.warning("test2");
logExample.setName("");
logExample.error("test3");
// output:
// WARN  example: test1
// WARN  example: myKey: test2
// ERROR myKey: test3
```
#### Filter
[**setFilter**, **addFilter**, **subFilter**]:
```cpp
Loggator logExample("example", std::cout);
logExample.setFilter(eFilterLog::EqualInfo);
logExample.error("test1"); // no output
logExample.info("test2"); // output
logExample.addFilter(eFilterLog::EqualError);
logExample.error("test3"); // output
logExample.subFilter(eFilterLog::EqualError);
logExample.error("test4"); // no output
```
#### Child
[**addChild**, **subChild**, **listen**, **unlisten**]:
```cpp
Loggator logExample1("example1", "example1.log", std::ios::trunc, eFilterLog::All);
Loggator logExample2("example2", "example2.log", std::ios::trunc, eFilterLog::EqualInfo);
logExample2.setFormat("{TIME:%X.%N} {NAME}: ");
logExample1.addChild(logExample2);
logExample1.info("test1");
logExample1.warn("test2");
logExample1.subChild(logExample2);
logExample1.info("test3");
logExample2.listen(logExample1);
logExample1.info("test4");
logExample2.unlisten(logExample1);
logExample1.info("test5");
logExample2.info("test6");

// example1.log
// INFO  70/01/01 01:23:45.678910 example1: test1
// WARN  70/01/01 01:23:45.678920 example1: test2
// INFO  70/01/01 01:23:45.678930 example1: test3
// INFO  70/01/01 01:23:45.678940 example1: test4
// INFO  70/01/01 01:23:45.678950 example1: test5

// example2.log
// 01:23:45.678910 example1: test1
// 01:23:45.678940 example1: test4
// 01:23:45.678960 example2: test6
```
#### Send
[**send**]:
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE}: {LINE:%s: }");
logExample.send() << "test1";
logExample.send(eTypeLog::Info) << "test2";
logExample.send(eTypeLog::Info, "test3");
logExample.send(eTypeLog::Info, "%s", "test4");
logExample.send(eTypeLog::Info, "%s%i", "test", 5) << " extra.";
logExample.LSEND() << "test6";
logExample.LSEND(Info) << "test7";
logExample.LSEND(Info, "%s%i", "test", 8) << " extra."; // limited 19 args
logExample.LSENDF(Info, "%s%i", "test", 9) << " extra."; // unlimited args
// output:
// DEBUG: test1
// INFO : test2
// INFO : test3
// INFO : test4
// INFO : test5 extra.
// DEBUG: 42: test6
// INFO : 43: test7
// INFO : 44: test8 extra.
// INFO : 45: test9 extra.
```
#### Functions Macros type
[**debug**, **LDEBUG**, **info**, **LINFO**, **warn**, **LWARN**, **warning**, **LWARNING**, **error**, **LERROR**]:
```cpp
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
// output:
// DEBUG: test1
// INFO : test2
// INFO : test3
// INFO : test4
// INFO : test5 extra.
// DEBUG: 42: test6
// INFO : 43: test7
// INFO : 44: test8 extra.
// INFO : 45: test9 extra.
```
#### Operator [] ()
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE}: {LINE:%s: }");
logExample("%s", "test1");
logExample(eTypeLog::Info) << "test2";
logExample(eTypeLog::Info, "test3");
logExample(eTypeLog::Info, "%s", "test4");
logExample(eTypeLog::Info, "%s%i", "test", 5) << " extra.";
logExample << "test6";
logExample[eTypeLog::Info] << "test7";
// output:
// DEBUG: test1
// INFO : test2
// INFO : test3
// INFO : test4
// INFO : test5 extra.
// DEBUG: test6
// INFO : test7
```