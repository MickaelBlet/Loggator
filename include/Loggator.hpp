/**
 * @brief 
 * 
 * @file Loggator.hpp
 * @author Blet Mickael
 * @date 2018-05-01
 */

#ifndef _LOG_LOGGATOR_HPP_
# define _LOG_LOGGATOR_HPP_

# include <cstdarg>         // va_list, va_start, va_end
# include <cstring>         // strrchr

# include <iostream>        // string, cerr
# include <fstream>         // ostream, ofstream
# include <sstream>         // ostringstream
# include <mutex>           // mutex, lock_guard
# include <thread>          // this_thread::get_id
# include <unordered_map>   // unordered_map
# include <vector>          // vector
# include <set>             // set

# define LFORMAT_BUFFER_SIZE     1024
# define LFORMAT_KEY_BUFFER_SIZE 64
# define LDEFAULT_TIME_FORMAT    "%x %X.%N"
# define LDEFAULT_FORMAT         "{TIME:" LDEFAULT_TIME_FORMAT "} {TYPE:[%-5s]} {FILE:%s:}{LINE:%s:}{FUNC:%s: }{NAME:%s: }"
# define LALWAYS_FORMAT_AT_NEWLINE

/*****************************************************************************/

# ifndef __GNUC__
#  ifndef __attribute__
#   define __attribute__(X) /* do nothing */
#  endif
# endif

# define LSOURCEINFOS Log::Loggator::SourceInfos{__FILE__, __LINE__, __func__}

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
# define LEMERGF(...)           LSENDF(EMERG,       __VA_ARGS__)
# define LEMERGENCYF(...)       LSENDF(EMERGENCY,   __VA_ARGS__)
# define LFATALF(...)           LSENDF(FATAL,       __VA_ARGS__)

// Loggator macro
# define LOGGATOR(...)          LMACRO_CHOOSER(LOGGATOR_, __VA_ARGS__)(__VA_ARGS__)
# define LOGGATOR_0()           Log::Loggator::getInstance("main")
# define LOGGATOR_1(_name)      Log::Loggator::getInstance(_name)
# define LOGGATOR_X(_name, ...) Log::Loggator::getInstance(_name).LSENDF(__VA_ARGS__)

// send macro
# define LSEND(...)             LMACRO_CHOOSER(LSEND_, __VA_ARGS__)(__VA_ARGS__)
# define LSEND_0()              send(Log::eTypeLog::DEBUG, LSOURCEINFOS)
# define LSEND_1(_typeLog)      send(Log::eTypeLog::_typeLog, LSOURCEINFOS)
# define LSEND_X(...)           LSENDF(__VA_ARGS__)

// type macro
# define LDEBUG(...)            LMACRO_CHOOSER(LDEBUG_, __VA_ARGS__)(__VA_ARGS__)
# define LDEBUG_0()             LSENDF(DEBUG)
# define LDEBUG_1(...)          LDEBUGF(__VA_ARGS__)
# define LDEBUG_X(...)          LDEBUGF(__VA_ARGS__)

# define LINFO(...)             LMACRO_CHOOSER(LINFO_, __VA_ARGS__)(__VA_ARGS__)
# define LINFO_0()              LSENDF(INFO)
# define LINFO_1(...)           LINFOF(__VA_ARGS__)
# define LINFO_X(...)           LINFOF(__VA_ARGS__)

# define LWARN(...)             LMACRO_CHOOSER(LWARN_, __VA_ARGS__)(__VA_ARGS__)
# define LWARNING(...)          LMACRO_CHOOSER(LWARN_, __VA_ARGS__)(__VA_ARGS__)
# define LWARN_0()              LSENDF(WARN)
# define LWARN_1(...)           LWARNF(__VA_ARGS__)
# define LWARN_X(...)           LWARNF(__VA_ARGS__)

# define LERROR(...)            LMACRO_CHOOSER(LERROR_, __VA_ARGS__)(__VA_ARGS__)
# define LERROR_0()             LSENDF(ERROR)
# define LERROR_1(...)          LERRORF(__VA_ARGS__)
# define LERROR_X(...)          LERRORF(__VA_ARGS__)

# define LCRIT(...)             LMACRO_CHOOSER(LCRIT_, __VA_ARGS__)(__VA_ARGS__)
# define LCRITICAL(...)         LMACRO_CHOOSER(LCRIT_, __VA_ARGS__)(__VA_ARGS__)
# define LCRIT_0()              LSENDF(CRIT)
# define LCRIT_1(...)           LCRITF(__VA_ARGS__)
# define LCRIT_X(...)           LCRITF(__VA_ARGS__)

# define LALERT(...)            LMACRO_CHOOSER(LALERT_, __VA_ARGS__)(__VA_ARGS__)
# define LALERT_0()             LSENDF(ALERT)
# define LALERT_1(...)          LALERTF(__VA_ARGS__)
# define LALERT_X(...)          LALERTF(__VA_ARGS__)

