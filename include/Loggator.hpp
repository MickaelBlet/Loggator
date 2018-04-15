#ifndef _LOGGATOR_HPP_
# define _LOGGATOR_HPP_

#include <ctime>
#include <cmath>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>

namespace Loggator
{

enum eTypeLog
{
    Debug           = 1<<0,
    Trace           = 1<<1,
    Info            = 1<<2,
    Warning         = 1<<3,
    Error           = 1<<4
};

enum eFilterLog
{
    EqDebug         = eTypeLog::Debug,
    EqTrace         = eTypeLog::Trace,
    EqInfo          = eTypeLog::Info,
    EqWarning       = eTypeLog::Warning,
    EqError         = eTypeLog::Error,
    All             = EqDebug | EqTrace | EqInfo | EqWarning | EqError,
    SupEqDebug      = All,
    SupEqWarning    = EqWarning | EqError,
    SupEqInfo       = EqInfo | SupEqWarning,
    SupEqTrace      = EqTrace | SupEqInfo,
    LessEqTrace     = EqDebug | EqTrace,
    LessEqInfo      = LessEqTrace | EqInfo,
    LessEqWarning   = LessEqInfo | EqWarning,
    LessEqError     = All,
    SupDebug        = SupEqTrace,
    SupInfo         = SupEqWarning,
    SupWarning      = EqError,
    LessTrace       = EqDebug,
    LessInfo        = LessEqTrace,
    LessWarning     = LessEqInfo,
    LessError       = LessEqWarning
};

class Log
{

public:

    struct timeInfos
    {
        time_t timer;
        struct timespec ts;
    };

    struct SourceInfos
    {
        const char *filename;
        int         line;
        const char *func;
    };

    Log(void) :
    _filter(eFilterLog::All),
    _outStream(&std::cerr),
    _name(__func__),
    _countLock(0),
    _muted(false)
    {
        return ;
    }

    Log(const std::string &name, const std::string &filename, std::ios_base::openmode openMode = std::ios_base::app) :
    _filter(eFilterLog::All),
    _outStream(&std::cerr),
    _filename(filename),
    _fileStream(_filename, std::ios_base::out | openMode),
    _name(name),
    _countLock(0),
    _format("{type} {time} {name}: "),
    _timeFormat("%y/%m/%d %X.%N"),
    _muted(false)
    {
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        return ;
    }

    Log(std::ostream &oStream) :
    _filter(eFilterLog::All),
    _outStream(&std::cerr),
    _muted(false)
    {
        _outStream = &oStream;
        return ;
    }

    Log(Log &log) :
    _filter(log._filter),
    _outStream(log._outStream),
    _filename(log._filename),
    _name(log._name),
    _countLock(0),
    _format(log._format),
    _timeFormat(log._timeFormat),
    _muted(false)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(log._mutex);
        _logChilds = log._logChilds;
        return ;
    }

