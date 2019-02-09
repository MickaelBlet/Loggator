/**
 * @file loggator.hpp
 * @author Mickaël BLET
 * 
 */

#ifndef _LOG_LOGGATOR_HPP_
# define _LOG_LOGGATOR_HPP_

# include <cstdarg>         // va_list, va_start, va_end
# include <cstring>         // strrchr
# include <ctime>           // localtime_r / localtime_s

# include <algorithm>       // lexicographical_compare
# include <iostream>        // string, cerr
# include <fstream>         // ostream, ofstream
# include <sstream>         // ostringstream
# include <mutex>           // mutex, lock_guard
# include <thread>          // this_thread::get_id
# include <unordered_map>   // unordered_map
# include <map>             // map
# include <vector>          // vector
# include <set>             // set

// stack buffer size
# ifndef LOGGATOR_FORMAT_BUFFER_SIZE
#  define LOGGATOR_FORMAT_BUFFER_SIZE     1024
# endif
# ifndef LOGGATOR_FORMAT_KEY_BUFFER_SIZE
#  define LOGGATOR_FORMAT_KEY_BUFFER_SIZE 64
# endif

// default prompt format
# ifndef LOGGATOR_DEFAULT_TIME_FORMAT
#  define LOGGATOR_DEFAULT_TIME_FORMAT    "%y/%m/%d %X.%N"
# endif
# ifndef LOGGATOR_DEFAULT_FORMAT
#  define LOGGATOR_DEFAULT_FORMAT         "{TIME:" LOGGATOR_DEFAULT_TIME_FORMAT "} {TYPE:[%-5s]} {FILE:%s:}{LINE:%s:}{FUNC:%s: }{NAME:%s: }"
# endif

// default loggator name (at use LOGGATOR() macro)
# ifndef LOGGATOR_DEFAULT_LOGGATOR_NAME
#  define LOGGATOR_DEFAULT_LOGGATOR_NAME  "main"
# endif

/*****************************************************************************/

# ifndef __GNUC__
#  ifndef __attribute__
#   define __attribute__(X) /* do nothing */
#  endif
# endif

# define LOGGATOR_PRIMITIVE_CAT(x, y) x ## y
# define LOGGATOR_CAT(x, y) LOGGATOR_PRIMITIVE_CAT(x, y)

// max 19 args
# define LOGGATOR_NARGS_SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _0, N, ...) N
# define LOGGATOR_NARGS(...) LOGGATOR_NARGS_SEQ(__VA_ARGS__, 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, 1)
# define LOGGATOR_PRE_NARGS(...) LOGGATOR_NARGS(__VA_ARGS__)
# define LOGGAOTR_NOARGS() 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
# define LOGGATOR_MACRO_CHOOSER(macro_prefix, ...) LOGGATOR_CAT(macro_prefix, LOGGATOR_PRE_NARGS(LOGGAOTR_NOARGS __VA_ARGS__ ()))

// source info
# define LOGGATOR_SOURCEINFOS Log::Loggator::SourceInfos{__FILE__, __LINE__, __func__}

// format macro
# define LOGGATOR_SEND(_name, _typeLog, ...) Log::Loggator::getInstance(_name).send(Log::eTypeLog::_typeLog, LOGGATOR_SOURCEINFOS, ##__VA_ARGS__)

// Loggator macro
# define LOGGATOR(...)          LOGGATOR_MACRO_CHOOSER(LOGGATOR_, __VA_ARGS__)(__VA_ARGS__)
# define LOGGATOR_0()           Log::Loggator::getInstance(LOGGATOR_DEFAULT_LOGGATOR_NAME)
# define LOGGATOR_1(_name)      Log::Loggator::getInstance(_name)
# define LOGGATOR_X(_name, ...) LOGGATOR_SEND(_name, __VA_ARGS__)

namespace Log
{

/**
 * @brief enum type of log
 * 
 */
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

/**
 * @brief namespace for enum filter log
 * 
 */
namespace eFilterLog
{
enum : int
{
    NONE                    = static_cast<int>(eTypeLog::NONE),
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
    GREATER_FATAL           = NONE,
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
    LESS_DEBUG              = NONE,
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

    class Config
    {

    public:

        struct InsensitiveCompare
        {
            bool operator() (const std::string & s1, const std::string & s2) const;
        };

        typedef std::map<std::string, std::string, InsensitiveCompare> MapSection;
        typedef std::map<std::string, MapSection, InsensitiveCompare>  MapConfig;

        /**
         * @brief Construct a new Config object
         * 
         * @param filename 
         */
        Config(const std::string &filename);

