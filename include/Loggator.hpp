/**
 * @brief 
 * 
 * @file Loggator.hpp
 * @author Blet Mickael
 * @date 2018-05-01
 */

#ifndef _LOG_LOGGATOR_HPP_
# define _LOG_LOGGATOR_HPP_

# include <cstdarg>     // va_list, va_start, va_end
# include <cstring>     // strrchr

# include <iostream>    // string, cerr
# include <fstream>     // ostream, ofstream
# include <sstream>     // ostringstream
# include <mutex>       // mutex, lock_guard
# include <shared_mutex>
# include <thread>      // this_thread::get_id
# include <algorithm>
# include <map>         // map
# include <set>         // set

# define LFORMAT_BUFFER_SIZE     1024
# define LFORMAT_KEY_BUFFER_SIZE 64
# define LDEFAULT_FORMAT         "{TIME} {TYPE} {NAME}: "
# define LDEFAULT_TIME_FORMAT    "%y/%m/%d %X.%N"

# define LSOURCEINFOS SourceInfos{__FILE__, __LINE__, __func__}

# define LPRIMITIVE_CAT(x, y) x ## y
# define LCAT(x, y) LPRIMITIVE_CAT(x, y)

// max 19 args
# define LNARGS_SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _0, N, ...) N
# define LNARGS(...) LNARGS_SEQ(__VA_ARGS__, 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 1)
# define LPRE_NARGS(...) LNARGS(__VA_ARGS__)
# define LNOARGS() 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
# define LMACRO_CHOOSER(macro_prefix, ...) LCAT(macro_prefix, LPRE_NARGS(LNOARGS __VA_ARGS__ ()))

// format macro
# define LSENDF(_typeLog, ...)  send(Log::eTypeLog::_typeLog, LSOURCEINFOS, ##__VA_ARGS__)
# define LDEBUGF(...)           LSENDF(Debug,   __VA_ARGS__)
# define LINFOF(...)            LSENDF(Info,    __VA_ARGS__)
# define LWARNF(...)            LSENDF(Warn,    __VA_ARGS__)
# define LWARNINGF(...)         LSENDF(Warning, __VA_ARGS__)
# define LERRORF(...)           LSENDF(Error,   __VA_ARGS__)

// send macro
# define LSEND(...)         LMACRO_CHOOSER(LSEND_, __VA_ARGS__)(__VA_ARGS__)
# define LSEND_0()          send(Log::eTypeLog::Debug, LSOURCEINFOS)
# define LSEND_1(_typeLog)  send(Log::eTypeLog::_typeLog, LSOURCEINFOS)
# define LSEND_X(...)       LSENDF(__VA_ARGS__)

// type macro
# define LDEBUG(...)    LMACRO_CHOOSER(LDEBUG_, __VA_ARGS__)(__VA_ARGS__)
# define LDEBUG_0()     send(Log::eTypeLog::Debug, LSOURCEINFOS)
# define LDEBUG_1(...)  LDEBUGF(__VA_ARGS__)
# define LDEBUG_X(...)  LDEBUGF(__VA_ARGS__)

# define LINFO(...)     LMACRO_CHOOSER(LINFO_, __VA_ARGS__)(__VA_ARGS__)
# define LINFO_0()      send(Log::eTypeLog::Info, LSOURCEINFOS)
# define LINFO_1(...)   LINFOF(__VA_ARGS__)
# define LINFO_X(...)   LINFOF(__VA_ARGS__)

# define LWARN(...)     LMACRO_CHOOSER(LWARN_, __VA_ARGS__)(__VA_ARGS__)
# define LWARNING(...)  LMACRO_CHOOSER(LWARN_, __VA_ARGS__)(__VA_ARGS__)
# define LWARN_0()      send(Log::eTypeLog::Warn, LSOURCEINFOS)
# define LWARN_1(...)   LWARNF(__VA_ARGS__)
# define LWARN_X(...)   LWARNF(__VA_ARGS__)

# define LERROR(...)    LMACRO_CHOOSER(LERROR_, __VA_ARGS__)(__VA_ARGS__)
# define LERROR_0()     send(Log::eTypeLog::Error, LSOURCEINFOS)
# define LERROR_1(...)  LERRORF(__VA_ARGS__)
# define LERROR_X(...)  LERRORF(__VA_ARGS__)

