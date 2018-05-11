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
# include <thread>      // this_thread::get_id
# include <map>         // map
# include <set>         // set

# define LFORMAT_BUFFER_SIZE     1024
# define LFORMAT_KEY_BUFFER_SIZE 64
# define LDEFAULT_FORMAT         "{TYPE} {TIME} {NAME}: "
# define LDEFAULT_TIME_FORMAT    "%x %X.%N"

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
# define LDEBUGF(...)           LSENDF(DEBUG,       __VA_ARGS__)
# define LINFOF(...)            LSENDF(INFO,        __VA_ARGS__)
# define LWARNF(...)            LSENDF(WARN,        __VA_ARGS__)
# define LWARNINGF(...)         LSENDF(WARNING,     __VA_ARGS__)
# define LERRORF(...)           LSENDF(ERROR,       __VA_ARGS__)
# define LCRITF(...)            LSENDF(CRIT,        __VA_ARGS__)
# define LCRITICALF(...)        LSENDF(CRITICAL,    __VA_ARGS__)
# define LALERTF(...)           LSENDF(ALERT,       __VA_ARGS__)
# define LFATALF(...)           LSENDF(FATAL,       __VA_ARGS__)
# define LEMERGF(...)           LSENDF(EMERG,       __VA_ARGS__)
# define LEMERGENCYF(...)       LSENDF(EMERGENCY,   __VA_ARGS__)

// send macro
# define LSEND(...)         LMACRO_CHOOSER(LSEND_, __VA_ARGS__)(__VA_ARGS__)
# define LSEND_0()          send(Log::eTypeLog::DEBUG, LSOURCEINFOS)
# define LSEND_1(_typeLog)  send(Log::eTypeLog::_typeLog, LSOURCEINFOS)
# define LSEND_X(...)       LSENDF(__VA_ARGS__)

// type macro
# define LDEBUG(...)        LMACRO_CHOOSER(LDEBUG_, __VA_ARGS__)(__VA_ARGS__)
# define LDEBUG_0()         send(Log::eTypeLog::DEBUG, LSOURCEINFOS)
# define LDEBUG_1(...)      LDEBUGF(__VA_ARGS__)
# define LDEBUG_X(...)      LDEBUGF(__VA_ARGS__)

# define LINFO(...)         LMACRO_CHOOSER(LINFO_, __VA_ARGS__)(__VA_ARGS__)
# define LINFO_0()          send(Log::eTypeLog::INFO, LSOURCEINFOS)
# define LINFO_1(...)       LINFOF(__VA_ARGS__)
# define LINFO_X(...)       LINFOF(__VA_ARGS__)

# define LWARN(...)         LMACRO_CHOOSER(LWARN_, __VA_ARGS__)(__VA_ARGS__)
# define LWARNING(...)      LMACRO_CHOOSER(LWARN_, __VA_ARGS__)(__VA_ARGS__)
# define LWARN_0()          send(Log::eTypeLog::WARN, LSOURCEINFOS)
# define LWARN_1(...)       LWARNF(__VA_ARGS__)
# define LWARN_X(...)       LWARNF(__VA_ARGS__)

# define LERROR(...)        LMACRO_CHOOSER(LERROR_, __VA_ARGS__)(__VA_ARGS__)
# define LERROR_0()         send(Log::eTypeLog::ERROR, LSOURCEINFOS)
# define LERROR_1(...)      LERRORF(__VA_ARGS__)
# define LERROR_X(...)      LERRORF(__VA_ARGS__)

# define LCRIT(...)         LMACRO_CHOOSER(LCRIT_, __VA_ARGS__)(__VA_ARGS__)
# define LCRITICAL(...)     LMACRO_CHOOSER(LCRIT_, __VA_ARGS__)(__VA_ARGS__)
# define LCRIT_0()          send(Log::eTypeLog::CRIT, LSOURCEINFOS)
# define LCRIT_1(...)       LCRITF(__VA_ARGS__)
# define LCRIT_X(...)       LCRITF(__VA_ARGS__)

