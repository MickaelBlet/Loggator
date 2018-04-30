#ifndef _LOG_LOGGATOR_HPP_
# define _LOG_LOGGATOR_HPP_

# include <cstdarg>     // va_list, va_start, va_end
# include <cstring>     // strrchr

# include <iostream>    // string, cerr
# include <fstream>     // ostream, ofstream
# include <sstream>     // ostringstream
# include <mutex>       // mutex, lock_guard
# include <thread>      // this_thread::get_id
# include <map>         // map
# include <set>         // set

# define FORMAT_BUFFER_SIZE     1024
# define FORMAT_KEY_BUFFER_SIZE 127
# define DEFAULT_FORMAT         "{TIME} {TYPE} {NAME}: "
# define DEFAULT_TIME_FORMAT    "%y/%m/%d %X.%N"

namespace Log
{

enum eTypeLog
{
    Debug           = 1<<0,
    Trace           = 1<<1,
    Info            = 1<<2,
    Warning         = 1<<3,
    Warn            = 1<<3,
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

        SendFifo(const Loggator &loggator, eTypeLog type, SourceInfos sourceInfos) noexcept:
        _log(loggator),
        _type(type),
        _sourceInfos(sourceInfos)
        {
            return ;
        }

        SendFifo(SendFifo &&sendFifo) noexcept:
        _log(sendFifo._log),
        _type(std::move(sendFifo._type)),
        _sourceInfos(std::move(sendFifo._sourceInfos)),
        _cacheStream(std::move(sendFifo._cacheStream))
        {
            return ;
        }

        ~SendFifo() noexcept
        {
            std::string cacheStr = std::move(_cacheStream.str());
            if (cacheStr.back() != '\n')
                cacheStr += '\n';
            _log.sendToStream(cacheStr, _type, _sourceInfos);
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

        const Loggator      &_log;
        eTypeLog            _type;
        SourceInfos         _sourceInfos;
        std::ostringstream  _cacheStream;

    };

    struct timeInfos
    {
        const struct tm* tm;
        const long       msec;
    };

    struct customKey
    {
        std::string format;
        std::string value;
    };

    Loggator(void) noexcept:
    _name(""),
    _filter(eFilterLog::All),
    _format(DEFAULT_FORMAT),
    _outStream(&std::cerr),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = DEFAULT_TIME_FORMAT;
        return ;
    }