        /**
         * @brief Destroy the Config object
         * 
         */
        ~Config(void);

        /*********************************************************************/

        /**
         * @brief read config in filename
         * 
         * @param filename 
         * @return true : config is read
         * @return false : config is not read
         */
        bool readFile(const std::string& filename);

        /**
         * @brief check if config is read
         * 
         * @return true : if config is read
         * @return false : if config is not read
         */
        bool isRead(void) const;

        /**
         * @brief Get the Config object
         * 
         * @return const MapConfig& 
         */
        const MapConfig &getConfig(void) const;

        /**
         * @brief Set the Loggator object from map section
         * 
         * @param loggator 
         * @param mapSection 
         */
        static void setLoggatorCommon(Loggator &loggator, const MapSection &mapSection);

        /**
         * @brief Set the Loggator object from map section
         * 
         * @param loggator 
         * @param mapSection 
         */
        static void setLoggatorChilds(Loggator &loggator, const MapSection &mapSection);

        Config(Config &src) = delete;
        Config &operator=(const Config &rhs) = delete;

    private:


        /**
         * @brief parse filter configuration
         * 
         * @param str 
         * @return int : filter
         */
        static int parseFilter(const std::string &str);

        /**
         * @brief parse open mode configuration
         * 
         * @param str 
         * @return std::ios::openmode 
         */
        static std::ios::openmode parseOpenMode(const std::string &str);

        /**
         * @brief parse open mode configuration
         * 
         * @param str 
         * @return std::ios::openmode 
         */
        static std::ostream &parseOutStream(const std::string &str);

        /**
         * @brief parse child configuration
         * 
         * @param str 
         * @return std::set<Loggator *> : list of child
         */
        static std::set<Loggator *> parseChild(const std::string &str);

        /**
         * @brief parse mute configuration
         * 
         * @param str 
         * @return true : if mute is true
         * @return false : if mute is false
         */
        static bool parseBool(const std::string &str);

        /**
         * @brief trim space characters at begin and end of string
         * 
         * @param str 
         * @return std::string : string without space characters at begin and end
         */
        static std::string parseSimpleTrim(const std::string &str);

        /**
         * @brief check if character is comment
         * 
         * @param c 
         * @return true : c is comment character
         * @return false : c is not comment character
         */
        bool isComment(const unsigned char &c) const;

        /**
         * @brief parse and add key in _mapConfig at currentSectionName
         * 
         * @param line 
         * @param currentSectionName 
         * @return true : if line is a key value
         * @return false : if line is not a key value
         */
        bool parseKey(const std::string &line, const std::string &currentSectionName);

        /**
         * @brief parse and get section name
         * 
         * @param line 
         * @param retSection : return name of section if found
         * @return true : if section found
         * @return false : if section not found
         */
        bool parseSection(const std::string &line, std::string *retSection) const;

        /**
         * @brief detect if line is empty or comment
         * 
         * @param line 
         * @return true : line is empty or comment
         * @return false : line is not empty or comment
         */
        bool emptyOrComment(const std::string &line) const;

        /**
         * @brief parse and fill the map from fileStream
         * 
         * @param fileStream 
         */
        void readStream(std::istream &fileStream);

        MapConfig   _mapConfig;
        bool        _isRead;

    }; // end class Config

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
        Stream(const Loggator &loggator, const eTypeLog &type = eTypeLog::DEBUG, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}, bool flush = false);

        /**
         * @brief Construct a new Send Stream object
         * 
         */
        Stream(Stream &&stream);

        /**
         * @brief Destroy the Send Stream object
         * 
         */
        ~Stream(void);

        /*********************************************************************/

        /**
         * @brief use str function of stringStream
         * 
         * @return std::string : copy of string from stringStream
         */
        std::string str(void) const;

        /**
         * @brief use write function of stringStream
         * 
         * @param cstr 
         * @param size 
         * @return Stream& : instance of current object
         */
        Stream &write(const char *cstr, std::streamsize size);

        /**
         * @brief override operator << to object
         * 
         * @param type : new type of instance
         * @return Stream& : instance of current object
         */
        Stream &operator<<(const eTypeLog &type);

        /**
         * @brief override operator << to object
         * 
         * @param sourceInfos : new sourceInfos of instance
         * @return Stream& : instance of current object
         */
        Stream &operator<<(const SourceInfos &sourceInfos);

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
        Stream &operator<<(std::ostream&(*manip)(std::ostream&));

        Stream(void) = delete;
        Stream(const Stream &) = delete;
        Stream &operator=(const Stream &) = delete;

    private:

        std::ostringstream  _cacheStream;
        const Loggator      &_log;
        eTypeLog            _type;
        SourceInfos         _sourceInfos;
        bool                _flush;
    }; // end class Stream

public:

    /**
     * @brief Construct a new Loggator object
     * 
     */
    Loggator(void);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param filter
     */
    Loggator(int filter);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param oStream 
     * @param filter 
     */
    Loggator(std::ostream &oStream, int filter = eFilterLog::ALL);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param path 
     * @param openMode 
     * @param filter 
     */
    Loggator(const std::string &name, const std::string &path, std::ofstream::openmode openMode = std::ofstream::app, int filter = eFilterLog::ALL);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param filter 
     */
    Loggator(const std::string &name, int filter);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param oStream 
     * @param filter 
     */
    Loggator(const std::string &name, std::ostream &oStream = std::cerr, int filter = eFilterLog::ALL);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param loggator 
     */
    Loggator(const std::string &name, Loggator &loggator);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param loggator 
     */
    Loggator(Loggator &loggator);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param loggator 
     */
    Loggator(const std::string &name, const Loggator &loggator);

    /**
     * @brief Construct a new Loggator object
     * 
     * @param loggator 
     */
    Loggator(const Loggator &loggator);

    /**
     * @brief override operator = to object
     * 
     * @param rhs 
     * @return Loggator& 
     */
    Loggator &operator=(const Loggator &rhs);

    /**
     * @brief Destroy the Loggator object
     * 
     */
    ~Loggator(void);

    /*************************************************************************/

    /**
     * @brief Get the Instance Loggator object by name
     * throw out_of_range if instance not found
     * 
     * @param name 
     * @return Loggator& 
     */
    static Loggator &getInstance(const std::string &name);

    /**
     * @brief Get the Instance Loggator
     * juste for simply use macro LOGGATOR
     * 
     * @param loggator 
     * @return Loggator& 
     */
    static Loggator &getInstance(Loggator &loggator);

    /**
     * @brief Load config from filename
     * create loggators and store in unique_ptr
     * 
     * @param filename 
     * @return true : config is read
     * @return false : config is not read
     */
    static bool openConfig(const std::string &filename);

    /*************************************************************************/

    /**
     * @brief Set the Out Stream object
     * 
     * @param os 
     * @return Loggator& 
     */
    Loggator &setOutStream(std::ostream &os);

    /**
     * @brief Set the Name object
     * 
     * @param name 
     * @return Loggator& 
     */
    Loggator &setName(const std::string &name);

    /**
     * @brief Set the Filter object
     * 
     * @param filter 
     * @return Loggator& 
     */
    Loggator &setFilter(int filter);

    /**
     * @brief Add a Filter object
     * 
     * @param filter 
     * @return Loggator& 
     */
    Loggator &addFilter(int filter);

    /**
     * @brief Sub a Filter object
     * 
     * @param filter 
     * @return Loggator& 
     */
    Loggator &subFilter(int filter);


    /**
     * @brief Set the Flush Filter object
     * 
     * @param flushFilter 
     * @return Loggator& 
     */
    Loggator &setFlushFilter(int flushFilter);

    /**
     * @brief Add a Flush Filter object
     * 
     * @param flushFilter 
     * @return Loggator& 
     */
    Loggator &addFlushFilter(int flushFilter);

    /**
     * @brief Sub a Flush Filter object
     * 
     * @param flushFilter 
     * @return Loggator& 
     */
    Loggator &subFlushFilter(int flushFilter);

    /**
     * @brief Add child in list of object
     *        Add parent in list of @param
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator &addChild(Loggator &loggator);

    /**
     * @brief Sub child in list of object
     *        Sub parent in list of @param
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator &subChild(Loggator &loggator);

    /**
     * @brief Add child in list of @param
     *        Add parent in list of object
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator &listen(Loggator &loggator);

    /**
     * @brief Sub child in list of @param
     *        Sub parent in list of object
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator &unlisten(Loggator &loggator);

    /**
     * @brief Set the Key object
     * 
     * @param key   : name of key
     * @param value : value of key
     * @return Loggator& : instance of current object
     */
    Loggator &setKey(const std::string &key, const std::string &value, bool overrideDefaultKey = false);

    /**
     * @brief Set the Mute object
     * 
     * @param mute 
     * @return Loggator& 
     */
    Loggator &setMute(bool mute);

    /**
     * @brief Set the Format New Line object
     * 
     * @param formatNewLine 
     * @return Loggator& 
     */
    Loggator &setFormatNewLine(bool formatNewLine);

    /**
     * @brief Set the Format object
     * 
     * @param format 
     * @return Loggator& 
     */
    Loggator &setFormat(const std::string &format);