# define LALERT(...)        LMACRO_CHOOSER(LALERT_, __VA_ARGS__)(__VA_ARGS__)
# define LALERT_0()         send(Log::eTypeLog::ALERT, LSOURCEINFOS)
# define LALERT_1(...)      LALERTF(__VA_ARGS__)
# define LALERT_X(...)      LALERTF(__VA_ARGS__)

# define LFATAL(...)        LMACRO_CHOOSER(LFATAL_, __VA_ARGS__)(__VA_ARGS__)
# define LFATAL_0()         send(Log::eTypeLog::FATAL, LSOURCEINFOS)
# define LFATAL_1(...)      LFATALF(__VA_ARGS__)
# define LFATAL_X(...)      LFATALF(__VA_ARGS__)

# define LEMERG(...)        LMACRO_CHOOSER(LEMERG_, __VA_ARGS__)(__VA_ARGS__)
# define LEMERGENCY(...)    LMACRO_CHOOSER(LEMERG_, __VA_ARGS__)(__VA_ARGS__)
# define LEMERG_0()         send(Log::eTypeLog::EMERG, LSOURCEINFOS)
# define LEMERG_1(...)      LEMERGF(__VA_ARGS__)
# define LEMERG_X(...)      LEMERGF(__VA_ARGS__)

namespace Log
{

enum eTypeLog
{
    DEBUG       = 1<<0,
    INFO        = 1<<1,
    WARN        = 1<<2,
    WARNING     = 1<<2,
    ERROR       = 1<<3,
    CRIT        = 1<<4,
    CRITICAL    = 1<<4,
    ALERT       = 1<<5,
    EMERG       = 1<<6,
    EMERGENCY   = 1<<6,
    FATAL       = 1<<7
};

enum eFilterLog
{
    EQUAL_DEBUG             = eTypeLog::DEBUG,
    EQUAL_INFO              = eTypeLog::INFO,
    EQUAL_WARN              = eTypeLog::WARN,
    EQUAL_WARNING           = eTypeLog::WARNING,
    EQUAL_ERROR             = eTypeLog::ERROR,
    EQUAL_CRIT              = eTypeLog::CRIT,
    EQUAL_CRITICAL          = eTypeLog::CRITICAL,
    EQUAL_ALERT             = eTypeLog::ALERT,
    EQUAL_EMERG             = eTypeLog::EMERG,
    EQUAL_EMERGENCY         = eTypeLog::EMERGENCY,
    EQUAL_FATAL             = eTypeLog::FATAL,
    ALL                     = EQUAL_DEBUG | EQUAL_INFO | EQUAL_WARN | EQUAL_ERROR | EQUAL_CRIT | EQUAL_ALERT | EQUAL_FATAL | EQUAL_EMERG,
    GREATER_FATAL           = 0,
    GREATER_EMERG           = EQUAL_FATAL,
    GREATER_EMERGENCY       = GREATER_EMERG,
    GREATER_ALERT           = EQUAL_EMERG | GREATER_EMERG,
    GREATER_CRIT            = EQUAL_ALERT | GREATER_ALERT,
    GREATER_CRITICAL        = GREATER_CRIT,
    GREATER_ERROR           = EQUAL_CRIT | GREATER_CRIT,
    GREATER_WARN            = EQUAL_ERROR | GREATER_ERROR,
    GREATER_WARNING         = GREATER_WARN,
    GREATER_INFO            = EQUAL_WARN | GREATER_WARN,
    GREATER_DEBUG           = EQUAL_INFO | GREATER_INFO,
    GREATER_EQUAL_FATAL     = EQUAL_FATAL,
    GREATER_EQUAL_EMERG     = EQUAL_EMERG | GREATER_EMERG,
    GREATER_EQUAL_EMERGENCY = GREATER_EQUAL_EMERG,
    GREATER_EQUAL_ALERT     = EQUAL_ALERT | GREATER_ALERT,
    GREATER_EQUAL_CRIT      = EQUAL_CRIT | GREATER_CRIT,
    GREATER_EQUAL_CRITICAL  = GREATER_EQUAL_CRIT,
    GREATER_EQUAL_ERROR     = EQUAL_ERROR | GREATER_ERROR,
    GREATER_EQUAL_WARN      = EQUAL_WARN | GREATER_WARN,
    GREATER_EQUAL_WARNING   = GREATER_EQUAL_WARN,
    GREATER_EQUAL_INFO      = EQUAL_INFO | GREATER_INFO,
    GREATER_EQUAL_DEBUG     = EQUAL_DEBUG | GREATER_DEBUG,
    LESS_DEBUG              = 0,
    LESS_INFO               = EQUAL_DEBUG,
    LESS_WARN               = EQUAL_INFO | LESS_INFO,
    LESS_WARNING            = LESS_WARN,
    LESS_ERROR              = EQUAL_WARN | LESS_WARN,
    LESS_CRIT               = EQUAL_ERROR | LESS_ERROR,
    LESS_CRITICAL           = LESS_CRIT,
    LESS_ALERT              = EQUAL_CRIT | LESS_CRIT,
    LESS_EMERG              = EQUAL_ALERT | LESS_ALERT,
    LESS_EMERGENCY          = LESS_EMERG,
    LESS_FATAL              = EQUAL_EMERG | LESS_EMERG,
    LESS_EQUAL_DEBUG        = EQUAL_DEBUG,
    LESS_EQUAL_INFO         = EQUAL_INFO | LESS_INFO,
    LESS_EQUAL_WARN         = EQUAL_WARN | LESS_WARN,
    LESS_EQUAL_WARNING      = LESS_EQUAL_WARN,
    LESS_EQUAL_ERROR        = EQUAL_ERROR | LESS_ERROR,
    LESS_EQUAL_CRIT         = EQUAL_CRIT | LESS_CRIT,
    LESS_EQUAL_CRITICAL     = LESS_EQUAL_CRIT,
    LESS_EQUAL_ALERT        = EQUAL_ALERT | LESS_ALERT,
    LESS_EQUAL_EMERG        = EQUAL_EMERG | LESS_EMERG,
    LESS_EQUAL_EMERGENCY    = LESS_EQUAL_EMERG,
    LESS_EQUAL_FATAL        = EQUAL_FATAL | LESS_FATAL
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
        _cacheStream(std::move(sendFifo._cacheStream)),
        _log(sendFifo._log),
        _type(std::move(sendFifo._type)),
        _sourceInfos(std::move(sendFifo._sourceInfos))
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