namespace Log
{

enum eTypeLog
{
    Debug   = 1<<0,
    Info    = 1<<1,
    Warning = 1<<2,
    Warn    = 1<<2,
    Error   = 1<<3
};

enum eFilterLog
{
    EqualDebug          = eTypeLog::Debug,
    EqualInfo           = eTypeLog::Info,
    EqualWarning        = eTypeLog::Warning,
    EqualError          = eTypeLog::Error,
    All                 = EqualDebug | EqualInfo | EqualWarning | EqualError,
    GreaterError        = 0,
    GreaterWarning      = EqualError,
    GreaterInfo         = EqualWarning | GreaterWarning,
    GreaterDebug        = EqualInfo | GreaterInfo,
    LessDebug           = 0,
    LessInfo            = EqualDebug,
    LessWarning         = EqualInfo | LessInfo,
    LessError           = EqualWarning | LessWarning,
    GreaterEqualDebug   = All,
    GreaterEqualWarning = EqualWarning | GreaterWarning,
    GreaterEqualInfo    = EqualInfo | GreaterInfo,
    GreaterEqualError   = EqualError,
    LessEqualDebug      = EqualDebug,
    LessEqualInfo       = EqualInfo | LessInfo,
    LessEqualWarning    = EqualWarning | LessWarning,
    LessEqualError      = All
};

struct SourceInfos
{
    const char *filename;
    int         line;
    const char *func;
};

class Loggator
{

private:

    /**
     * @brief class SendFifo
     * create a temporary object ostringstream
     * at destruct send to loggator method (sendToStream)
     */
    class SendFifo
    {

    public:

        /**
         * @brief Construct a new Send Fifo object
         * 
         * @param loggator 
         * @param type 
         * @param sourceInfos 
         */
        SendFifo(const Loggator &loggator, const eTypeLog type, const SourceInfos sourceInfos) noexcept:
        _log(loggator),
        _type(type),
        _sourceInfos(sourceInfos)
        {
            return ;
        }

        /**
         * @brief Construct a new Send Fifo object
         * 
         * @param sendFifo 
         */
        SendFifo(SendFifo &&sendFifo) noexcept:
        _log(sendFifo._log),
        _type(std::move(sendFifo._type)),
        _sourceInfos(std::move(sendFifo._sourceInfos)),
        _cacheStream(std::move(sendFifo._cacheStream))
        {
            return ;
        }

        /**
         * @brief Destroy the Send Fifo object
         * 
         */
        ~SendFifo(void) noexcept
        {
            std::string cacheStr = std::move(_cacheStream.str());
            if (cacheStr.back() != '\n')
                cacheStr += '\n';
            _log.sendToStream(cacheStr, _type, _sourceInfos);
            return ;
        }

        /**
         * @brief override operator << to object _cacheStream
         * 
         * @tparam T 
         * @param var 
         * @return std::ostringstream& 
         */
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
        const eTypeLog      _type;
        const SourceInfos   _sourceInfos;
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

public:

    static std::mutex                               sMapMutex;
    static std::map<const std::string, Loggator*>   sMapLoggators;

    Loggator(void) noexcept:
    _name(""),
    _filter(eFilterLog::All),
    _format(LDEFAULT_FORMAT),
    _outStream(&std::cerr),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        return ;
    }

    Loggator(const eFilterLog &filter) noexcept:
    _name(""),
    _filter(filter),
    _format(LDEFAULT_FORMAT),
    _outStream(&std::cerr),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        return ;
    }