    /**
     * @brief Get the Name object
     * 
     * @return const std::string& 
     */
    const std::string &getName(void) const;

    /**
     * @brief Check if object is mute
     * 
     * @return true  : is mute
     * @return false : is not mute
     */
    bool isMute(void) const;

    /**
     * @brief Open a file 
     * if a file is allready open, that will be closed.
     * 
     * @param path 
     * @param openMode 
     * @return true  : file is opened
     * @return false : file is not opened
     */
    bool open(const std::string &path, std::ofstream::openmode openMode = std::ofstream::app);

    /**
     * @brief Close file if file is open
     * 
     */
    void close(void);

    /**
     * @brief Check if file is open
     * 
     * @return true  : if file is open
     * @return false : if file is not open
     */
    bool isOpen(void) const;

    /**
     * @brief flush this loggator and child
     * 
     */
    void flush(void) const;

    /*************************************************************************/

    /**
     * @brief Function send without argument
     * 
     * @return Stream : temporary instance of Stream
     */
    Stream send(void) const;

    /**
     * @brief Function send with type of log and format style printf
     * 
     * @param type 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream send(const eTypeLog &type, const char *format, ...) const __attribute__((__format__(__printf__, 3, 4)));

    /**
     * @brief Function send with type, SourceInfos and format style printf for macro functions
     * 
     * @param type 
     * @param sourceInfos 
     * @return Stream : temporary instance of Stream
     */
    Stream send(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) const;

    /**
     * @brief Function send with type, SourceInfos and var
     * 
     * @tparam T 
     * @param type 
     * @param sourceInfos 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream send(const eTypeLog &type, const SourceInfos &sourceInfos, const T &var) const
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
    Stream send(const eTypeLog &type, const SourceInfos &sourceInfos, const char *format, ...) const __attribute__((__format__(__printf__, 4, 5)));

    #define LOGGATOR_FUNCTION_TYPE(_type, _func)                                                                                \
    Stream _func(void) const;                                                                                                   \
    Stream _func(const char *format, ...) const __attribute__((__format__(__printf__, 2, 3)));                                  \
    Stream _func(const SourceInfos &sourceInfos, const char *format, ...) const __attribute__((__format__(__printf__, 3, 4)));  \
    template<typename T>                                                                                                        \
    Stream _func(const T& var) const                                                                                            \
    {                                                                                                                           \
        Stream stream(*this, eTypeLog::_type);                                                                                  \
        stream << var;                                                                                                          \
        return stream;                                                                                                          \
    }                                                                                                                           \
    template<typename T>                                                                                                        \
    Stream _func(const SourceInfos &sourceInfos, const T &var) const                                                            \
    {                                                                                                                           \
        Stream stream(*this, eTypeLog::_type, sourceInfos);                                                                     \
        stream << var;                                                                                                          \
        return stream;                                                                                                          \
    }

    LOGGATOR_FUNCTION_TYPE(DEBUG, debug);
    LOGGATOR_FUNCTION_TYPE(INFO,  info);
    LOGGATOR_FUNCTION_TYPE(WARN,  warn);
    LOGGATOR_FUNCTION_TYPE(WARN,  warning);
    LOGGATOR_FUNCTION_TYPE(ERROR, error);
    LOGGATOR_FUNCTION_TYPE(CRIT,  crit);
    LOGGATOR_FUNCTION_TYPE(CRIT,  critical);
    LOGGATOR_FUNCTION_TYPE(ALERT, alert);
    LOGGATOR_FUNCTION_TYPE(EMERG, emerg);
    LOGGATOR_FUNCTION_TYPE(EMERG, emergency);
    LOGGATOR_FUNCTION_TYPE(FATAL, fatal);

    #undef LOGGATOR_FUNCTION_TYPE

    /*************************************************************************/

    /**
     * @brief override operator << to object
     * 
     * @param type 
     * @return Stream : temporary instance of Stream
     */
    Stream operator<<(const eTypeLog &type) const;

    /**
     * @brief overide operator << to object
     * 
     * @param manip function pointer (std::endl, std::flush, ...)
     * @return Stream : temporary instance of Stream
     */
    Stream operator<<(std::ostream&(*manip)(std::ostream&)) const;

    /**
     * @brief override operator << to object
     * 
     * @tparam T 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream operator<<(const T &var) const
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
    Stream operator()(void) const;

    /**
     * @brief override operator () to object
     * 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream operator()(const char * format, ...) const __attribute__((__format__(__printf__, 2, 3)));

    /**
     * @brief override operator () to object
     * 
     * @tparam T 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream operator()(const T &var) const
    {
        Stream stream(*this);
        stream << var;
        return stream;
    }

    /**
     * @brief override operator () to object
     * 
     * @param type 
     * @param sourceInfos 
     * @return Stream : temporary instance of Stream
     */
    Stream operator()(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) const;