# define LEMERG(...)            LMACRO_CHOOSER(LEMERG_, __VA_ARGS__)(__VA_ARGS__)
# define LEMERGENCY(...)        LMACRO_CHOOSER(LEMERG_, __VA_ARGS__)(__VA_ARGS__)
# define LEMERG_0()             LSENDF(EMERG)
# define LEMERG_1(...)          LEMERGF(__VA_ARGS__)
# define LEMERG_X(...)          LEMERGF(__VA_ARGS__)

# define LFATAL(...)            LMACRO_CHOOSER(LFATAL_, __VA_ARGS__)(__VA_ARGS__)
# define LFATAL_0()             LSENDF(FATAL)
# define LFATAL_1(...)          LFATALF(__VA_ARGS__)
# define LFATAL_X(...)          LFATALF(__VA_ARGS__)

namespace Log
{

enum class eTypeLog: int
{
    NONE      = 0,
    DEBUG     = 1<<0,
    INFO      = 1<<1,
    WARN      = 1<<2,
    WARNING   = WARN,
    ERROR     = 1<<3,
    CRIT      = 1<<4,
    CRITICAL  = CRIT,
    ALERT     = 1<<5,
    EMERG     = 1<<6,
    EMERGENCY = EMERG,
    FATAL     = 1<<7
};

namespace eFilterLog
{
enum : int
{
    EQUAL_DEBUG             = static_cast<int>(eTypeLog::DEBUG),
    EQUAL_INFO              = static_cast<int>(eTypeLog::INFO),
    EQUAL_WARN              = static_cast<int>(eTypeLog::WARN),
    EQUAL_WARNING           = EQUAL_WARN,
    EQUAL_ERROR             = static_cast<int>(eTypeLog::ERROR),
    EQUAL_CRIT              = static_cast<int>(eTypeLog::CRIT),
    EQUAL_CRITICAL          = EQUAL_CRIT,
    EQUAL_ALERT             = static_cast<int>(eTypeLog::ALERT),
    EQUAL_EMERG             = static_cast<int>(eTypeLog::EMERG),
    EQUAL_EMERGENCY         = EQUAL_EMERG,
    EQUAL_FATAL             = static_cast<int>(eTypeLog::FATAL),
    ALL                     = EQUAL_DEBUG | EQUAL_INFO | EQUAL_WARN | EQUAL_ERROR | EQUAL_CRIT | EQUAL_ALERT | EQUAL_EMERG | EQUAL_FATAL,
    GREATER_FATAL           = static_cast<int>(eTypeLog::NONE),
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
    LESS_DEBUG              = static_cast<int>(eTypeLog::NONE),
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
}

class Loggator
{

public:

    struct SourceInfos
    {
        const char *filename;
        int         line;
        const char *func;
    };

private:

    /**
     * @brief class Stream
     * create a temporary object same ostringstream
     * at destruct send to loggator method (sendToStream)
     */
    class Stream
    {

    public:

        /**
         * @brief Construct a new Send Stream object
         * 
         * @param loggator 
         * @param type 
         * @param sourceInfos 
         */
        Stream(const Loggator &loggator, const eTypeLog &type = eTypeLog::DEBUG, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) noexcept:
        _log(loggator),
        _type(type),
        _sourceInfos(sourceInfos)
        {
            return ;
        }

        /**
         * @brief Construct a new Send Stream object
         * 
         */
        Stream(Stream &&) = default;

        /**
         * @brief Destroy the Send Stream object
         * 
         */
        ~Stream(void) noexcept
        {
            if (_type != eTypeLog::NONE)
            {
                std::string cacheStr = _cacheStream.str();
                if (cacheStr.back() != '\n')
                    cacheStr += '\n';
                _log.sendToOutStream(cacheStr, _type, _sourceInfos);
            }
            return ;
        }

        /*********************************************************************/

        /**
         * @brief use write function of stringStream
         * 
         * @param cstr 
         * @param size 
         * @return Stream& : instance of current object
         */
        Stream &write(const char *cstr, std::streamsize size)
        {
            _cacheStream.write(cstr, size);
            return *this;
        }

        /**
         * @brief override operator << to object
         * 
         * @param type : new type of instance
         * @return Stream& : instance of current object
         */
        Stream &operator<<(const eTypeLog &type)
        {
            _type = type;
            return *this;
        }

        /**
         * @brief override operator << to object
         * 
         * @param sourceInfos : new sourceInfos of instance
         * @return Stream& : instance of current object
         */
        Stream &operator<<(const SourceInfos &sourceInfos)
        {
            _sourceInfos = sourceInfos;
            return *this;
        }

        /**
         * @brief override operator << to object
         * 
         * @tparam T 
         * @param var 
         * @return Stream& : instance of current object
         */
        template<typename T>
        Stream &operator<<(const T &var)
        {
            _cacheStream << var;
            return *this;
        }

        /**
         * @brief overide operator << to object
         * 
         * @param manip : function pointer (std::endl, std::flush, ...)
         * @return Stream& : instance of current object
         */
        Stream &operator<<(std::ostream&(*manip)(std::ostream&))
        {
            manip(_cacheStream);
            return *this;
        }