        /*************************************************************************/

        /**
         * @brief 
         * 
         * @param type 
         * @return SendFifo 
         */
        SendFifo& operator[](const eTypeLog &type)
        {
            _type = type;
            return *this;
        }

        /**
         * @brief 
         * 
         * @param type 
         * @return SendFifo 
         */
        SendFifo& operator<<(const eTypeLog &type)
        {
            _type = type;
            return *this;
        }

        /**
         * @brief override operator << to object
         * 
         * @tparam T 
         * @param var 
         * @return std::ostringstream& 
         */
        template<typename T>
        SendFifo& operator<<(const T& var)
        {
            _cacheStream << var;
            return *this;
        }

        /**
         * @brief overide operator << to object
         * 
         * @param manip function pointer (std::endl, std::flush, ...)
         * @return std::ostringstream& 
         */
        SendFifo& operator<<(std::ostream&(*manip)(std::ostream&))
        {
            manip(_cacheStream);
            return *this;
        }

        /**
         * @brief 
         * 
         * @return SendFifo 
         */
        SendFifo& operator()(void)
        {
            return *this;
        }

        /**
         * @brief 
         * 
         * @param format 
         * @param ... 
         * @return SendFifo 
         */
        SendFifo& operator()(const char * format, ...)
        {
            char    buffer[LFORMAT_BUFFER_SIZE];
            va_list vargs;
            va_start(vargs, format);
            vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
            va_end(vargs);
            _cacheStream << buffer;
            return *this;
        }

