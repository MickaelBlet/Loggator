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

namespace Log
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

struct SourceInfos
{
    const char *filename;
    int         line;
    const char *func;
};

class Loggator
{

public:

    class SendFifo
    {

    public:

        SendFifo(Loggator &log, const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) noexcept: _log(log), _type(type), _sourceInfos(sourceInfos)
        {
            return ;
        }

        SendFifo(SendFifo&& log) noexcept: _log(log._log), _type(log._type), _sourceInfos(log._sourceInfos)
        {
            return ;
        }

        ~SendFifo() noexcept
        {
            std::string cacheStr = std::move(_cacheStream.str());
            if (cacheStr.back() != '\n')
                cacheStr += "\n";
            _log.send(cacheStr, _type, _sourceInfos);
            return ;
        }

        template<typename T>
        std::stringstream& operator<<(const T& var)
        {
            _cacheStream << var;
            return _cacheStream;
        }

    private:
        SendFifo(void) = delete;
        SendFifo(const SendFifo &) = delete;
        SendFifo &operator=(const SendFifo &) = delete;

        Loggator            &_log;
        const eTypeLog      &_type;
        const SourceInfos   &_sourceInfos;
        std::stringstream   _cacheStream;

    };

    struct timeInfos
    {
        time_t timer;
        struct timespec ts;
    };

    Loggator(void) noexcept:
    _name("unknown"),
    _filter(eFilterLog::All),
    _format("{type} {time} : "),
    _timeFormat("%y/%m/%d %X.%N"),
    _filename("unknown"),
    _outStream(&std::cerr),
    _muted(false)
    {
        return ;
    }

    Loggator(const std::string &name, const std::string &filename, std::ios::openmode openMode = std::ios::app) noexcept:
    _name(name),
    _filter(eFilterLog::All),
    _format("{type} {time} {name}: "),
    _timeFormat("%y/%m/%d %X.%N"),
    _filename(filename),
    _fileStream(_filename, std::ios::out | openMode),
    _outStream(&std::cerr),
    _muted(false)
    {
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        return ;
    }

    Loggator(std::ostream &oStream) noexcept:
    _name("unknown"),
    _filter(eFilterLog::All),
    _format("{type} {time} : "),
    _timeFormat("%y/%m/%d %X.%N"),
    _filename("unknown"),
    _outStream(&oStream),
    _muted(false)
    {
        return ;
    }

