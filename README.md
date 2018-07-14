# Loggator

**Logger** for **C++11**.

## Log Types

8  reals types  (**Warn** and **Warning**, **Crit** and **Critical**, **Emerg** and **Emergency**) has the same type.

|Log Type|Functions|Macros
|--------|---------|------
|DEBUG   |debug|LDEBUG
|INFO    |info|LINFO
|WARN    |warn|LWARN
|WARNING |warning|LWARNING
|ERROR   |error|LERROR
|CRIT    |crit|LCRIT
|CRITICAL|critical|LCRITICAL
|ALERT|alert|LALERT
|EMERG|emerg|LEMERG
|EMERGENCY|emergency|LEMERGENCY
|FATAL|fatal|LFATAL

## Filters

|Log Type|DEBUG|INFO|WARN|ERROR|CRIT|ALERT|EMERG|FATAL
|-|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
|EQUAL_DEBUG|X| | | | | | | 
|EQUAL_INFO| |X| | | | | | 
|EQUAL_WARN| | |X| | | | | 
|EQUAL_WARNING| | |X| | | | | 
|EQUAL_ERROR| | | |X| | | | 
|EQUAL_CRIT| | | | |X| | | 
|EQUAL_CRITICAL| | | | |X| | | 
|EQUAL_ALERT| | | | | |X| | 
|EQUAL_EMERG| | | | | | |X| 
|EQUAL_EMERGENCY| | | | | | |X| 
|EQUAL_FATAL| | | | | | | |X
|GREATER_DEBUG| |X|X|X|X|X|X|X
|GREATER_INFO| | |X|X|X|X|X|X
|GREATER_WARN| | | |X|X|X|X|X
|GREATER_WARNING| | | |X|X|X|X|X
|GREATER_ERROR| | | | |X|X|X|X
|GREATER_CRIT| | | | | |X|X|X
|GREATER_CRITICAL| | | | | |X|X|X
|GREATER_ALERT| | | | | | |X|X
|GREATER_EMERG| | | | | | | |X
|GREATER_EMERGENCY| | | | | | | |X
|GREATER_FATAL| | | | | | | | 
|GREATER_EQUAL_DEBUG|X|X|X|X|X|X|X|X
|GREATER_EQUAL_INFO| |X|X|X|X|X|X|X
|GREATER_EQUAL_WARN| | |X|X|X|X|X|X
|GREATER_EQUAL_WARNING| | |X|X|X|X|X|X
|GREATER_EQUAL_ERROR| | | |X|X|X|X|X
|GREATER_EQUAL_CRIT| | | | |X|X|X|X
|GREATER_EQUAL_CRITICAL| | | | |X|X|X|X
|GREATER_EQUAL_ALERT| | | | | |X|X|X
|GREATER_EQUAL_EMERG| | | | | | |X|X
|GREATER_EQUAL_EMERGENCY| | | | | | |X|X
|GREATER_EQUAL_FATAL| | | | | | | |X
|LESS_DEBUG| | | | | | | | 
|LESS_INFO|X| | | | | | | 
|LESS_WARN|X|X| | | | | | 
|LESS_WARNING|X|X| | | | | | 
|LESS_ERROR|X|X|X| | | | | 
|LESS_CRIT|X|X|X|X| | | | 
|LESS_CRITICAL|X|X|X|X| | | | 
|LESS_ALERT|X|X|X|X|X| | | 
|LESS_EMERG|X|X|X|X|X|X| | 
|LESS_EMERGENCY|X|X|X|X|X|X| | 
|LESS_FATAL|X|X|X|X|X|X|X| 
|LESS_EQUAL_DEBUG|X| | | | | | | 
|LESS_EQUAL_INFO|X|X| | | | | | 
|LESS_EQUAL_WARN|X|X|X| | | | | 
|LESS_EQUAL_WARNING|X|X|X| | | | | 
|LESS_EQUAL_ERROR|X|X|X|X| | | | 
|LESS_EQUAL_CRIT|X|X|X|X|X| | | 
|LESS_EQUAL_CRITICAL|X|X|X|X|X| | | 
|LESS_EQUAL_ALERT|X|X|X|X|X|X| | 
|LESS_EQUAL_EMERG|X|X|X|X|X|X|X| 
|LESS_EQUAL_EMERGENCY|X|X|X|X|X|X|X| 
|LESS_EQUAL_FATAL|X|X|X|X|X|X|X|X
|ALL|X|X|X|X|X|X|X|X

Use bit composition (example):
```cpp
Loggator logExample(eFilterLog::EQUAL_DEBUG | eFilterLog::EQUAL_WARN | eFilterLog::EQUAL_FATAL);
logExample[eTypeLog::DEBUG] << "example Debug"; // OK
logExample[eTypeLog::INFO]  << "example Info";  //    KO
logExample[eTypeLog::WARN]  << "example Warn";  // OK
logExample[eTypeLog::ERROR] << "example Error"; //    KO
logExample[eTypeLog::CRIT]  << "example Crit";  //    KO
logExample[eTypeLog::ALERT] << "example Alert"; //    KO
logExample[eTypeLog::EMERG] << "example Emerg"; //    KO
logExample[eTypeLog::FATAL] << "example Fatal"; // OK
```

## Constructor