    Loggator(const std::string &name, const std::string &path, std::ios::openmode openMode = std::ios::app) noexcept:
    _name(name),
    _filter(eFilterLog::All),
    _format(DEFAULT_FORMAT),
    _fileStream(path, std::ios::out | openMode),
    _outStream(&std::cerr),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = DEFAULT_TIME_FORMAT;
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        return ;
    }

    Loggator(const std::string &name, std::ostream &oStream = std::cerr) noexcept:
    _name(name),
    _filter(eFilterLog::All),
    _format(DEFAULT_FORMAT),
    _outStream(&oStream),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = DEFAULT_TIME_FORMAT;
        return ;
    }

    Loggator(std::ostream &oStream) noexcept:
    _name(""),
    _filter(eFilterLog::All),
    _format(DEFAULT_FORMAT),
    _outStream(&oStream),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = DEFAULT_TIME_FORMAT;
        return ;
    }

    Loggator(const std::string &name, Loggator &loggator) noexcept:
    _name(name),
    _filter(loggator._filter),
    _format(loggator._format),
    _outStream(NULL),
    _muted(loggator._muted)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        _logChilds = loggator._logChilds;
        _mapCustomKey = loggator._mapCustomKey;
        _logChilds.insert(&loggator);
        loggator._logChilds.insert(this);
        return ;
    }

    Loggator(Loggator &loggator) noexcept:
    _name(loggator._name),
    _filter(loggator._filter),
    _format(loggator._format),
    _outStream(NULL),
    _muted(loggator._muted)
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        _logChilds = loggator._logChilds;
        _mapCustomKey = loggator._mapCustomKey;
        _logChilds.insert(&loggator);
        loggator._logChilds.insert(this);
        return ;
    }

    Loggator    &operator=(Loggator &rhs)
    {
        this->_name = rhs._name;
        this->_filter = rhs._filter;
        this->_format = rhs._format;
        this->_muted = rhs._muted;
        std::lock_guard<std::mutex> lockGuard(rhs._mutex);
        this->_logChilds = rhs._logChilds;
        this->_mapCustomKey = rhs._mapCustomKey;
        return *this;
    }

    ~Loggator(void) noexcept
    {
        return ;
    }

    void            setOutStream(std::ostream &os)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _outStream = &os;
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

    Loggator        &setKey(const std::string &key, const std::string &value)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _mapCustomKey[key].value = value;
        return (*this);
    }

    void            setMuted(bool mute)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
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
                std::string key = _format.substr(indexStart + 1, indexEnd - indexStart - 1);
                if (key == "TIME")
                    _mapCustomKey[key].format = DEFAULT_TIME_FORMAT;
                else
                    _mapCustomKey[key].format = "%s";
                indexStart = _format.find("{", indexStart + 1);
                continue;
            }
            std::string key = _format.substr(indexStart + 1, indexFormat - indexStart - 1);
            std::string formatKey = _format.substr(indexFormat + 1, indexEnd - indexFormat - 1);
            _mapCustomKey[key].format = std::move(formatKey);
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
        {
            _fileStream.close();
            _fileStream.clear();
        }
        _fileStream.open(path, std::ios::out | openMode);
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        else
            _outStream = &std::cerr;
        return _fileStream.is_open();
    }

    void            close(void)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_fileStream.is_open())
        {
            _fileStream.close();
            _fileStream.clear();
        }
        _outStream = &std::cerr;
    }

    bool            isOpened(void) const
    {
        return _fileStream.is_open();
    }

    std::string     formatTime(const timeInfos &infos) const
    {
        char bufferFormatTime[FORMAT_BUFFER_SIZE];

        std::string retStr = _mapCustomKey.at("TIME").format;

        std::size_t findPos = retStr.find("%N");
        if (findPos != std::string::npos)
        {
            snprintf(bufferFormatTime, 7, "%06ld", infos.msec);
            retStr.replace(findPos, 2, bufferFormatTime, 6);
        }
        strftime(bufferFormatTime, 126, retStr.c_str(), infos.tm);
        return std::string(bufferFormatTime);
    }

    const timeInfos getCurrentTimeInfos(void) const
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

    const char      *typeToStr(const eTypeLog &type) const
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

    void            sendToStream(const std::string str, const eTypeLog &type, const SourceInfos &source) const
    {
        const timeInfos timeInfos = getCurrentTimeInfos();
        if (_outStream != NULL && _muted == false && _filter & type)
        {
            std::lock_guard<std::mutex> lockGuard(_mutex);
            *(_outStream) << prompt(*this, type, _mapCustomKey, timeInfos, source) << str << std::flush;
        }
        std::set<const Loggator*> tmpsetLog;
        tmpsetLog.insert(this);
        sendChild(tmpsetLog, *this, type, timeInfos, source, str);
    }

    void            sendChild(std::set<const Loggator*> &setLog, const Loggator &loggator, const eTypeLog &type, const timeInfos &timeInfos, const SourceInfos &source, const std::string &str) const
    {
        std::lock_guard<std::mutex> lockGuardMain(loggator._mutex);
        for (const Loggator *child : loggator._logChilds)
        {
            if (setLog.find(child) != setLog.end())
                continue;
            setLog.insert(child);
            if (child->_outStream != NULL && child->_muted == false && child->_filter & type)
            {
                std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
                *(child->_outStream) << child->prompt(loggator, type, _mapCustomKey, timeInfos, source) << str << std::flush;
            }
            sendChild(setLog, *child, type, timeInfos, source, str);
        }
    }

    std::string     formatCustomKey(const std::map<const std::string, customKey> &mapCustomKey, const std::string &key) const
    {
        std::map<const std::string, customKey>::const_iterator itMap = mapCustomKey.find(key);
        if (itMap == mapCustomKey.end())
            return "";
        if (itMap->second.value.empty())
            return "";
        char buffer[FORMAT_KEY_BUFFER_SIZE];
        snprintf(buffer, FORMAT_KEY_BUFFER_SIZE - 1, itMap->second.format.c_str(), itMap->second.value.c_str());
        return std::string(buffer);
    }

    std::string     formatKey(const std::string &key, const std::string &value) const
    {
        if (value.empty())
            return "";
        std::map<const std::string, customKey>::const_iterator itMap = _mapCustomKey.find(key);
        if (itMap == _mapCustomKey.end())
            return value;
        char buffer[FORMAT_KEY_BUFFER_SIZE];
        snprintf(buffer, FORMAT_KEY_BUFFER_SIZE - 1, itMap->second.format.c_str(), value.c_str());
        return std::string(buffer);
    }

    std::string     prompt(const Loggator &loggator, const eTypeLog &type, const std::map<const std::string, customKey> &mapCustomKey, const timeInfos &timeInfos, const SourceInfos &source) const
    {
        std::string prompt = _format;
        std::size_t indexStart = prompt.find("{");
        while (indexStart != std::string::npos)
        {
            std::size_t indexEnd = prompt.find("}", indexStart);
            if (indexEnd == std::string::npos)
                break;
            std::string key = prompt.substr(indexStart + 1, indexEnd - indexStart - 1);
            if (key == "TIME")
            {
                prompt.replace(indexStart, 6, formatTime(timeInfos));
            }
            else if (key == "TYPE")
            {
                prompt.replace(indexStart, 6, formatKey(key, typeToStr(type)));
            }
            else if (key == "NAME")
            {
                prompt.replace(indexStart, 6, formatKey(key, loggator._name));
            }
            else if (key == "FUNC")
            {
                if (source.func != NULL)
                    prompt.replace(indexStart, 6, formatKey(key, source.func));
                else
                    prompt.erase(indexStart, 6);
            }
            else if (key == "PATH")
            {
                if (source.filename != NULL)
                    prompt.replace(indexStart, 6, formatKey(key, source.filename));
                else
                    prompt.erase(indexStart, 6);
            }
            else if (key == "FILE")
            {
                if (source.filename != NULL)
                {
                    const char *rchr = strrchr(source.filename, '/');
                    if (rchr != NULL)
                        prompt.replace(indexStart, 6, formatKey(key, source.filename + (rchr - source.filename) + 1));
                    else
                        prompt.replace(indexStart, 6, formatKey(key, source.filename));
                }
                else
                    prompt.erase(indexStart, 6);
            }
            else if (key == "LINE")
            {
                if (source.line > 0)
                    prompt.replace(indexStart, 6, formatKey(key, std::to_string(source.line)));
                else
                    prompt.erase(indexStart, 6);
            }
            else if (key == "THREAD_ID")
            {
                std::stringstream stream;
                stream << std::hex << std::uppercase << std::this_thread::get_id();
                prompt.replace(indexStart, 11, formatKey(key, stream.str()));
            }
            else
            {
                prompt.replace(indexStart, key.size() + 2, formatCustomKey(mapCustomKey, key));
            }
            indexStart = prompt.find("{", indexStart);
        }
        return prompt;
    }

    SendFifo        send(const SourceInfos &sourceInfos = {NULL, 0, NULL}) const
    {
        return SendFifo(*this, eTypeLog::Debug, sourceInfos);
    }

    SendFifo        send(const eTypeLog &type, const SourceInfos &sourceInfos = {NULL, 0, NULL}) const
    {
        return SendFifo(*this, type, sourceInfos);
    }

    SendFifo        send(const eTypeLog &type, const char *format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, type, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

    SendFifo        send(const eTypeLog &type, const SourceInfos &sourceInfos, const char *format, ...) const
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

    SendFifo        debug(const SourceInfos &sourceInfos = {NULL, 0, NULL}) const
    {
        return SendFifo(*this, eTypeLog::Debug, sourceInfos);
    }

    SendFifo        debug(const char *format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Debug, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

    SendFifo        trace(const SourceInfos &sourceInfos = {NULL, 0, NULL}) const
    {
        return SendFifo(*this, eTypeLog::Trace, sourceInfos);
    }

    SendFifo        trace(const char *format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Trace, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

    SendFifo        info(const SourceInfos &sourceInfos = {NULL, 0, NULL}) const
    {
        return SendFifo(*this, eTypeLog::Info, sourceInfos);
    }

    SendFifo        info(const char *format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Info, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

    SendFifo        warning(const SourceInfos &sourceInfos = {NULL, 0, NULL}) const
    {
        return SendFifo(*this, eTypeLog::Warning, sourceInfos);
    }

    SendFifo        warning(const char *format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Warning, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

    SendFifo        warn(const SourceInfos &sourceInfos = {NULL, 0, NULL}) const
    {
        return SendFifo(*this, eTypeLog::Warn, sourceInfos);
    }

    SendFifo        warn(const char *format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Warn, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

    SendFifo        error(const SourceInfos &sourceInfos = {NULL, 0, NULL}) const
    {
        return SendFifo(*this, eTypeLog::Error, sourceInfos);
    }

    SendFifo        error(const char *format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Error, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

    template<typename T>
    SendFifo        operator<<(const T &var) const
    {
        SendFifo fifo(*this, eTypeLog::Debug, {NULL, 0, NULL});
        fifo << var;
        return fifo;
    }

    SendFifo        operator[](const eTypeLog &type) const
    {
        return SendFifo(*this, type, {NULL, 0, NULL});
    }

    SendFifo        operator()(const eTypeLog &type, const char * format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, type, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

    SendFifo        operator()(const char * format, ...) const
    {
        char    buffer[FORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, FORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Error, {NULL, 0, NULL});
        fifo << buffer;
        return fifo;
    }

private:
    Loggator(const Loggator &) = delete;
    Loggator &operator=(const Loggator &) = delete;

    std::string                             _name;
    int                                     _filter;
    std::string                             _format;
    std::ofstream                           _fileStream;
    std::ostream                            *_outStream;
    std::set<Loggator*>                     _logChilds;
    mutable std::mutex                      _mutex;
    std::map<const std::string, customKey>  _mapCustomKey;
    bool                                    _muted;

};

} // end namespace

# define SOURCEINFOS SourceInfos{__FILE__, __LINE__, __func__}

# define NARGS_SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9, _0, N, ...) N
# define NARGS(...) NARGS_SEQ(__VA_ARGS__, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1)
# define PRIMITIVE_CAT(x, y) x ## y
# define CAT(x, y) PRIMITIVE_CAT(x, y)
# define PRE_NARGS(...) NARGS(__VA_ARGS__)
# define NOARGS() ,,,,,,,,,
# define MACRO_CHOOSER(macro_prefix, ...) CAT(macro_prefix, PRE_NARGS(NOARGS __VA_ARGS__ ()))

# define LSENDF(_typeLog, ...) send(Log::eTypeLog::_typeLog, SOURCEINFOS, ##__VA_ARGS__)
# define LSEND(...) MACRO_CHOOSER(LSEND_, __VA_ARGS__)(__VA_ARGS__)
# define LSEND_0() send(Log::eTypeLog::Debug, SOURCEINFOS)
# define LSEND_1(_typeLog) send(Log::eTypeLog::_typeLog, SOURCEINFOS)
# define LSEND_2(...) LSENDF(__VA_ARGS__)
# define LSEND_3(...) LSENDF(__VA_ARGS__)
# define LSEND_4(...) LSENDF(__VA_ARGS__)
# define LSEND_5(...) LSENDF(__VA_ARGS__)
# define LSEND_6(...) LSENDF(__VA_ARGS__)
# define LSEND_7(...) LSENDF(__VA_ARGS__)
# define LSEND_8(...) LSENDF(__VA_ARGS__)
# define LSEND_9(...) LSENDF(__VA_ARGS__)

# define LDEBUGF(...) send(Log::eTypeLog::Debug, SOURCEINFOS, ##__VA_ARGS__)
# define LDEBUG(...) MACRO_CHOOSER(LDEBUG_, __VA_ARGS__)(__VA_ARGS__)
# define LDEBUG_0() send(Log::eTypeLog::Debug, SOURCEINFOS)
# define LDEBUG_1(...) LDEBUGF(__VA_ARGS__)
# define LDEBUG_2(...) LDEBUGF(__VA_ARGS__)
# define LDEBUG_3(...) LDEBUGF(__VA_ARGS__)
# define LDEBUG_4(...) LDEBUGF(__VA_ARGS__)
# define LDEBUG_5(...) LDEBUGF(__VA_ARGS__)
# define LDEBUG_6(...) LDEBUGF(__VA_ARGS__)
# define LDEBUG_7(...) LDEBUGF(__VA_ARGS__)
# define LDEBUG_8(...) LDEBUGF(__VA_ARGS__)
# define LDEBUG_9(...) LDEBUGF(__VA_ARGS__)

# define LTRACEF(...) send(Log::eTypeLog::Trace, SOURCEINFOS, ##__VA_ARGS__)
# define LTRACE(...) MACRO_CHOOSER(LTRACE_, __VA_ARGS__)(__VA_ARGS__)
# define LTRACE_0() send(Log::eTypeLog::Trace, SOURCEINFOS)
# define LTRACE_1(...) LTRACEF(__VA_ARGS__)
# define LTRACE_2(...) LTRACEF(__VA_ARGS__)
# define LTRACE_3(...) LTRACEF(__VA_ARGS__)
# define LTRACE_4(...) LTRACEF(__VA_ARGS__)
# define LTRACE_5(...) LTRACEF(__VA_ARGS__)
# define LTRACE_6(...) LTRACEF(__VA_ARGS__)
# define LTRACE_7(...) LTRACEF(__VA_ARGS__)
# define LTRACE_8(...) LTRACEF(__VA_ARGS__)
# define LTRACE_9(...) LTRACEF(__VA_ARGS__)

# define LINFOF(...) send(Log::eTypeLog::Info, SOURCEINFOS, ##__VA_ARGS__)
# define LINFO(...) MACRO_CHOOSER(LINFO_, __VA_ARGS__)(__VA_ARGS__)
# define LINFO_0() send(Log::eTypeLog::Info, SOURCEINFOS)
# define LINFO_1(...) LINFOF(__VA_ARGS__)
# define LINFO_2(...) LINFOF(__VA_ARGS__)
# define LINFO_3(...) LINFOF(__VA_ARGS__)
# define LINFO_4(...) LINFOF(__VA_ARGS__)
# define LINFO_5(...) LINFOF(__VA_ARGS__)
# define LINFO_6(...) LINFOF(__VA_ARGS__)
# define LINFO_7(...) LINFOF(__VA_ARGS__)
# define LINFO_8(...) LINFOF(__VA_ARGS__)
# define LINFO_9(...) LINFOF(__VA_ARGS__)

# define LWARNINGF(...) send(Log::eTypeLog::Warning, SOURCEINFOS, ##__VA_ARGS__)
# define LWARNING(...) MACRO_CHOOSER(LWARN_, __VA_ARGS__)(__VA_ARGS__)
# define LWARNF(...) send(Log::eTypeLog::Warn, SOURCEINFOS, ##__VA_ARGS__)
# define LWARN(...) MACRO_CHOOSER(LWARN_, __VA_ARGS__)(__VA_ARGS__)
# define LWARN_0() send(Log::eTypeLog::Warn, SOURCEINFOS)
# define LWARN_1(...) LWARNF(__VA_ARGS__)
# define LWARN_2(...) LWARNF(__VA_ARGS__)
# define LWARN_3(...) LWARNF(__VA_ARGS__)
# define LWARN_4(...) LWARNF(__VA_ARGS__)
# define LWARN_5(...) LWARNF(__VA_ARGS__)
# define LWARN_6(...) LWARNF(__VA_ARGS__)
# define LWARN_7(...) LWARNF(__VA_ARGS__)
# define LWARN_8(...) LWARNF(__VA_ARGS__)
# define LWARN_9(...) LWARNF(__VA_ARGS__)

# define LERRORF(...) send(Log::eTypeLog::Error, SOURCEINFOS, ##__VA_ARGS__)
# define LERROR(...) MACRO_CHOOSER(LERROR_, __VA_ARGS__)(__VA_ARGS__)
# define LERROR_0() send(Log::eTypeLog::Error, SOURCEINFOS)
# define LERROR_1(...) LERRORF(__VA_ARGS__)
# define LERROR_2(...) LERRORF(__VA_ARGS__)
# define LERROR_3(...) LERRORF(__VA_ARGS__)
# define LERROR_4(...) LERRORF(__VA_ARGS__)
# define LERROR_5(...) LERRORF(__VA_ARGS__)
# define LERROR_6(...) LERRORF(__VA_ARGS__)
# define LERROR_7(...) LERRORF(__VA_ARGS__)
# define LERROR_8(...) LERRORF(__VA_ARGS__)
# define LERROR_9(...) LERRORF(__VA_ARGS__)

#endif // _LOGGATOR_HPP_