    Loggator(Loggator &loggator) noexcept:
    _name(loggator._name),
    _filter(loggator._filter),
    _format(loggator._format),
    _timeFormat(loggator._timeFormat),
    _filename(loggator._filename),
    _outStream(loggator._outStream),
    _muted(loggator._muted)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        _logChilds = loggator._logChilds;
        return ;
    }

    ~Loggator(void) noexcept
    {
        return ;
    }

    void                setName(const std::string &name)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _name = name;
    }

    void                setFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter = filter;
    }

    void                addFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter |= filter;
    }

    void                subFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_filter | filter)
            _filter -= filter;
    }

    Loggator            &addChild(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _logChilds.insert(&loggator);
        return (*this);
    }

    Loggator            &subChild(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _logChilds.erase(&loggator);
        return (*this);
    }

    Loggator            &listen(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        loggator._logChilds.insert(this);
        return (*this);
    }

    Loggator            &unlisten(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        loggator._logChilds.erase(this);
        return (*this);
    }

    void                setMuted(bool mute)
    {
        _muted = mute;
    }

    bool                isMuted(void) const
    {
        return (_muted == true);
    }

    void                setFormat(const std::string &format)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
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
                std::size_t indexStart = found + 5;
                std::size_t indexEnd = _format.find("}", found + 5);
                if (indexEnd != std::string::npos)
                {
                    _timeFormat = _format.substr(indexStart + 1, indexEnd - indexStart);
                    _format.erase(indexStart, indexEnd - indexStart);
                }
            }
        }
    }

    bool                isOpened(void) const
    {
        return _fileStream.is_open();
    }

    std::string         formatTime(timeInfos &infos) const
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

    timeInfos getCurrentTimeInfos(void) const
    {
        time_t timer;
        struct timespec ts;

        time(&timer);

        if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
            ts.tv_nsec = 0;

        return {timer, ts};
    }

    const char      *typeToStr(eTypeLog type) const
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

    void            send(const std::string &str, eTypeLog type, const SourceInfos &source = {nullptr, 0, nullptr})
    {
        if (_muted == true)
            return ;
        timeInfos timeInfos = getCurrentTimeInfos();
        if (_filter & type)
        {
            std::lock_guard<std::mutex> lockGuard(_mutex);
            *(_outStream) << prompt(*this, type, timeInfos, source) << str << std::flush;
        }
        std::set<Loggator*> tmpsetLog;
        tmpsetLog.insert(this);
        sendChild(tmpsetLog, *this, type, timeInfos, source, str);
    }

    void            sendChild(std::set<Loggator*> &setLog, Loggator &loggator, eTypeLog type, timeInfos &timeInfos, const SourceInfos &source, const std::string &str)
    {
        for (Loggator *child : loggator._logChilds)
        {
            if (setLog.find(child) != setLog.end())
                continue;
            setLog.insert(child);
            if (child->_muted == false && child->_filter & type)
            {
                std::lock_guard<std::mutex> lockGuard(child->_mutex);
                *(child->_outStream) << child->prompt(loggator, type, timeInfos, source) << str << std::flush;
            }
            sendChild(setLog, *child, type, timeInfos, source, str);
        }
    }

    std::string     prompt(Loggator &loggator, eTypeLog type, timeInfos &timeInfos, const SourceInfos &source = {nullptr, 0, nullptr}) const
    {
        std::string prompt = _format;
        std::size_t indexStart = prompt.find("{");
        while (indexStart != std::string::npos)
        {
            std::size_t indexEnd = prompt.find("}", indexStart);
            if (indexEnd == std::string::npos)
                break;
            std::string key = prompt.substr(indexStart, indexEnd - indexStart + 1);
            if (key == "{time}")
            {
                prompt.replace(indexStart, key.size(), formatTime(timeInfos));
            }
            else if (key == "{type}")
            {
                prompt.replace(indexStart, key.size(), typeToStr(type));
            }
            else if (key == "{name}")
            {
                prompt.replace(indexStart, key.size(), loggator._name);
            }
            else if (key == "{func}")
            {
                if (source.func != nullptr)
                    prompt.replace(indexStart, key.size(), source.func);
                else
                    prompt.erase(indexStart, key.size());
            }
            else if (key == "{path}")
            {
                if (source.filename != nullptr)
                    prompt.replace(indexStart, key.size(), source.filename);
                else
                    prompt.erase(indexStart, key.size());
            }
            else if (key == "{line}")
            {
                if (source.line > 0)
                    prompt.replace(indexStart, key.size(), std::to_string(source.line));
                else
                    prompt.erase(indexStart, key.size());
            }
            indexStart = prompt.find("{", indexStart + 1);
        }
        return prompt;
    }

    SendFifo send(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr})
    {
        return SendFifo(*this, type, sourceInfos);
    }

private:
    std::string             _name;
    int                     _filter;
    std::string             _format;
    std::string             _timeFormat;
    std::string             _filename;
    std::ofstream           _fileStream;
    std::ostream            *_outStream;
    std::set<Loggator*>     _logChilds;
    std::mutex              _mutex;
    bool                    _muted;

};

} // end namespace

# define NARGS_SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9, _0, N, ...) N
# define NARGS(...) NARGS_SEQ(__VA_ARGS__, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1)
# define PRIMITIVE_CAT(x, y) x ## y
# define CAT(x, y) PRIMITIVE_CAT(x, y)
# define PRE_NARGS(...) NARGS(__VA_ARGS__)
# define NOARGS() ,,,,,,,,,
# define MACRO_CHOOSER(macro_prefix, ...) CAT(macro_prefix, PRE_NARGS(NOARGS __VA_ARGS__ ()))

# define LOG(_log, _type) _log.send(Log::eTypeLog::_type, {__FILE__, __LINE__, __func__})
# define SEND(...) MACRO_CHOOSER(SEND_, __VA_ARGS__)(__VA_ARGS__)
# define SEND_0() send(Log::eTypeLog::Debug, SourceInfos{__FILE__, __LINE__, __func__})
# define SEND_1(_typeLog) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__})

#endif // _LOGGER_HPP_