    Loggator(const std::string &name, const std::string &path, std::ios::openmode openMode = std::ios::app, const eFilterLog &filter = eFilterLog::All) noexcept:
    _name(name),
    _filter(filter),
    _format(LDEFAULT_FORMAT),
    _fileStream(path, std::ios::out | openMode),
    _outStream(&std::cerr),
    _muted(false)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex);
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        if (sMapLoggators.find(_name) == sMapLoggators.end())
            sMapLoggators.emplace(_name, this);
        return ;
    }

    Loggator(const std::string &name, std::ostream &oStream = std::cerr, const eFilterLog &filter = eFilterLog::All) noexcept:
    _name(name),
    _filter(filter),
    _format(LDEFAULT_FORMAT),
    _outStream(&oStream),
    _muted(false)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex);
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        if (sMapLoggators.find(name) == sMapLoggators.end())
            sMapLoggators.emplace(name, this);
        return ;
    }

    Loggator(std::ostream &oStream, const eFilterLog &filter = eFilterLog::All) noexcept:
    _name(""),
    _filter(filter),
    _format(LDEFAULT_FORMAT),
    _outStream(&oStream),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        return ;
    }

    Loggator(const std::string &name, Loggator &loggator) noexcept:
    _name(name),
    _filter(loggator._filter),
    _format(loggator._format),
    _outStream(nullptr),
    _muted(loggator._muted)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex);
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logParents.insert(this);
        _logChilds.insert(&loggator);
        _mapCustomKey = loggator._mapCustomKey;
        if (sMapLoggators.find(_name) == sMapLoggators.end())
            sMapLoggators.emplace(_name, this);
        return ;
    }

    Loggator(Loggator &loggator) noexcept:
    _name(""),
    _filter(loggator._filter),
    _format(loggator._format),
    _outStream(nullptr),
    _muted(loggator._muted)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logParents.insert(this);
        _logChilds.insert(&loggator);
        _mapCustomKey = loggator._mapCustomKey;
        return ;
    }

    Loggator    &operator=(Loggator &rhs)
    {
        std::lock_guard<std::mutex> lockGuard(this->_mutex);
        std::lock_guard<std::mutex> lockGuardParent(rhs._mutex);
        this->_filter = rhs._filter;
        this->_format = rhs._format;
        this->_muted = rhs._muted;
        for (Loggator *child : rhs._logChilds)
        {
            std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
            this->_logChilds.insert(child);
            child->_logParents.insert(this);
        }
        this->_mapCustomKey = rhs._mapCustomKey;
        return *this;
    }

    ~Loggator(void) noexcept
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex);
        std::lock_guard<std::mutex> lockGuard(_mutex);
        for (Loggator *child : _logChilds)
        {
            std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
            child->_logParents.erase(this);
        }
        for (Loggator *parent : _logParents)
        {
            std::lock_guard<std::mutex> lockGuardParent(parent->_mutex);
            parent->_logChilds.erase(this);
        }
        if (_name.empty() == false)
        {
            auto it = sMapLoggators.find(_name);
            if (it != sMapLoggators.end())
                sMapLoggators.erase(it);
        }
        return ;
    }

    static Loggator &getInstance(const std::string &name)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex);
        std::map<const std::string, Loggator*>::iterator it = sMapLoggators.find(name);
        if (it != sMapLoggators.end())
        {
            return (*(it->second));
        }
        else
        {
            throw std::runtime_error("Instance of loggator \"" +name+ "\" not found.");
        }
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
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex);
        if (sMapLoggators.find(name) != sMapLoggators.end())
            sMapLoggators.emplace(name, this);
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
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        _logChilds.insert(&loggator);
        loggator._logParents.insert(this);
        return (*this);
    }

    Loggator        &subChild(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        _logChilds.erase(&loggator);
        loggator._logParents.erase(this);
        return (*this);
    }

    Loggator        &listen(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logChilds.insert(this);
        _logParents.insert(&loggator);
        return (*this);
    }
 
    Loggator        &unlisten(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logChilds.erase(this);
        _logParents.erase(&loggator);
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
                    _mapCustomKey[key].format = LDEFAULT_TIME_FORMAT;
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

    SendFifo        send(void) const
    {
        return SendFifo(*this, eTypeLog::Debug, {nullptr, 0, nullptr});
    }

    SendFifo        send(const eTypeLog &type, const char *format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, type, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

    SendFifo        send(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) const
    {
        return SendFifo(*this, type, sourceInfos);
    }

    SendFifo        send(const eTypeLog &type, const SourceInfos &sourceInfos, const char *format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, type, sourceInfos);
        fifo << buffer;
        return fifo;
    }

    SendFifo        debug(void) const
    {
        return SendFifo(*this, eTypeLog::Debug, {nullptr, 0, nullptr});
    }

    SendFifo        debug(const char *format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Debug, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

    SendFifo        info(void) const
    {
        return SendFifo(*this, eTypeLog::Info, {nullptr, 0, nullptr});
    }

    SendFifo        info(const char *format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Info, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

    SendFifo        warning(void) const
    {
        return SendFifo(*this, eTypeLog::Warning, {nullptr, 0, nullptr});
    }

    SendFifo        warning(const char *format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Warning, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

    SendFifo        warn(void) const
    {
        return SendFifo(*this, eTypeLog::Warn, {nullptr, 0, nullptr});
    }

    SendFifo        warn(const char *format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Warn, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

    SendFifo        error(void) const
    {
        return SendFifo(*this, eTypeLog::Error, {nullptr, 0, nullptr});
    }

    SendFifo        error(const char *format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Error, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

    template<typename T>
    SendFifo        operator<<(const T &var) const
    {
        SendFifo fifo(*this, eTypeLog::Debug, {nullptr, 0, nullptr});
        fifo << var;
        return fifo;
    }

    SendFifo        operator[](const eTypeLog &type) const
    {
        return SendFifo(*this, type, {nullptr, 0, nullptr});
    }

    SendFifo        operator()(const char * format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::Debug, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

    SendFifo        operator()(const eTypeLog &type) const
    {
        return SendFifo(*this, type, {nullptr, 0, nullptr});
    }

    SendFifo        operator()(const eTypeLog &type, const char * format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, type, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }


private:

    void            sendToStream(const std::string str, const eTypeLog &type, const SourceInfos &source) const
    {
        const timeInfos timeInfos = getCurrentTimeInfos();
        if (_outStream != nullptr && _muted == false && _filter & type)
        {
            std::lock_guard<std::mutex> lockGuard(_mutex);
            *(_outStream) << prompt(  this->_name, type, _mapCustomKey, timeInfos, source) << str << std::flush;
        }
        std::set<const Loggator*> tmpsetLog;
        tmpsetLog.insert(this);
        sendChild(tmpsetLog, *this, this->_name, type, timeInfos, source, str);
    }

    void            sendChild(std::set<const Loggator*> &setLog, const Loggator &loggator, const std::string &name, const eTypeLog &type, const timeInfos &timeInfos, const SourceInfos &source, const std::string &str) const
    {
        std::lock_guard<std::mutex> lockGuard(loggator._mutex);
        for (const Loggator *child : loggator._logChilds)
        {
            if (setLog.find(child) != setLog.end())
                continue;
            setLog.insert(child);
            if (child->_outStream != nullptr && child->_muted == false && child->_filter & type)
            {
                std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
                *(child->_outStream) << child->prompt(name, type, _mapCustomKey, timeInfos, source) << str << std::flush;
            }
            sendChild(setLog, *child, name, type, timeInfos, source, str);
        }
    }

    std::string     formatTime(const timeInfos &infos) const
    {
        char bufferFormatTime[LFORMAT_BUFFER_SIZE];

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

        return timeInfos{tm_info, ts.tv_nsec / 1000};
    }

    const char      *typeToStr(const eTypeLog &type) const
    {
        switch (type)
        {
            case eTypeLog::Debug:
                return "DEBUG";
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

    std::string     formatCustomKey(const std::map<const std::string, customKey> &mapCustomKey, const std::string &key) const
    {
        std::map<const std::string, customKey>::const_iterator itMap = mapCustomKey.find(key);
        if (itMap == mapCustomKey.end())
            return "";
        if (itMap->second.value.empty())
            return "";
        char buffer[LFORMAT_KEY_BUFFER_SIZE];
        snprintf(buffer, LFORMAT_KEY_BUFFER_SIZE - 1, itMap->second.format.c_str(), itMap->second.value.c_str());
        return std::string(buffer);
    }

    std::string     formatKey(const std::string &key, const std::string &value) const
    {
        if (value.empty())
            return "";
        std::map<const std::string, customKey>::const_iterator itMap = _mapCustomKey.find(key);
        if (itMap == _mapCustomKey.end())
            return value;
        char buffer[LFORMAT_KEY_BUFFER_SIZE];
        snprintf(buffer, LFORMAT_KEY_BUFFER_SIZE - 1, itMap->second.format.c_str(), value.c_str());
        return std::string(buffer);
    }

    std::string     prompt(const std::string &name, const eTypeLog &type, const std::map<const std::string, customKey> &mapCustomKey, const timeInfos &timeInfos, const SourceInfos &source) const
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
                prompt.replace(indexStart, 6, formatKey(key, name));
            }
            else if (key == "FUNC")
            {
                if (source.func != nullptr)
                    prompt.replace(indexStart, 6, formatKey(key, source.func));
                else
                    prompt.erase(indexStart, 6);
            }
            else if (key == "PATH")
            {
                if (source.filename != nullptr)
                    prompt.replace(indexStart, 6, formatKey(key, source.filename));
                else
                    prompt.erase(indexStart, 6);
            }
            else if (key == "FILE")
            {
                if (source.filename != nullptr)
                {
                    const char *rchr = strrchr(source.filename, '/');
                    if (rchr != nullptr)
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

    Loggator(const Loggator &) = delete;
    Loggator &operator=(const Loggator &) = delete;

    std::string                             _name;
    int                                     _filter;
    std::string                             _format;
    std::ofstream                           _fileStream;
    std::ostream                            *_outStream;
    std::set<Loggator*>                     _logParents;
    std::set<Loggator*>                     _logChilds;
    mutable std::mutex                      _mutex;
    std::map<const std::string, customKey>  _mapCustomKey;
    bool                                    _muted;

};

} // end namespace

std::mutex                                  Log::Loggator::sMapMutex;
std::map<const std::string, Log::Loggator*> Log::Loggator::sMapLoggators;

#endif // _LOG_LOGGATOR_HPP_