    private:
        Stream(void) = delete;
        Stream(const Stream &) = delete;
        Stream &operator=(const Stream &) = delete;

        std::ostringstream  _cacheStream;
        const Loggator      &_log;
        eTypeLog            _type;
        SourceInfos         _sourceInfos;

    }; // end class Stream

public:

    /**
     * @brief Construct a new Loggator object
     * 
     */
    Loggator(void) noexcept:
    _name(std::string()),
    _filter(eFilterLog::ALL),
    _outStream(&std::cerr),
    _indexTimeNano(std::string::npos),
    _mute(false)
    {
        setFormat(LDEFAULT_FORMAT);
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param filter
     */
    Loggator(int filter) noexcept:
    _name(std::string()),
    _filter(filter),
    _outStream(&std::cerr),
    _indexTimeNano(std::string::npos),
    _mute(false)
    {
        setFormat(LDEFAULT_FORMAT);
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param oStream 
     * @param filter 
     */
    Loggator(std::ostream &oStream, int filter = eFilterLog::ALL) noexcept:
    _name(std::string()),
    _filter(filter),
    _outStream(&oStream),
    _indexTimeNano(std::string::npos),
    _mute(false)
    {
        setFormat(LDEFAULT_FORMAT);
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
    Loggator(const std::string &name, const std::string &path, std::ofstream::openmode openMode = std::ofstream::app, int filter = eFilterLog::ALL) noexcept:
    _name(name),
    _filter(filter),
    _fileStream(path, std::ofstream::out | openMode),
    _outStream(&std::cerr),
    _indexTimeNano(std::string::npos),
    _mute(false)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        setFormat(LDEFAULT_FORMAT);
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        // if name is not empty and not find in static list of loggators
        if (_name.empty() == false && sMapLoggators().find(_name) == sMapLoggators().end())
        {
            // add new loggator in static list
            sMapLoggators()[_name] = this;
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
    Loggator(const std::string &name, std::ostream &oStream = std::cerr, int filter = eFilterLog::ALL) noexcept:
    _name(name),
    _filter(filter),
    _outStream(&oStream),
    _indexTimeNano(std::string::npos),
    _mute(false)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        setFormat(LDEFAULT_FORMAT);
        // if name is not empty and not in static list of loggators
        if (_name.empty() == false && sMapLoggators().find(_name) == sMapLoggators().end())
        {
            // add new loggator in static list
            sMapLoggators()[_name] = this;
        }
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
    _indexTimeNano(loggator._indexTimeNano),
    _mute(loggator._mute)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logParents.insert(this);
        _logChilds.insert(&loggator);
        _mapCustomFormatKey = loggator._mapCustomFormatKey;
        _mapCustomValueKey = loggator._mapCustomValueKey;
        // if name is not empty and not in static list of loggators
        if (_name.empty() == false && sMapLoggators().find(_name) == sMapLoggators().end())
        {
            // add new loggator in static list
            sMapLoggators()[_name] = this;
        }
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param loggator 
     */
    Loggator(Loggator &loggator) noexcept:
    _name(std::string()),
    _filter(loggator._filter),
    _format(loggator._format),
    _outStream(nullptr),
    _indexTimeNano(loggator._indexTimeNano),
    _mute(loggator._mute)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logParents.insert(this);
        _logChilds.insert(&loggator);
        _mapCustomFormatKey = loggator._mapCustomFormatKey;
        _mapCustomValueKey = loggator._mapCustomValueKey;
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
        this->_indexTimeNano = rhs._indexTimeNano;
        this->_mute = rhs._mute;
        for (Loggator *child : rhs._logChilds)
        {
            std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
            child->_logParents.insert(this);
        }
        this->_logChilds = rhs._logChilds;
        this->_mapCustomFormatKey = rhs._mapCustomFormatKey;
        this->_mapCustomValueKey = rhs._mapCustomValueKey;
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
        // erase all link of object in childs
        for (Loggator *child : _logChilds)
        {
            std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
            child->_logParents.erase(this);
        }
        // erase all link of object in parents
        for (Loggator *parent : _logParents)
        {
            std::lock_guard<std::mutex> lockGuardParent(parent->_mutex);
            parent->_logChilds.erase(this);
        }
        if (_name.empty() == false)
        {
            std::unordered_map<std::string, Log::Loggator*>::iterator it = sMapLoggators().begin();
            while (it != sMapLoggators().end())
            {
                // erase link of object in static map
                if (it->second == this)
                {
                    sMapLoggators().erase(it);
                    break;
                }
                ++it;
            }
        }
        return ;
    }

    /*************************************************************************/

    /**
     * @brief Get the Instance Loggator object by name
     * throw out_of_range if instance not found
     * @param name 
     * @return Loggator& 
     */
    static Loggator &getInstance(const std::string &name)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        return *(sMapLoggators().at(name));
    }

    /**
     * @brief Set the Out Stream object
     * 
     * @param os 
     */
    void            setOutStream(std::ostream &os)
    {
        close();
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
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_name.empty() == false)
        {
            std::unordered_map<std::string, Log::Loggator*>::iterator it = sMapLoggators().begin();
            while (it != sMapLoggators().end())
            {
                if (it->second == this)
                {
                    sMapLoggators().erase(it);
                    break;
                }
                ++it;
            }
        }
        _name = name;
        // if name is not empty and not in static list of loggators
        if (_name.empty() == false && sMapLoggators().find(_name) == sMapLoggators().end())
        {
            // add new loggator in static list
            sMapLoggators()[_name] = this;
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
     * @brief Add ta Filter object
     * 
     * @param filter 
     */
    void            addFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter |= filter;
    }

    /**
     * @brief Sub a Filter object
     * 
     * @param filter 
     */
    void            subFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter &= ~filter;
    }

    /**
     * @brief Add child in list of object
     *        Add parent in list of @param
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator        &addChild(Loggator &loggator)
    {
        if (&loggator == this)
            return *this;
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        _logChilds.insert(&loggator);
        loggator._logParents.insert(this);
        return *this;
    }

    /**
     * @brief Sub child in list of object
     *        Sub parent in list of @param
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator        &subChild(Loggator &loggator)
    {
        if (&loggator == this)
            return *this;
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        _logChilds.erase(&loggator);
        loggator._logParents.erase(this);
        return *this;
    }

    /**
     * @brief Add child in list of @param
     *        Add parent in list of object
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator        &listen(Loggator &loggator)
    {
        if (&loggator == this)
            return *this;
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logChilds.insert(this);
        _logParents.insert(&loggator);
        return *this;
    }

    /**
     * @brief Sub child in list of @param
     *        Sub parent in list of object
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator        &unlisten(Loggator &loggator)
    {
        if (&loggator == this)
            return *this;
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        loggator._logChilds.erase(this);
        _logParents.erase(&loggator);
        return *this;
    }

    /**
     * @brief Set the Key object
     * 
     * @param key   : name of key
     * @param value : value of key
     * @return Loggator& : instance of current object
     */
    Loggator        &setKey(const std::string &key, const std::string &value, bool overrideDefaultKey = false)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (overrideDefaultKey || _mapCustomValueKey.find(key) == _mapCustomValueKey.end())
        {
            // add new value key in custom key map
            _mapCustomValueKey[key] = value;
        }
        std::stringstream streamThreadIDKey;
        streamThreadIDKey << std::hex << std::uppercase << std::this_thread::get_id() << key;
        std::string threadIDKey = std::move(streamThreadIDKey.str());
        _mapCustomValueKey[threadIDKey] = value;
        return *this;
    }