        /**
         * @brief 
         * 
         * @tparam T 
         * @param var 
         * @return SendFifo 
         */
        template<typename T>
        SendFifo& operator()(const T& var)
        {
            _cacheStream << var;
            return *this;
        }

    private:
        SendFifo(void) = delete;
        SendFifo(const SendFifo &) = delete;
        SendFifo &operator=(const SendFifo &) = delete;

        std::ostringstream  _cacheStream;
        const Loggator      &_log;
        eTypeLog            _type;
        const SourceInfos   _sourceInfos;

    };

    /**
     * @brief 
     * struct tm* tm   : stock pointer of struct tm
     * long       msec : stock milisecond of time
     */
    struct timeInfos
    {
        const struct tm* tm;
        const long       msec;
    };

    /**
     * @brief 
     * std::string format : style printf (exemple: "%10s")
     * std::string value  : value of key
     */
    struct customKey
    {
        std::string format;
        std::string value;
    };

public:

    /**
     * @brief Construct a new Loggator object
     * 
     */
    Loggator(void) noexcept:
    _name(""),
    _filter(eFilterLog::ALL),
    _format(LDEFAULT_FORMAT),
    _outStream(&std::cerr),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param filter
     */
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

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param path 
     * @param openMode 
     * @param filter 
     */
    Loggator(const std::string &name, const std::string &path, std::ios::openmode openMode = std::ios::app, const eFilterLog &filter = eFilterLog::ALL) noexcept:
    _name(name),
    _filter(filter),
    _format(LDEFAULT_FORMAT),
    _fileStream(path, std::ios::out | openMode),
    _outStream(&std::cerr),
    _muted(false)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        // if name is not empty and not in static list of loggators
        if (_name.empty() == false && sMapLoggators().find(_name) == sMapLoggators().end())
        {
            // add new loggator in static list
            sMapLoggators().emplace(_name, this);
        }
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param oStream 
     * @param filter 
     */
    Loggator(const std::string &name, std::ostream &oStream = std::cerr, const eFilterLog &filter = eFilterLog::ALL) noexcept:
    _name(name),
    _filter(filter),
    _format(LDEFAULT_FORMAT),
    _outStream(&oStream),
    _muted(false)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        // if name is not empty and not in static list of loggators
        if (_name.empty() == false && sMapLoggators().find(_name) == sMapLoggators().end())
        {
            // add new loggator in static list
            sMapLoggators().emplace(_name, this);
        }
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param oStream 
     * @param filter 
     */
    Loggator(std::ostream &oStream, const eFilterLog &filter = eFilterLog::ALL) noexcept:
    _name(""),
    _filter(filter),
    _format(LDEFAULT_FORMAT),
    _outStream(&oStream),
    _muted(false)
    {
        _mapCustomKey["TIME"].format = LDEFAULT_TIME_FORMAT;
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param loggator 
     */
    Loggator(const std::string &name, Loggator &loggator) noexcept:
    _name(name),
    _filter(loggator._filter),
    _format(loggator._format),
    _outStream(nullptr),
    _muted(loggator._muted)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logParents.insert(this);
        _logChilds.insert(&loggator);
        _mapCustomKey = loggator._mapCustomKey;
        // if name is not empty and not in static list of loggators
        if (_name.empty() == false && sMapLoggators().find(_name) == sMapLoggators().end())
        {
            // add new loggator in static list
            sMapLoggators().emplace(_name, this);
        }
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param loggator 
     */
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

    /**
     * @brief 
     * 
     * @param rhs 
     * @return Loggator& 
     */
    Loggator    &operator=(const Loggator &rhs)
    {
        std::lock_guard<std::mutex> lockGuard(this->_mutex);
        std::lock_guard<std::mutex> lockGuardParent(rhs._mutex);
        this->_filter = rhs._filter;
        this->_format = rhs._format;
        this->_muted = rhs._muted;
        for (Loggator *child : rhs._logChilds)
        {
            std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
            child->_logParents.insert(this);
        }
        this->_logChilds = rhs._logChilds;
        this->_mapCustomKey = rhs._mapCustomKey;
        return *this;
    }

    /**
     * @brief Destroy the Loggator object
     * 
     */
    ~Loggator(void) noexcept
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
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
            std::map<const std::string, Log::Loggator*>::iterator it = sMapLoggators().begin();
            while (it != sMapLoggators().end())
            {
                if (it->second == this)
                {
                    sMapLoggators().erase(it);
                    break;
                }
                it++;
            }
        }
        return ;
    }

    /*************************************************************************/

    /**
     * @brief Get the Instance Loggator object by name
     * throw runtime_error if instance not found
     * @param name 
     * @return Loggator& 
     */
    static Loggator &getInstance(const std::string &name)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        std::map<const std::string, Loggator*>::iterator it = sMapLoggators().find(name);
        if (it != sMapLoggators().end())
        {
            return (*(it->second));
        }
        else
        {
            throw std::runtime_error("Instance of loggator \"" +name+ "\" not found.");
        }
    }

    /**
     * @brief Set the Out Stream object
     * 
     * @param os 
     */
    void            setOutStream(std::ostream &os)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _outStream = &os;
    }

    /**
     * @brief Set the Name object
     * 
     * @param name 
     */
    void            setName(const std::string &name)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _name = name;
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        // if name is not empty and not in static list of loggators
        if (_name.empty() == false && sMapLoggators().find(_name) == sMapLoggators().end())
        {
            // add new loggator in static list
            sMapLoggators().emplace(_name, this);
        }
    }

    /**
     * @brief Set the Filter object
     * 
     * @param filter 
     */
    void            setFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter = filter;
    }

    /**
     * @brief 
     * 
     * @param filter 
     */
    void            addFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter |= filter;
    }

    /**
     * @brief 
     * 
     * @param filter 
     */
    void            subFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_filter | filter)
            _filter -= filter;
    }

    /**
     * @brief 
     * 
     * @param loggator 
     * @return Loggator& 
     */
    Loggator        &addChild(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        _logChilds.insert(&loggator);
        loggator._logParents.insert(this);
        return (*this);
    }

    /**
     * @brief 
     * 
     * @param loggator 
     * @return Loggator& 
     */
    Loggator        &subChild(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        _logChilds.erase(&loggator);
        loggator._logParents.erase(this);
        return (*this);
    }

    /**
     * @brief 
     * 
     * @param loggator 
     * @return Loggator& 
     */
    Loggator        &listen(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logChilds.insert(this);
        _logParents.insert(&loggator);
        return (*this);
    }

    /**
     * @brief 
     * 
     * @param loggator 
     * @return Loggator& 
     */
    Loggator        &unlisten(Loggator &loggator)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logChilds.erase(this);
        _logParents.erase(&loggator);
        return (*this);
    }

    /**
     * @brief Set the Key object
     * 
     * @param key 
     * @param value 
     * @return Loggator& 
     */
    Loggator        &setKey(const std::string &key, const std::string &value)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _mapCustomKey[key].value = value;
        return (*this);
    }

    /**
     * @brief Set the Muted object
     * 
     * @param mute 
     */
    void            setMuted(bool mute)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _muted = mute;
    }

    /**
     * @brief Set the Format object
     * 
     * @param format 
     */
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

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool            isMuted(void) const
    {
        return (_muted == true);
    }

    /**
     * @brief 
     * 
     * @param path 
     * @param openMode 
     * @return true 
     * @return false 
     */
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

    /**
     * @brief 
     * close file if file is open
     * 
     */
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

    /**
     * @brief 
     * 
     * @return true  : if file is open
     * @return false : if file is not open
     */
    bool            isOpen(void) const
    {
        return _fileStream.is_open();
    }

    /*************************************************************************/

    /**
     * @brief 
     * 
     * @return SendFifo 
     */
    SendFifo        send(void) const
    {
        return SendFifo(*this, eTypeLog::DEBUG, {nullptr, 0, nullptr});
    }

    /**
     * @brief 
     * 
     * @param type 
     * @param format 
     * @param ... 
     * @return SendFifo 
     */
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

    /**
     * @brief 
     * 
     * @param type 
     * @param sourceInfos 
     * @return SendFifo 
     */
    SendFifo        send(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) const
    {
        return SendFifo(*this, type, sourceInfos);
    }

    /**
     * @brief 
     * 
     * @param type 
     * @param sourceInfos 
     * @param format 
     * @param ... 
     * @return SendFifo 
     */
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

    #define LFUNCTION_TYPE(_type, _name)                                \
    SendFifo        _name(void) const                                   \
    {                                                                   \
        return SendFifo(*this, eTypeLog::_type, {nullptr, 0, nullptr}); \
    }                                                                   \
    SendFifo        _name(const char *format, ...) const                \
    {                                                                   \
        char    buffer[LFORMAT_BUFFER_SIZE];                            \
        va_list vargs;                                                  \
        va_start(vargs, format);                                        \
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);      \
        va_end(vargs);                                                  \
        SendFifo fifo(*this, eTypeLog::_type, {nullptr, 0, nullptr});   \
        fifo << buffer;                                                 \
        return fifo;                                                    \
    }                                                                   \
    template<typename T>                                                \
    SendFifo        _name(const T& var) const                           \
    {                                                                   \
        SendFifo fifo(*this, eTypeLog::_type, {nullptr, 0, nullptr});   \
        fifo << var;                                                    \
        return fifo;                                                    \
    }

    LFUNCTION_TYPE(DEBUG,   debug);
    LFUNCTION_TYPE(INFO,    info);
    LFUNCTION_TYPE(WARN,    warn);
    LFUNCTION_TYPE(WARN,    warning);
    LFUNCTION_TYPE(ERROR,   error);
    LFUNCTION_TYPE(CRIT,    crit);
    LFUNCTION_TYPE(CRIT,    critical);
    LFUNCTION_TYPE(ALERT,   alert);
    LFUNCTION_TYPE(EMERG,   emerg);
    LFUNCTION_TYPE(EMERG,   emergency);
    LFUNCTION_TYPE(FATAL,   fatal);

    /*************************************************************************/

    /**
     * @brief 
     * 
     * @param type 
     * @return SendFifo 
     */
    SendFifo        operator[](const eTypeLog &type) const
    {
        return SendFifo(*this, type, {nullptr, 0, nullptr});
    }

    /**
     * @brief 
     * 
     * @param type 
     * @return SendFifo 
     */
    SendFifo        operator<<(const eTypeLog &type) const
    {
        return SendFifo(*this, type, {nullptr, 0, nullptr});
    }

    /**
     * @brief 
     * 
     * @param manip 
     * @return SendFifo 
     */
    SendFifo        operator<<(std::ostream&(*manip)(std::ostream&))
    {
        SendFifo fifo(*this, eTypeLog::DEBUG, {nullptr, 0, nullptr});
        fifo << manip;
        return fifo;
    }

    /**
     * @brief 
     * 
     * @tparam T 
     * @param var 
     * @return SendFifo 
     */
    template<typename T>
    SendFifo        operator<<(const T &var) const
    {
        SendFifo fifo(*this, eTypeLog::DEBUG, {nullptr, 0, nullptr});
        fifo << var;
        return fifo;
    }

    /**
     * @brief 
     * 
     * @return SendFifo 
     */
    SendFifo        operator()(void) const
    {
        return SendFifo(*this, eTypeLog::DEBUG, {nullptr, 0, nullptr});
    }

    /**
     * @brief 
     * 
     * @param format 
     * @param ... 
     * @return SendFifo 
     */
    SendFifo        operator()(const char * format, ...) const
    {
        char    buffer[LFORMAT_BUFFER_SIZE];
        va_list vargs;
        va_start(vargs, format);
        vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs);
        va_end(vargs);
        SendFifo fifo(*this, eTypeLog::DEBUG, {nullptr, 0, nullptr});
        fifo << buffer;
        return fifo;
    }

    /**
     * @brief 
     * 
     * @tparam T 
     * @param var 
     * @return SendFifo 
     */
    template<typename T>
    SendFifo        operator()(const T& var) const
    {
        SendFifo fifo(*this, eTypeLog::DEBUG, {nullptr, 0, nullptr});
        fifo << var;
        return fifo;
    }

    /**
     * @brief 
     * 
     * @param type 
     * @return SendFifo 
     */
    SendFifo        operator()(const eTypeLog &type) const
    {
        return SendFifo(*this, type, {nullptr, 0, nullptr});
    }

    /**
     * @brief 
     * 
     * @param type 
     * @param format 
     * @param ... 
     * @return SendFifo 
     */
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

    /**
     * @brief 
     * 
     * @tparam T 
     * @param type 
     * @param var 
     * @return SendFifo 
     */
    template<typename T>
    SendFifo        operator()(const eTypeLog &type, const T& var) const
    {
        SendFifo fifo(*this, type, {nullptr, 0, nullptr});
        fifo << var;
        return fifo;
    }