    void                setName(const std::string &name)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        _name = name;
    }

    void                setFilter(int filter)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        _filter = filter;
    }

    void                addFilter(int filter)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        _filter |= filter;
    }

    void                subFilter(int filter)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        if (_filter | filter)
            _filter -= filter;
    }

    Log                 &addChild(Log &log)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        _logChilds.insert(&log);
        return (*this);
    }

    Log                 &subChild(Log &log)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        _logChilds.erase(&log);
        return (*this);
    }

    Log                 &listen(Log &log)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(log._mutex);
        log._logChilds.insert(this);
        return (*this);
    }

    Log                 &unlisten(Log &log)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(log._mutex);
        log._logChilds.erase(this);
        return (*this);
    }

    void                setMuted(bool mute)
    {
        _muted = mute;
    }

    bool                isMuted(void)
    {
        return (_muted == true);
    }

    void                setFormat(const std::string &format)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        _format = format;
        std::size_t found = _format.find("{time");
        if (found != std::string::npos)
        {
            if (_format[found + 5] == '}')
            {
                _timeFormat = "%x %X.%N";
            }
            else if (_format[found + 5] == ':')
            {
                std::size_t start = found + 5;
                found = _format.find("}", found + 5);
                if (found != std::string::npos)
                {
                    std::size_t end = found;
                    _timeFormat = _format.substr(start + 1, end - start);
                    _format.erase(start, end - start);
                }
            }
        }
    }

    bool                isOpened(void)
    {
        return _fileStream.is_open();
    }

    std::string         formatTime(timeInfos &infos)
    {
        struct tm* tm_info;
        char bufferFormatTime[127];

        tm_info = localtime(&infos.timer);

        std::string retStr = _timeFormat;

        std::size_t findPos = retStr.find("%N");
        if (findPos != std::string::npos)
        {
            long microSec = 0;
            microSec = infos.ts.tv_nsec / 1000;
            snprintf(bufferFormatTime, 6, "%06ld", microSec);
            retStr.replace(findPos, 2, bufferFormatTime, 6);
        }
        strftime(bufferFormatTime, 126, retStr.c_str(), tm_info);

        return std::string(bufferFormatTime);
    }

    timeInfos            &getCurrentTimeInfos(void)
    {
        time_t timer;
        struct timespec ts;

        time(&timer);

        if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
            ts.tv_nsec = 0;

        _timeInfos = {timer, ts};

        return _timeInfos;
    }

    const char      *typeToStr(eTypeLog type)
    {
        switch (type)
        {
            case eTypeLog::Debug:
                return "DEBUG";
                break;
            case eTypeLog::Trace:
                return "TRACE";
                break;
            case eTypeLog::Info:
                return "INFO ";
                break;
            case eTypeLog::Warning:
                return "WARNG";
                break;
            case eTypeLog::Error:
                return "ERROR";
                break;
            default:
                return "";
                break;
        }
    }

    void            resetCache(void)
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        size_t nbUnlock = _countLock;
        _countLock = 0;
        _cacheStream.clear();
        _cacheStream.str(std::string());
        for (size_t i = 0; i < nbUnlock; i++)
            _mutex.unlock();
    }

    void            send(eTypeLog type, const SourceInfos &source = {nullptr, 0, nullptr})
    {
        std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
        size_t nbUnlock = _countLock;
        _countLock = 0;
        if (_muted == true)
        {
            _cacheStream.clear();
            _cacheStream.str(std::string());
            for (size_t i = 0; i < nbUnlock; i++)
                _mutex.unlock();
            return ;
        }
        std::string cacheStr = _cacheStream.str();
        if (_cacheStream.str().back() != '\n')
            cacheStr += "\n";
        getCurrentTimeInfos();
        if (_filter & type)
        {
            *(_outStream) << prompt(*this, type, _timeInfos, source) << cacheStr;
        }
        std::set<Log*> tmpsetLog;
        tmpsetLog.insert(this);
        sendChild(tmpsetLog, *this, type, _timeInfos, source, cacheStr);
        _cacheStream.clear();
        _cacheStream.str(std::string());
        *(_outStream) << std::flush;
        for (size_t i = 0; i < nbUnlock; i++)
            _mutex.unlock();
    }

    void            sendChild(std::set<Log*> &setLog, Log &log, eTypeLog type, timeInfos &timeInfos, const SourceInfos &source, const std::string &str)
    {
        for (Log *child : log._logChilds)
        {
            if (setLog.find(child) != setLog.end())
                continue;
            std::lock_guard<std::recursive_mutex> lockGuard(child->_mutex);
            setLog.insert(child);
            if (child->_muted == false && child->_filter & type)
            {
                *(child->_outStream) << child->prompt(log, type, timeInfos, source) << str;
                *(child->_outStream) << std::flush;
            }
            sendChild(setLog, *child, type, timeInfos, source, str);
        }
    }

    std::string     prompt(Log &log, eTypeLog type, timeInfos &timeInfos, const SourceInfos &source = {nullptr, 0, nullptr})
    {
        std::string prompt = _format;
        std::size_t found = prompt.find("{");
        while (found != std::string::npos)
        {
            std::size_t start = found;
            found = prompt.find("}", found);
            if (found == std::string::npos)
                break;
            std::size_t end = found;
            std::string key = prompt.substr(start, end - start + 1);
            if (key == "{time}")
            {
                prompt.replace(start, key.size(), formatTime(timeInfos));
            }
            else if (key == "{type}")
            {
                prompt.replace(start, key.size(), typeToStr(type));
            }
            else if (key == "{name}")
            {
                prompt.replace(start, key.size(), log._name);
            }
            else if (key == "{func}")
            {
                if (source.func != nullptr)
                    prompt.replace(start, key.size(), source.func);
                else
                    prompt.erase(start, key.size());
            }
            else if (key == "{path}")
            {
                if (source.filename != nullptr)
                    prompt.replace(start, key.size(), source.filename);
                else
                    prompt.erase(start, key.size());
            }
            else if (key == "{line}")
            {
                if (source.line > 0)
                    prompt.replace(start, key.size(), std::to_string(source.line));
                else
                    prompt.erase(start, key.size());
            }
            found = prompt.find("{", start + 1);
        }
        return prompt;
    }

    template<typename T>
    friend std::stringstream& operator<<(Log& log, const T& var)
    {
        log._mutex.lock();
        log._countLock++;
        log._cacheStream << var;
        return log._cacheStream;
    }

private:
    int                     _filter;
    std::ostream            *_outStream;
    std::string             _filename;
    std::ofstream           _fileStream;
    std::set<Log*>          _logChilds;
    std::stringstream       _cacheStream;
    std::string             _name;
    std::recursive_mutex    _mutex;
    size_t                  _countLock;
    std::string             _format;
    std::string             _timeFormat;
    timeInfos               _timeInfos;
    bool                    _muted;

};

} // end namespace

#define SOURCE_INFOS \
    {__FILE__, __LINE__, __func__}

#define LOGGATOR(_log, _type, _args) \
    if (_log.isMuted() == false) \
        _log << _args, _log.send(Loggator::_type, SOURCE_INFOS);

#endif // _LOGGER_HPP_