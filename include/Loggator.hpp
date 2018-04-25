#ifndef _LOGGATOR_HPP_
# define _LOGGATOR_HPP_

#include <ctime>
#include <cmath>
#include <cstdarg>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <map>

# define FORMAT_BUFFER_SIZE 1024

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

        SendFifo(Loggator &loggator, const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) noexcept:
        _log(loggator),
        _type(type),
        _sourceInfos(sourceInfos)
        {
            return ;
        }

        SendFifo(SendFifo &&sendFifo) noexcept:
        _log(sendFifo._log),
        _type(sendFifo._type),
        _sourceInfos(sendFifo._sourceInfos),
        _cacheStream(std::move(sendFifo._cacheStream))
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
        std::ostringstream& operator<<(const T& var)
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
        std::ostringstream  _cacheStream;

    };

    struct timeInfos
    {
        struct tm*  tm;
        long        msec;
    };

    Loggator(void) noexcept:
    _name("unknown"),
    _filter(eFilterLog::All),
    _format("{TYPE} {TIME} : "),
    _outStream(&std::cerr),
    _muted(false)
    {
        _mapFormatKey["{TIME}"] = "%y/%m/%d %X.%N";
        return ;
    }

    Loggator(const std::string &name, const std::string &path, std::ios::openmode openMode = std::ios::app) noexcept:
    _name(name),
    _filter(eFilterLog::All),
    _format("{TYPE} {TIME} {NAME}: "),
    _fileStream(path, std::ios::out | openMode),
    _outStream(&std::cerr),
    _muted(false)
    {
        _mapFormatKey["{TIME}"] = "%y/%m/%d %X.%N";
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        return ;
    }

    Loggator(const std::string &name, std::ostream &oStream = std::cerr) noexcept:
    _name(name),
    _filter(eFilterLog::All),
    _format("{TYPE} {TIME} : "),
    _outStream(&oStream),
    _muted(false)
    {
        _mapFormatKey["{TIME}"] = "%y/%m/%d %X.%N";
        return ;
    }

    Loggator(std::ostream &oStream) noexcept:
    _name("unknown"),
    _filter(eFilterLog::All),
    _format("{TYPE} {TIME} : "),
    _outStream(&oStream),
    _muted(false)
    {
        _mapFormatKey["{TIME}"] = "%y/%m/%d %X.%N";
        return ;
    }

    Loggator(const std::string &name, Loggator &loggator) noexcept:
    _name(name),
    _filter(loggator._filter),
    _format(loggator._format),
    _outStream(nullptr),
    _muted(loggator._muted)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        _logChilds = loggator._logChilds;
        _mapFormatKey = loggator._mapFormatKey;
        _logChilds.insert(&loggator);
        return ;
    }

    Loggator(Loggator &loggator) noexcept:
    _name(loggator._name),
    _filter(loggator._filter),
    _format(loggator._format),
    _outStream(nullptr),
    _muted(loggator._muted)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        _logChilds = loggator._logChilds;
        _mapFormatKey = loggator._mapFormatKey;
        _logChilds.insert(&loggator);
        return ;
    }

    Loggator    &operator=(const Loggator &rhs)
    {
        this->_name = rhs._name;
        this->_filter = rhs._filter;
        this->_format = rhs._format;
        this->_outStream = rhs._outStream;
        this->_muted = rhs._muted;
        std::lock_guard<std::mutex> lockGuard(rhs._mutex);
        this->_logChilds = rhs._logChilds;
        this->_mapFormatKey = rhs._mapFormatKey;
        return *this;
    }

    ~Loggator(void) noexcept
    {
        return ;
    }

    void            setName(const std::string &name)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _name = name;
    }

    void            setFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter = filter;
    }

    void            addFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter |= filter;
    }

    void            subFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_filter | filter)
            _filter -= filter;
    }

    Loggator        &addChild(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _logChilds.insert(&loggator);
        return (*this);
    }

    Loggator        &subChild(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _logChilds.erase(&loggator);
        return (*this);
    }

    Loggator        &listen(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        loggator._logChilds.insert(this);
        return (*this);
    }

    Loggator        &unlisten(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        loggator._logChilds.erase(this);
        return (*this);
    }

    Loggator        &addKey(const std::string &key, const std::string &value)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _mapCustomKey["{" + key + "}"] = value;
        return (*this);
    }

    void            setMuted(bool mute)
    {
        _muted = mute;
    }

    void            setFormat(const std::string &format)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _format = format;
        std::size_t indexStart = _format.find("{");
        while (indexStart != std::string::npos)
        {
            std::size_t indexEnd = _format.find("}", indexStart);
            if (indexEnd == std::string::npos)
                break;
            std::size_t indexFormat = _format.find(":", indexStart);
            if (indexFormat == std::string::npos || indexFormat > indexEnd)
            {
                indexStart = _format.find("{", indexStart + 1);
                continue;
            }
            std::string key = _format.substr(indexStart, indexFormat - indexStart) + "}";
            std::string formatKey = _format.substr(indexFormat + 1, indexEnd - indexFormat - 1);
            _mapFormatKey[key] = formatKey;
            _format.erase(indexFormat, indexEnd - indexFormat);
            indexStart = _format.find("{", indexStart + 1);
        }
    }

    bool            isMuted(void) const
    {
        return (_muted == true);
    }

    bool            open(const std::string &path, std::ios::openmode openMode = std::ios::app)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_fileStream.is_open())
            _fileStream.close();
        _fileStream.open(path, std::ios::out | openMode);
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        return _fileStream.is_open();
    }

    void            close(void)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_fileStream.is_open())
            _fileStream.close();
        _outStream = &std::cerr;
    }

    bool            isOpened(void) const
    {
        return _fileStream.is_open();
    }

    std::string     formatTime(timeInfos &infos) const
    {
        char bufferFormatTime[127];

        std::string retStr = _mapFormatKey.at("{TIME}");
        if (retStr.empty())
            retStr = "%y/%m/%d %X.%N";

        std::size_t findPos = retStr.find("%N");
        if (findPos != std::string::npos)
        {
            snprintf(bufferFormatTime, 6, "%06ld", infos.msec);
            retStr.replace(findPos, 2, bufferFormatTime, 6);
        }
        strftime(bufferFormatTime, 126, retStr.c_str(), infos.tm);
        return std::string(bufferFormatTime);
    }

    timeInfos       getCurrentTimeInfos(void) const
    {
        time_t timer;
        struct tm* tm_info;
        struct timespec ts;

        time(&timer);

        tm_info = localtime(&timer);

        if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
            ts.tv_nsec = 0;

        return {tm_info, ts.tv_nsec / 1000};
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
                return "WARN ";
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
        timeInfos timeInfos = getCurrentTimeInfos();
        if (_outStream != nullptr && _muted == false && _filter & type)
        {
            std::lock_guard<std::mutex> lockGuard(_mutex);
            *(_outStream) << prompt(*this, type, _mapCustomKey, timeInfos, source) << str << std::flush;
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
                std::lock_guard<std::mutex> lockGuardMain(_mutex);
                std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
                *(child->_outStream) << child->prompt(loggator, type, _mapCustomKey, timeInfos, source) << str << std::flush;
            }
            sendChild(setLog, *child, type, timeInfos, source, str);
        }
    }

    const std::string &customKey(const std::map<const std::string, std::string> &mapCustomKey, const std::string &key) const
    {
        std::map<const std::string, std::string>::const_iterator itMap = mapCustomKey.find(key);
        if (itMap == mapCustomKey.end())
            return _empty;
        return itMap->second;
    }

    std::string     formatKey(const std::string &keyFormat, const std::string &key) const
    {
        if (key.empty())
            return _empty;
        std::map<const std::string, std::string>::const_iterator itMap = _mapFormatKey.find(keyFormat);
        if (itMap == _mapFormatKey.end())
            return key;
        char buffer[127];
        sprintf(buffer, itMap->second.c_str(), key.c_str());
        return std::string(buffer);
    }

    std::string     prompt(Loggator &loggator, eTypeLog type, const std::map<const std::string, std::string> &mapCustomKey, timeInfos &timeInfos, const SourceInfos &source = {nullptr, 0, nullptr}) const
    {
        std::string prompt = _format;
        std::size_t indexStart = prompt.find("{");
        while (indexStart != std::string::npos)
        {
            std::size_t indexEnd = prompt.find("}", indexStart);
            if (indexEnd == std::string::npos)
                break;
            std::string key = prompt.substr(indexStart, indexEnd - indexStart + 1);
            if (key == "{TIME}")
            {
                prompt.replace(indexStart, key.size(), formatTime(timeInfos));
            }
            else if (key == "{TYPE}")
            {
                prompt.replace(indexStart, key.size(), formatKey(key, typeToStr(type)));
            }
            else if (key == "{NAME}")
            {
                prompt.replace(indexStart, key.size(), formatKey(key, loggator._name));
            }
            else if (key == "{FUNC}")
            {
                if (source.func != nullptr)
                    prompt.replace(indexStart, key.size(), formatKey(key, source.func));
                else
                    prompt.erase(indexStart, key.size());
            }
            else if (key == "{PATH}")
            {
                if (source.filename != nullptr)
                    prompt.replace(indexStart, key.size(), formatKey(key, source.filename));
                else
                    prompt.erase(indexStart, key.size());
            }
            else if (key == "{LINE}")
            {
                if (source.line > 0)
                    prompt.replace(indexStart, key.size(), formatKey(key, std::to_string(source.line)));
                else
                    prompt.erase(indexStart, key.size());
            }
            else
            {
                prompt.replace(indexStart, key.size(), formatKey(key, customKey(mapCustomKey, key)));
            }
            indexStart = prompt.find("{", indexStart);
        }
        return prompt;
    }

    SendFifo        send(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr})
    {
        return SendFifo(*this, type, sourceInfos);
    }

    SendFifo        send(const eTypeLog &type, const SourceInfos &sourceInfos, const char *format, ...)
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, type, sourceInfos);
        fifo << buffer;
        return fifo;
    }

    template<typename T>
    friend SendFifo operator<<(Loggator &loggator, const T& var)
    {
        SendFifo fifo(loggator, eTypeLog::Debug, {nullptr, 0, nullptr});
        fifo << var;
        return fifo;
    }

    SendFifo operator[](const eTypeLog &type)
    {
        return SendFifo(*this, type, {nullptr, 0, nullptr});
    }

    SendFifo operator()(const eTypeLog &type, const char * format, ...)
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, type, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

private:
    const std::string                           _empty;
    std::string                                 _name;
    int                                         _filter;
    std::string                                 _format;
    std::ofstream                               _fileStream;
    std::ostream                                *_outStream;
    std::set<Loggator*>                         _logChilds;
    bool                                        _muted;
    mutable std::mutex                          _mutex;
    std::map<const std::string, std::string>    _mapFormatKey;
    std::map<const std::string, std::string>    _mapCustomKey;

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
# define SEND_2(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)
# define SEND_3(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)
# define SEND_4(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)
# define SEND_5(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)
# define SEND_6(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)
# define SEND_7(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)
# define SEND_8(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)
# define SEND_9(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)
# define SEND_FORMAT(_typeLog, _format, ...) send(Log::eTypeLog::_typeLog, SourceInfos{__FILE__, __LINE__, __func__}, _format, ##__VA_ARGS__)

#endif // _LOGGATOR_HPP_