```cpp
/// name        : ""
/// filter      : ALL
/// outStream   : std::cerr
/// muted       : false
Loggator(void);

/// name        : ""
/// filter      : @param filter
/// outStream   : std::cerr
/// muted       : false
Loggator(int filter);

/// name        : @param name (if not empty add in static map for getInstance method)
/// fileOpen    : @param path + @param openMode
/// filter      : @param filter
/// outStream   : if file is_open <fileStream> else <std::cerr>
/// muted       : false
Loggator(const std::string &name, const std::string &path, std::ios::openmode openMode = std::ios::app, int filter = eFilterLog::ALL);

/// name        : @param name (if not empty add in static map for getInstance method)
/// filter      : @param filter
/// outStream   : @param oStream
/// muted       : false
Loggator(const std::string &name, std::ostream &oStream = std::cerr, int filter = eFilterLog::ALL);

/// name        : ""
/// filter      : @param filter
/// outStream   : @param oStream
/// muted       : false
Loggator(std::ostream &oStream, int filter = eFilterLog::ALL);

/// name        : @param name (if not empty add in static map for getInstance method)
/// filter      : @param loggator.filter
/// outStream   : nullptr
/// muted       : false
/// child       : @param loggator
Loggator(const std::string &name, Loggator &loggator);

/// name        : ""
/// filter      : @param loggator.filter
/// outStream   : nullptr
/// muted       : false
/// child       : @param loggator
Loggator(Loggator &loggator);
```

## Operator =
```cpp
/// filter      : @param rhs.filter
/// format      : @param rhs.format
/// muted       : @param rhs.muted
Loggator    &operator=(const Loggator &rhs);
```

## getInstance
```cpp
Loggator logExample("example", std::cout);
Loggator &refLogExample = Loggator::getInstance("example"); // OK
Loggator &badRefLogExample = Loggator::getInstance("test"); // throw runtime error
```

## setOutStream
```cpp
Loggator logExample("example");
logExample.setOutStream(std::cout);
```

## setName
```cpp
Loggator logExample("example");
logExample.setName("new name"); // if not empty add in static map for getInstance method
```

## setFilter, addFilter, subFilter
```cpp
Loggator logExample("example", std::cout);
logExample.setFilter(eFilterLog::EQUAL_INFO);
logExample.error("test1"); // no output
logExample.info("test2"); // output
logExample.addFilter(eFilterLog::EQUAL_ERROR);
logExample.error("test3"); // output
logExample.subFilter(eFilterLog::EQUAL_ERROR);
logExample.error("test4"); // no output
```

## addChild, subChild, listen, unlisten
```cpp
Loggator logExample1("example1", "example1.log", std::ios::trunc, eFilterLog::ALL);
Loggator logExample2("example2", "example2.log", std::ios::trunc, eFilterLog::EQUAL_INFO);
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

## setKeys
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

## setMuted
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE} {NAME:%s: }{customKey:%s: }");
logExample.warn("test1");
logExample.setMuted(true);
logExample.warn("test1"); // not write
// output:
// WARN  example: test1
```

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
|**TIME**|Time from `strftime`|`"{TIME:%x_%X.%N}"`|`01/01/70 01:23:45.678910`|
|**TYPE**|Type of log|`"{TYPE}"`|`DEBUG`/`INFO`/`WARN`/`ERROR`/`CRIT`/`ALERT`/`FATAL`/`EMERG`|
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

## isMuted
```cpp
Loggator logExample("example", std::cout);
logExample.isMuted(); // return false
logExample.setMuted(true);
logExample.isMuted(); // return true
```

## open
```cpp
Loggator logExample("example", std::cout);
logExample.open("test1.log"); // return true if open success else return false
```

## close
```cpp
Loggator logExample("example", "test.log");
logExample.close(); // close the file if filestream is open
```

## isOpen
```cpp
Loggator logExample("example");
logExample.isOpen(); // return true if file is open else return false
```

## Send
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE}: {LINE:%s: }");
logExample.send() << "test1";
logExample.send(eTypeLog::INFO) << "test2";
logExample.send(eTypeLog::INFO, "test3");
logExample.send(eTypeLog::INFO, "%s", "test4");
logExample.send(eTypeLog::INFO, "%s%i", "test", 5) << " extra.";
logExample.LSEND() << "test6";
logExample.LSEND(INFO) << "test7";
logExample.LSEND(INFO, "%s%i", "test", 8) << " extra."; // limited 19 args
logExample.LSENDF(INFO, "%s%i", "test", 9) << " extra."; // unlimited args
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

## Functions Macros type
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

## Operator [] () <<
```cpp
Loggator logExample("example", std::cout);
logExample.setFormat("{TYPE}: {LINE:%s: }");
logExample("%s", "test1");
logExample(eTypeLog::INFO) << "test2";
logExample(eTypeLog::INFO, "test3");
logExample(eTypeLog::INFO, "%s", "test4");
logExample(eTypeLog::INFO, "%s%i", "test", 5) << " extra.";
logExample << "test6";
logExample[eTypeLog::INFO] << "test7";
logExample << "test8" << eTypeLog::INFO;
logExample.LSEND() << "test9" << eTypeLog::INFO;
// output:
// DEBUG: test1
// INFO : test2
// INFO : test3
// INFO : test4
// INFO : test5 extra.
// DEBUG: test6
// INFO : test7
// INFO : test8
// INFO : 42: test9
```