    /**
     * @brief Set the Mute object
     * 
     * @param mute 
     */
    void            setMute(bool mute)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _mute = mute;
    }

    /**
     * @brief Set the Format object
     * 
     * @param format 
     */
    void            setFormat(const std::string &format)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _mapIndexFormatKey.clear();
        _format = format;
        // escape character and replace code character
        for (std::size_t i = 0; i < _format.size(); i++)
        {
            if (i > 0 && _format[i - 1] == '\\')
            {
                if (_format[i] == '\\')
                    _format.erase(i - 1, 1);
                else
                    _format.erase(--i, 1);
            }
            else if (_format[i] == '{')
            {
                _format[i] = -41;
            }
            else if (_format[i] == '}')
            {
                _format[i] = -42;
            }
            else if (_format[i] == ':')
            {
                _format[i] = -43;
            }
        }
        // search first occurrence of '{'
        std::size_t indexStart = _format.find(-41);
        std::size_t indexEnd;
        std::size_t indexFormat;
        while (indexStart != std::string::npos)
        {
            // search first occurrence of '}' after indexStart
            indexEnd = _format.find(-42, indexStart);
            if (indexEnd == std::string::npos)
                break;
            // search first occurrence of ':' after indexStart
            indexFormat = _format.find(-43, indexStart);
            // if ':' not found or ':' is not between '{' and '}'
            if (indexFormat == std::string::npos || indexFormat > indexEnd)
            {
                // get name of key
                const std::string &key = _format.substr(indexStart + 1, indexEnd - indexStart - 1);
                // if key is specific "TIME" set default format
                if (key == "TIME")
                {
                    _mapCustomFormatKey[key] = LDEFAULT_TIME_FORMAT;
                    _indexTimeNano = _mapCustomFormatKey.at(key).find("%N");
                    if (_indexTimeNano != std::string::npos)
                        _mapCustomFormatKey.at(key).erase(_indexTimeNano, 2);
                }
                else
                {
                    _mapCustomFormatKey[key] = "%s";
                }
                _mapIndexFormatKey.push_back(std::move(std::pair<std::string, std::size_t>(key, indexStart)));
                // erase the full key in string object _format [{...}]
                _format.erase(indexStart, indexEnd - indexStart + 1);
                // jump to next occurrence '{' after indexStart + 1
                indexStart = _format.find(-41, indexStart);
                continue;
            }
            // get name of key {[...]:...}
            const std::string &key = _format.substr(indexStart + 1, indexFormat - indexStart - 1);
            // get format of key {...:[...]}
            std::string formatKey = _format.substr(indexFormat + 1, indexEnd - indexFormat - 1);
            // replace no print character by real
            for (char &c : formatKey)
            {
                if      (c == -41)
                    c = '{';
                else if (c == -42)
                    c = '}';
                else if (c == -43)
                    c = ':';
            }
            // add new format key in custom key map
            _mapCustomFormatKey[key] = std::move(formatKey);
            // if key is specific "TIME" get index of time nano
            if (key == "TIME")
            {
                _indexTimeNano = _mapCustomFormatKey.at(key).find("%N");
                if (_indexTimeNano != std::string::npos)
                    _mapCustomFormatKey.at(key).erase(_indexTimeNano, 2);
            }
            _mapIndexFormatKey.push_back(std::move(std::pair<std::string, std::size_t>(key, indexStart)));
            // erase the full key in string object _format [{...:...}]
            _format.erase(indexStart, indexEnd - indexStart + 1);
            // jump to next occurrence '{' after indexStart + 1
            indexStart = _format.find(-41, indexStart);
        }
        // replace no print character by real
        bool inBracette = false;
        for (char &c : _format)
        {
            if (inBracette == false)
            {
                if      (c == -41)
                    inBracette = true;
                else if (c == -42)
                    c = '}';
                else if (c == -43)
                    c = ':';
            }
            else if (c == -42)
            {
                inBracette = false;
            }
        }
    }