private:

    /**
     * @brief 
     * 
     * @param str 
     * @param type 
     * @param source 
     */
    void            sendToStream(const std::string str, const eTypeLog &type, const SourceInfos &source) const
    {
        const timeInfos timeInfos = getCurrentTimeInfos();
        if (_outStream != nullptr && _muted == false && _filter & type)
        {
            std::lock_guard<std::mutex> lockGuard(_mutex);
            *(_outStream) << prompt(this->_name, type, _mapCustomKey, timeInfos, source) << str << std::flush;
        }
        std::set<const Loggator*> tmpsetLog;
        tmpsetLog.insert(this);
        sendChild(tmpsetLog, *this, this->_name, type, timeInfos, source, str);
    }

    /**
     * @brief 
     * 
     * @param setLog 
     * @param loggator 
     * @param name 
     * @param type 
     * @param timeInfos 
     * @param source 
     * @param str 
     */
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

    /**
     * @brief 
     * 
     * @param infos 
     * @return std::string 
     */
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
        strftime(bufferFormatTime, LFORMAT_BUFFER_SIZE - 1, retStr.c_str(), infos.tm);
        return std::string(bufferFormatTime);
    }

    /**
     * @brief Get the Current Time Infos object
     * 
     * @return const timeInfos 
     */
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

    /**
     * @brief convert eTypeLog to const char *
     * 
     * @param type 
     * @return const char* 
     */
    const char      *typeToStr(const eTypeLog &type) const
    {
        switch (type)
        {
            case eTypeLog::DEBUG:
                return "DEBUG";
            case eTypeLog::INFO:
                return "INFO ";
            case eTypeLog::WARN:
                return "WARN ";
            case eTypeLog::ERROR:
                return "ERROR";
            case eTypeLog::CRIT:
                return "CRIT ";
            case eTypeLog::ALERT:
                return "ALERT";
            case eTypeLog::EMERG:
                return "EMERG";
            case eTypeLog::FATAL:
                return "FATAL";
            default:
                return "";
        }
    }

    /**
     * @brief 
     * 
     * @param mapCustomKey 
     * @param key 
     * @return std::string 
     */
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

    /**
     * @brief 
     * 
     * @param key 
     * @param value 
     * @return std::string 
     */
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

    /**
     * @brief 
     * 
     * @param name 
     * @param type 
     * @param mapCustomKey 
     * @param timeInfos 
     * @param source 
     * @return std::string 
     */
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

    /**
     * @brief singleton Map Mutex
     * 
     * @return std::mutex& 
     */
    static std::mutex &sMapMutex(void)
    {
        static std::mutex singletonMapMutex;
        return singletonMapMutex;
    }

    /**
     * @brief singleton Map Loggator
     * 
     * @return std::map<const std::string, Loggator*>& 
     */
    static std::map<const std::string, Loggator*> &sMapLoggators(void)
    {
        static std::map<const std::string, Loggator*> singletonMapLoggators;
        return singletonMapLoggators;
    }

    Loggator(const Loggator &) = delete;

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

#endif // _LOG_LOGGATOR_HPP_