    /**
     * @brief override operator () to object
     * 
     * @param type 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream operator()(const eTypeLog &type, const char * format, ...) const __attribute__((__format__(__printf__, 3, 4)));

    /**
     * @brief override operator () to object
     * 
     * @param type 
     * @param sourceInfos 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream operator()(const eTypeLog &type, const SourceInfos &sourceInfos, const char * format, ...) const __attribute__((__format__(__printf__, 4, 5)));

    /**
     * @brief override operator () to object
     * 
     * @tparam T 
     * @param type 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream operator()(const eTypeLog &type, const T &var) const
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
     * @param sourceInfos 
     * @param var 
     * @return Stream : temporary instance of Stream
     */
    template<typename T>
    Stream operator()(const eTypeLog &type, const SourceInfos &sourceInfos, const T& var) const
    {
        Stream stream(*this, type, sourceInfos);
        stream << var;
        return stream;
    }

protected:

    /**
     * struct tm  tm   : stock struct tm
     * char[7]    msec : stock microseconds of time
     */
    struct TimeInfo
    {
        struct tm   tm;
        char        msec[7];
    };

    /**
     * @brief write in _outStream and _outStream_child
     * 
     * @param str 
     * @param type 
     * @param source 
     */
    void sendToOutStream(const std::string &str, const eTypeLog &type, const SourceInfos &source, bool flush) const;

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
    void sendChild(std::set<const Loggator*> &setLog, const Loggator &loggator, const std::string &name, const eTypeLog &type, TimeInfo &timeInfo, const SourceInfos &source, std::string &stringThreadID, const std::string &str, bool flush) const;

    /**
     * @brief flush to childs
     * 
     * @param setLog 
     * @param loggator 
     * @param name 
     * @param type 
     * @param timeInfo 
     * @param source 
     * @param str 
     */
    void flushChild(std::set<const Loggator*> &setLog, const Loggator &loggator) const;

    /**
     * @brief get string format time from TimeInfo
     * 
     * @param timeInfo 
     * @return std::string 
     */
    std::string formatTime(TimeInfo &timeInfo) const;

    /**
     * @brief Get the Current TimeInfo object
     * 
     * @return TimeInfo 
     */
    void getCurrentTimeInfo(TimeInfo &timeInfo) const;

    /**
     * @brief Get the Thread Id object
     * 
     * @return threadId 
     */
    void getThreadId(std::string &threadId) const;

    /**
     * @brief convert eTypeLog to const char *
     * 
     * @param type 
     * @return const char* 
     */
    const char *typeToStr(const eTypeLog &type) const;

    /**
     * @brief convert format type
     * 
     * @param type 
     * @return std::string 
     */
    std::string typeFormatConvert(const std::string &type) const;

    /**
     * @brief get custom key with format
     * 
     * @param mapCustomValueKey 
     * @param threadId 
     * @param key 
     * @return std::string : format custom key
     */
    std::string formatCustomKey(const std::unordered_map<std::string, std::string> &mapCustomValueKey, const std::string &threadId, const std::string &key) const;

    /**
     * @brief get format of common key
     * 
     * @param key 
     * @param value 
     * @return std::string : format common key
     */
    std::string formatKey(const std::string &key, const std::string &value) const;

    /**
     * @brief get prompt from object _format
     * 
     * @param name 
     * @param type 
     * @param timeInfo 
     * @param source 
     * @param stringThreadID 
     * @param mapCustomValueKey 
     * @return std::string 
     */
    std::string prompt(const std::string &name, const eTypeLog &type, TimeInfo &timeInfo, const SourceInfos &source, std::string &stringThreadID, const std::unordered_map<std::string, std::string> &mapCustomValueKey) const;

    /**
     * @brief singleton mutex for static map Loggator
     * 
     * @return std::mutex& 
     */
    static std::mutex &sMapMutex(void);

    /**
     * @brief singleton static map Loggator
     * 
     * @return std::map<std::string, Loggator*>& 
     */
    static std::unordered_map<std::string, Loggator* > &sMapLoggators(void);

    std::string                                     _name;
    int                                             _filter;
    int                                             _flushFilter;
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
    bool                                            _formatNewLine;

}; // end class Loggator

} // end namespace Log

#endif // _LOG_LOGGATOR_HPP_