    /**
     * @brief Check if object is mute
     * 
     * @return true  : is mute
     * @return false : is not mute
     */
    bool            isMute(void) const
    {
        return _mute;
    }

    /**
     * @brief Open a file 
     * if a file is allready open, that will be closed.
     * 
     * @param path 
     * @param openMode 
     * @return true  : file is opened
     * @return false : file is not opened
     */
    bool            open(const std::string &path, std::ofstream::openmode openMode = std::ofstream::app)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_fileStream.is_open())
        {
            // close the preview file
            _fileStream.close();
            // clear the cache of _fileStream
            _fileStream.clear();
        }
        _fileStream.open(path, std::ofstream::out | openMode);
        if (_fileStream.is_open())
            _outStream = &_fileStream;
        else
            _outStream = &std::cerr;
        return _fileStream.is_open();
    }

    /**
     * @brief Close file if file is open
     * 
     */
    void            close(void)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_fileStream.is_open())
        {
            // close the preview file
            _fileStream.close();
            // clear the cache of _fileStream
            _fileStream.clear();
        }
        _outStream = &std::cerr;
    }

    /**
     * @brief Check if file is open
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
     * @brief Function send without argument
     * 
     * @return Stream : temporary instance of Stream
     */
    Stream          send(void) const
    {
        return Stream(*this);
    }

    /**
     * @brief Function send with type of log and format style printf
     * 
     * @param type 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream          send(const eTypeLog &type, const char *format, ...) const __attribute__((__format__(__printf__, 3, 4)))
    {
        char        buffer[LFORMAT_BUFFER_SIZE];
        Stream      stream(*this, type);
        va_list     vargs;

        va_start(vargs, format);
        stream.write(buffer, std::vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs));
        va_end(vargs);

        return stream;
    }

    /**
     * @brief Function send with type, SourceInfos and format style printf for macro functions
     * 
     * @param type 
     * @param sourceInfos 
     * @return Stream : temporary instance of Stream
     */
    Stream          send(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) const
    {
        return Stream(*this, type, sourceInfos);
    }

    /**
     * @brief Function send with type, SourceInfos and format style printf for macro functions
     * 
     * @param type 
     * @param sourceInfos 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream          send(const eTypeLog &type, const SourceInfos &sourceInfos, const T &var) const
    {
        Stream stream(*this, type, sourceInfos);
        stream << var;
        return stream;
    }

    /**
     * @brief Function send with type and SourceInfos for macro functions
     * and format style printf
     * 
     * @param type 
     * @param sourceInfos 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream          send(const eTypeLog &type, const SourceInfos &sourceInfos, const char *format, ...) const __attribute__((__format__(__printf__, 4, 5)))
    {
        char        buffer[LFORMAT_BUFFER_SIZE];
        Stream      stream(*this, type, sourceInfos);
        va_list     vargs;
        va_start(vargs, format);
        stream.write(buffer, std::vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs));
        va_end(vargs);
        return stream;
    }

    #define LFUNCTION_TYPE(_type, _func)                                                                \
    Stream          _func(void) const                                                                   \
    {                                                                                                   \
        return Stream(*this, eTypeLog::_type);                                                          \
    }                                                                                                   \
    Stream          _func(const char *format, ...) const __attribute__((__format__(__printf__, 2, 3)))  \
    {                                                                                                   \
        char        buffer[LFORMAT_BUFFER_SIZE];                                                        \
        Stream      stream(*this, eTypeLog::_type);                                                     \
        va_list     vargs;                                                                              \
        va_start(vargs, format);                                                                        \
        stream.write(buffer, std::vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs));           \
        va_end(vargs);                                                                                  \
        return stream;                                                                                  \
    }                                                                                                   \
    template<typename T>                                                                                \
    Stream          _func(const T& var) const                                                           \
    {                                                                                                   \
        Stream stream(*this, eTypeLog::_type);                                                          \
        stream << var;                                                                                  \
        return stream;                                                                                  \
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

    #undef LFUNCTION_TYPE

    /*************************************************************************/

    /**
     * @brief override operator << to object
     * 
     * @param type 
     * @return Stream : temporary instance of Stream
     */
    Stream          operator<<(const eTypeLog &type) const
    {
        return Stream(*this, type);
    }

    /**
     * @brief override operator << to object
     * 
     * @param sourceInfos 
     * @return Stream : temporary instance of Stream
     */
    Stream          operator<<(const SourceInfos &sourceInfos) const
    {
        return Stream(*this, eTypeLog::DEBUG, sourceInfos);
    }

    /**
     * @brief overide operator << to object
     * 
     * @param manip function pointer (std::endl, std::flush, ...)
     * @return Stream : temporary instance of Stream
     */
    Stream          operator<<(std::ostream&(*manip)(std::ostream&)) const
    {
        Stream stream(*this);
        stream << manip;
        return stream;
    }

    /**
     * @brief override operator << to object
     * 
     * @tparam T 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream          operator<<(const T &var) const
    {
        Stream stream(*this);
        stream << var;
        return stream;
    }

    /**
     * @brief override operator () to object
     * 
     * @return Stream : temporary instance of Stream
     */
    Stream          operator()(void) const
    {
        return Stream(*this);
    }

    /**
     * @brief override operator () to object
     * 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream          operator()(const char * format, ...) const __attribute__((__format__(__printf__, 2, 3)))
    {
        char        buffer[LFORMAT_BUFFER_SIZE];
        Stream      stream(*this);
        va_list     vargs;
        va_start(vargs, format);
        stream.write(buffer, std::vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs));
        va_end(vargs);
        return stream;
    }

    /**
     * @brief override operator () to object
     * 
     * @tparam T 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream          operator()(const T& var) const
    {
        Stream stream(*this);
        stream << var;
        return stream;
    }

    /**
     * @brief 
     * 
     * @param type 
     * @param sourceInfos 
     * @return Stream : temporary instance of Stream
     */
    Stream          operator()(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) const
    {
        return Stream(*this, type, sourceInfos);
    }

    /**
     * @brief override operator () to object
     * 
     * @param sourceInfos 
     * @return Stream : temporary instance of Stream
     */
    Stream          operator()(const SourceInfos &sourceInfos) const
    {
        return Stream(*this, eTypeLog::DEBUG, sourceInfos);
    }

    /**
     * @brief override operator () to object
     * 
     * @param type 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream          operator()(const eTypeLog &type, const char * format, ...) const __attribute__((__format__(__printf__, 3, 4)))
    {
        char        buffer[LFORMAT_BUFFER_SIZE];
        Stream      stream(*this, type);
        va_list     vargs;
        va_start(vargs, format);
        stream.write(buffer, std::vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs));
        va_end(vargs);
        return stream;
    }

    /**
     * @brief override operator () to object
     * 
     * @param type 
     * @param sourceInfos 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream          operator()(const eTypeLog &type, const SourceInfos &sourceInfos, const char * format, ...) const __attribute__((__format__(__printf__, 4, 5)))
    {
        char        buffer[LFORMAT_BUFFER_SIZE];
        Stream      stream(*this, type, sourceInfos);
        va_list     vargs;
        va_start(vargs, format);
        stream.write(buffer, std::vsnprintf(buffer, LFORMAT_BUFFER_SIZE - 1, format, vargs));
        va_end(vargs);
        return stream;
    }

    /**
     * @brief override operator () to object
     * 
     * @tparam T 
     * @param type 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream          operator()(const eTypeLog &type, const T& var) const
    {
        Stream stream(*this, type);
        stream << var;
        return stream;
    }

    /**
     * @brief override operator () to object
     * 
     * @tparam T 
     * @param type 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream          operator()(const eTypeLog &type, const SourceInfos &sourceInfos, const T& var) const
    {
        Stream stream(*this, type, sourceInfos);
        stream << var;
        return stream;
    }

protected:

    /**
     * @brief 
     * struct tm  tm   : stock struct tm
     * long       msec : stock milisecond of time
     */
    struct TimeInfo
    {
        struct tm   tm;
        long        msec;
    };

    /**
     * @brief write in _outStream and _outStream_child
     * 
     * @param str 
     * @param type 
     * @param source 
     */
    void            sendToOutStream(const std::string &str, const eTypeLog &type, const SourceInfos &source) const
    {
        const TimeInfo timeInfo = getCurrentTimeInfo();
        // get thread id
        std::stringstream streamThreadID;
        streamThreadID << std::hex << std::uppercase << std::this_thread::get_id();
        const std::string &stringThreadID = streamThreadID.str();
        // std::string stringThreadID;
        if (_outStream != nullptr && _mute == false && _filter & static_cast<int>(type))
        {
            _mutex.lock();
            std::string tmpPrompt = prompt(this->_name, type, timeInfo, source, stringThreadID, _mapCustomValueKey);
            #ifdef LALWAYS_FORMAT_AT_NEWLINE
                std::size_t indexSub = 0;
                std::size_t indexNewLine = str.find('\n');
                while (indexNewLine != std::string::npos)
                {
                    (*_outStream) << tmpPrompt << str.substr(indexSub, ++indexNewLine - indexSub);
                    indexSub = indexNewLine;
                    indexNewLine = str.find('\n', indexSub);
                }
                (*_outStream) << std::flush;
            #else
                (*_outStream) << tmpPrompt << str << std::flush;
            #endif
            _mutex.unlock();
        }
        if (_logChilds.empty())
            return;
        std::set<const Loggator*> tmpsetLog = {this};
        sendChild(tmpsetLog, *this, this->_name, type, timeInfo, source, stringThreadID, str);
    }

    /**
     * @brief send to childs the same data
     * 
     * @param setLog 
     * @param loggator 
     * @param name 
     * @param type 
     * @param timeInfo 
     * @param source 
     * @param str 
     */
    void            sendChild(std::set<const Loggator*> &setLog, const Loggator &loggator, const std::string &name, const eTypeLog &type, const TimeInfo &timeInfo, const SourceInfos &source, const std::string &stringThreadID, const std::string &str) const
    {
        // create a cpy of LogChild for no dead lock
        loggator._mutex.lock();
        std::set<Loggator*> cpyLogChilds = loggator._logChilds;
        loggator._mutex.unlock();
        for (const Loggator *child : cpyLogChilds)
        {
            if (setLog.find(child) != setLog.end())
                continue;
            setLog.insert(child);
            if (child->_outStream != nullptr && child->_mute == false && child->_filter & static_cast<int>(type))
            {
                child->_mutex.lock();
                _mutex.lock();
                std::string tmpPrompt = child->prompt(name, type, timeInfo, source, stringThreadID, _mapCustomValueKey);
                _mutex.unlock();
                #ifdef LALWAYS_FORMAT_AT_NEWLINE
                    std::size_t indexSub = 0;
                    std::size_t indexNewLine = str.find('\n');
                    while (indexNewLine != std::string::npos)
                    {
                        (*child->_outStream) << tmpPrompt << str.substr(indexSub, ++indexNewLine - indexSub);
                        indexSub = indexNewLine;
                        indexNewLine = str.find('\n', indexSub);
                    }
                    (*child->_outStream) << std::flush;
                #else
                    (*child->_outStream) << tmpPrompt << str << std::flush;
                #endif
                child->_mutex.unlock();
            }
            if (child->_logChilds.empty())
                continue;
            sendChild(setLog, *child, name, type, timeInfo, source, stringThreadID, str);
        }
    }

    /**
     * @brief get string format time from TimeInfo
     * 
     * @param infos 
     * @return std::string 
     */
    std::string     formatTime(const TimeInfo &infos) const
    {
        char bufferFormatTime[LFORMAT_BUFFER_SIZE];

        std::string retStr = _mapCustomFormatKey.at("TIME");

        if (_indexTimeNano != std::string::npos)
        {
            std::snprintf(bufferFormatTime, 7, "%06ld", infos.msec);
            retStr.insert(_indexTimeNano, bufferFormatTime, 6);
        }
        return std::string(bufferFormatTime, 0, std::strftime(bufferFormatTime, LFORMAT_BUFFER_SIZE - 1, retStr.c_str(), &infos.tm));
    }

    /**
     * @brief Get the Current TimeInfo object
     * 
     * @return const TimeInfo 
     */
    TimeInfo        getCurrentTimeInfo(void) const
    {
        TimeInfo timeInfo;
        std::time_t timer;
        struct timespec ts;

        std::time(&timer);
        #ifdef __GNUC__
            localtime_r(&timer, &timeInfo.tm);
        #else
            localtime_s(&timeInfo.tm, &timer);
        #endif

        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
            timeInfo.msec = ts.tv_nsec / 1000;
        else
            timeInfo.msec = 0;
        return timeInfo;
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
                return "INFO";
            case eTypeLog::WARN:
                return "WARN";
            case eTypeLog::ERROR:
                return "ERROR";
            case eTypeLog::CRIT:
                return "CRIT";
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
     * @brief get custom key with format
     * 
     * @param mapCustomKey 
     * @param key 
     * @return std::string : format custom key
     */
    std::string     formatCustomKey(const std::unordered_map<std::string, std::string> &mapCustomValueKey, const std::string &threadId, const std::string &key) const
    {
        std::unordered_map<std::string, std::string>::const_iterator itFormatMap;
        itFormatMap = _mapCustomFormatKey.find(key);
        if (itFormatMap == _mapCustomFormatKey.end())
            return "";
        std::unordered_map<std::string, std::string>::const_iterator itValueMap;
        itValueMap = mapCustomValueKey.find(threadId + key);
        if (itValueMap == mapCustomValueKey.end())
        {
            itValueMap = mapCustomValueKey.find(key);
            if (itValueMap == mapCustomValueKey.end())
            {
                itValueMap = _mapCustomValueKey.find(threadId + key);
                if (itValueMap == _mapCustomValueKey.end())
                {
                    itValueMap = _mapCustomValueKey.find(key);
                    if (itValueMap == _mapCustomValueKey.end())
                        return "";
                }
            }
        }
        if (itValueMap->second.empty())
            return "";
        char buffer[LFORMAT_KEY_BUFFER_SIZE];
        return std::string(buffer, 0, std::snprintf(buffer, LFORMAT_KEY_BUFFER_SIZE - 1, itFormatMap->second.c_str(), itValueMap->second.c_str()));
    }

    /**
     * @brief get format of common key
     * 
     * @param key 
     * @param value 
     * @return std::string : format common key
     */
    std::string     formatKey(const std::string &key, const std::string &value) const
    {
        if (value.empty())
            return "";
        char buffer[LFORMAT_KEY_BUFFER_SIZE];
        return std::string(buffer, 0, std::snprintf(buffer, LFORMAT_KEY_BUFFER_SIZE - 1, _mapCustomFormatKey.at(key).c_str(), value.c_str()));
    }

    /**
     * @brief get prompt from object _format
     * 
     * @param name 
     * @param type 
     * @param mapCustomKey 
     * @param timeInfo 
     * @param source 
     * @return std::string 
     */
    std::string     prompt(const std::string &name, const eTypeLog &type, const TimeInfo &timeInfo, const SourceInfos &source, const std::string &stringThreadID, const std::unordered_map<std::string, std::string> &mapCustomValueKey) const
    {
        std::string prompt = _format;
        std::size_t index = 0;
        for (const std::pair<std::string, std::size_t> &itemFormatKey : _mapIndexFormatKey)
        {
            if (itemFormatKey.first == "TIME")
            {
                std::string tmpStr = formatTime(timeInfo);
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
            else if (itemFormatKey.first == "TYPE")
            {
                std::string tmpStr = formatKey(itemFormatKey.first, typeToStr(type));
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
            else if (itemFormatKey.first == "NAME")
            {
                if (name.empty())
                    continue;
                std::string tmpStr = formatKey(itemFormatKey.first, name);
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
            else if (itemFormatKey.first == "FUNC")
            {
                if (source.func == nullptr)
                    continue;
                std::string tmpStr = formatKey(itemFormatKey.first, source.func);
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
            else if (itemFormatKey.first == "PATH")
            {
                if (source.filename == nullptr)
                    continue;
                std::string tmpStr = formatKey(itemFormatKey.first, source.filename);
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
            else if (itemFormatKey.first == "FILE")
            {
                if (source.filename == nullptr)
                    continue;
                const char *rchr = strrchr(source.filename, '/');
                if (rchr == nullptr)
                    rchr = source.filename;
                else
                    ++rchr;
                std::string tmpStr = formatKey(itemFormatKey.first, rchr);
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
            else if (itemFormatKey.first == "LINE")
            {
                if (source.line == 0)
                    continue;
                std::string tmpStr = formatKey(itemFormatKey.first, std::to_string(source.line));
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
            else if (itemFormatKey.first == "THREAD_ID")
            {
                std::string tmpStr = formatKey(itemFormatKey.first, stringThreadID);
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
            else
            {
                std::string tmpStr = formatCustomKey(mapCustomValueKey, stringThreadID, itemFormatKey.first);
                prompt.insert(itemFormatKey.second + index, tmpStr);
                index += tmpStr.size();
            }
        }
        return prompt;
    }

    /**
     * @brief singleton mutex for static map Loggator
     * 
     * @return std::mutex& 
     */
    static std::mutex &sMapMutex(void)
    {
        static std::mutex singletonMapMutex;
        return singletonMapMutex;
    }

    /**
     * @brief singleton static map Loggator
     * 
     * @return std::map<std::string, Loggator*>& 
     */
    static std::unordered_map<std::string, Loggator* > &sMapLoggators(void)
    {
        static std::unordered_map<std::string, Loggator* > singletonMapLoggators;
        return singletonMapLoggators;
    }

    Loggator(const Loggator &) = delete;

    std::string                                     _name;
    int                                             _filter;
    std::string                                     _format;
    std::ofstream                                   _fileStream;
    std::ostream                                    *_outStream;
    std::set<Loggator*>                             _logParents;
    std::set<Loggator*>                             _logChilds;
    mutable std::mutex                              _mutex;
    std::size_t                                     _indexTimeNano;
    std::vector<std::pair<std::string, std::size_t>>_mapIndexFormatKey;
    std::unordered_map<std::string, std::string>    _mapCustomFormatKey;
    std::unordered_map<std::string, std::string>    _mapCustomValueKey;
    bool                                            _mute;

}; // end class Loggator

} // end namespace Log

#endif // _LOG_LOGGATOR_HPP_