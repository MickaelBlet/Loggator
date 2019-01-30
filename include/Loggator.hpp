/**
 * @author Mickaël BLET
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

    class Config
    {

    public:

        struct InsensitiveCompare
        {
            bool operator() (const std::string & s1, const std::string & s2) const
            {
                return std::lexicographical_compare(
                    s1.begin (), s1.end (),   // source range
                    s2.begin (), s2.end (),   // dest range
                    [](const unsigned char&c1, const unsigned char&c2) // lambda compare
                    {
                        return std::tolower(c1) < std::tolower(c2);
                    });  // comparison
            }
        };

        typedef std::map<std::string, std::string, InsensitiveCompare> MapSection;
        typedef std::map<std::string, MapSection, InsensitiveCompare>  MapConfig;

        /**
         * @brief Construct a new Config object
         * 
         * @param filename 
         */
        Config(const std::string &filename):
        _isRead(false)
        {
            readFile(filename);
        }

        /**
         * @brief Destroy the Config object
         * 
         */
        ~Config(void)
        {
            return ;
        }

        /**
         * @brief read config in filename
         * 
         * @param filename 
         * @return true : config is read
         * @return false : config is not read
         */
        bool readFile(const std::string& filename)
        {
            _isRead = false;

            std::ifstream fileStream(filename.c_str());

            if (fileStream.is_open() == false)
                return false;

            _mapConfig.clear();
            readStream(fileStream);
            fileStream.close();
            fileStream.clear();
            _isRead = true;
            return true;
        }

        /**
         * @brief check if config is read
         * 
         * @return true : if config is read
         * @return false : if config is not read
         */
        bool isRead(void) const
        {
            return _isRead;
        }

        /**
         * @brief Set the Loggator object from map section
         * 
         * @param loggator 
         * @param mapSection 
         */
        static void setLoggatorCommon(Loggator &loggator, const MapSection &mapSection)
        {
            // NAME
            MapSection::const_iterator itSection = mapSection.find("name");
            if (itSection != mapSection.end())
            {
                loggator.setName(itSection->second);
            }
            // FILTER
            itSection = mapSection.find("filter");
            if (itSection != mapSection.end())
            {
                loggator.setFilter(parseFilter(itSection->second));
            }
            // FLUSH FILTER
            itSection = mapSection.find("flushFilter");
            if (itSection != mapSection.end())
            {
                loggator.setFilter(parseFilter(itSection->second));
            }
            // FORMAT
            itSection = mapSection.find("format");
            if (itSection != mapSection.end())
            {
                InsensitiveCompare insensitiveCompare;
                if (insensitiveCompare(itSection->second, "DEFAULT"))
                    loggator.setFormat(LOGGATOR_DEFAULT_FORMAT);
                else
                    loggator.setFormat(itSection->second);
            }
            // FILE
            itSection = mapSection.find("file");
            if (itSection != mapSection.end())
            {
                // OPEN MODE
                MapSection::const_iterator itSectionOpenMode = mapSection.find("openMode");
                if (itSectionOpenMode != mapSection.end())
                    loggator.open(itSection->second, parseOpenMode(itSectionOpenMode->second));
                else
                    loggator.open(itSection->second);
            }
            else
            {
                // OUTSTREAM
                itSection = mapSection.find("outStream");
                if (itSection != mapSection.end())
                {
                    loggator.setOutStream(parseOutStream(itSection->second));
                }
            }
            // NEW LINE FORMAT
            itSection = mapSection.find("newLineFormat");
            if (itSection != mapSection.end())
            {
                loggator.setFormatNewLine(parseBool(itSection->second));
            }
            // MUTE
            itSection = mapSection.find("mute");
            if (itSection != mapSection.end())
            {
                loggator.setMute(parseBool(itSection->second));
            }
        }

        /**
         * @brief Set the Loggator object from map section
         * 
         * @param loggator 
         * @param mapSection 
         */
        static void setLoggatorChilds(Loggator &loggator, const MapSection &mapSection)
        {
            // CHILD
            MapSection::const_iterator itSection = mapSection.find("child");
            if (itSection != mapSection.end())
            {
                for (Loggator *child : parseChild(itSection->second))
                {
                    loggator.addChild(*child);
                }
            }
            // LISTEN
            itSection = mapSection.find("listen");
            if (itSection != mapSection.end())
            {
                for (Loggator *child : parseChild(itSection->second))
                {
                    loggator.listen(*child);
                }
            }
        }

        /**
         * @brief Get the Config object
         * 
         * @return const MapConfig& 
         */
        const MapConfig &getConfig(void) const
        {
            return _mapConfig;
        }

    private:

        Config(Config &src) = delete;
        Config &operator=(const Config &rhs) = delete;

        /**
         * @brief parse filter configuration
         * 
         * @param str 
         * @return int : filter
         */
        static int parseFilter(const std::string &str)
        {
            static std::map<std::string, int, InsensitiveCompare> mapStrToFilter = {
                {"DEBUG",                   eFilterLog::EQUAL_DEBUG},
                {"INFO",                    eFilterLog::EQUAL_INFO},
                {"WARN",                    eFilterLog::EQUAL_WARN},
                {"WARNING",                 eFilterLog::EQUAL_WARNING},
                {"ERROR",                   eFilterLog::EQUAL_ERROR},
                {"CRIT",                    eFilterLog::EQUAL_CRIT},
                {"CRITICAL",                eFilterLog::EQUAL_CRITICAL},
                {"ALERT",                   eFilterLog::EQUAL_ALERT},
                {"EMERG",                   eFilterLog::EQUAL_EMERG},
                {"EMERGENCY",               eFilterLog::EQUAL_EMERGENCY},
                {"FATAL",                   eFilterLog::EQUAL_FATAL},
                {"EQUAL_DEBUG",             eFilterLog::EQUAL_DEBUG},
                {"EQUAL_INFO",              eFilterLog::EQUAL_INFO},
                {"EQUAL_WARN",              eFilterLog::EQUAL_WARN},
                {"EQUAL_WARNING",           eFilterLog::EQUAL_WARNING},
                {"EQUAL_ERROR",             eFilterLog::EQUAL_ERROR},
                {"EQUAL_CRIT",              eFilterLog::EQUAL_CRIT},
                {"EQUAL_CRITICAL",          eFilterLog::EQUAL_CRITICAL},
                {"EQUAL_ALERT",             eFilterLog::EQUAL_ALERT},
                {"EQUAL_EMERG",             eFilterLog::EQUAL_EMERG},
                {"EQUAL_EMERGENCY",         eFilterLog::EQUAL_EMERGENCY},
                {"EQUAL_FATAL",             eFilterLog::EQUAL_FATAL},
                {"ALL",                     eFilterLog::ALL},
                {"GREATER_FATAL",           eFilterLog::GREATER_FATAL},
                {"GREATER_EMERG",           eFilterLog::GREATER_EMERG},
                {"GREATER_EMERGENCY",       eFilterLog::GREATER_EMERGENCY},
                {"GREATER_ALERT",           eFilterLog::GREATER_ALERT},
                {"GREATER_CRIT",            eFilterLog::GREATER_CRIT},
                {"GREATER_CRITICAL",        eFilterLog::GREATER_CRITICAL},
                {"GREATER_ERROR",           eFilterLog::GREATER_ERROR},
                {"GREATER_WARN",            eFilterLog::GREATER_WARN},
                {"GREATER_WARNING",         eFilterLog::GREATER_WARNING},
                {"GREATER_INFO",            eFilterLog::GREATER_INFO},
                {"GREATER_DEBUG",           eFilterLog::GREATER_DEBUG},
                {"GREATER_EQUAL_FATAL",     eFilterLog::GREATER_EQUAL_FATAL},
                {"GREATER_EQUAL_EMERG",     eFilterLog::GREATER_EQUAL_EMERG},
                {"GREATER_EQUAL_EMERGENCY", eFilterLog::GREATER_EQUAL_EMERGENCY},
                {"GREATER_EQUAL_ALERT",     eFilterLog::GREATER_EQUAL_ALERT},
                {"GREATER_EQUAL_CRIT",      eFilterLog::GREATER_EQUAL_CRIT},
                {"GREATER_EQUAL_CRITICAL",  eFilterLog::GREATER_EQUAL_CRITICAL},
                {"GREATER_EQUAL_ERROR",     eFilterLog::GREATER_EQUAL_ERROR},
                {"GREATER_EQUAL_WARN",      eFilterLog::GREATER_EQUAL_WARN},
                {"GREATER_EQUAL_WARNING",   eFilterLog::GREATER_EQUAL_WARNING},
                {"GREATER_EQUAL_INFO",      eFilterLog::GREATER_EQUAL_INFO},
                {"GREATER_EQUAL_DEBUG",     eFilterLog::GREATER_EQUAL_DEBUG},
                {"LESS_DEBUG",              eFilterLog::LESS_DEBUG},
                {"LESS_INFO",               eFilterLog::LESS_INFO},
                {"LESS_WARN",               eFilterLog::LESS_WARN},
                {"LESS_WARNING",            eFilterLog::LESS_WARNING},
                {"LESS_ERROR",              eFilterLog::LESS_ERROR},
                {"LESS_CRIT",               eFilterLog::LESS_CRIT},
                {"LESS_CRITICAL",           eFilterLog::LESS_CRITICAL},
                {"LESS_ALERT",              eFilterLog::LESS_ALERT},
                {"LESS_EMERG",              eFilterLog::LESS_EMERG},
                {"LESS_EMERGENCY",          eFilterLog::LESS_EMERGENCY},
                {"LESS_FATAL",              eFilterLog::LESS_FATAL},
                {"LESS_EQUAL_DEBUG",        eFilterLog::LESS_EQUAL_DEBUG},
                {"LESS_EQUAL_INFO",         eFilterLog::LESS_EQUAL_INFO},
                {"LESS_EQUAL_WARN",         eFilterLog::LESS_EQUAL_WARN},
                {"LESS_EQUAL_WARNING",      eFilterLog::LESS_EQUAL_WARNING},
                {"LESS_EQUAL_ERROR",        eFilterLog::LESS_EQUAL_ERROR},
                {"LESS_EQUAL_CRIT",         eFilterLog::LESS_EQUAL_CRIT},
                {"LESS_EQUAL_CRITICAL",     eFilterLog::LESS_EQUAL_CRITICAL},
                {"LESS_EQUAL_ALERT",        eFilterLog::LESS_EQUAL_ALERT},
                {"LESS_EQUAL_EMERG",        eFilterLog::LESS_EQUAL_EMERG},
                {"LESS_EQUAL_EMERGENCY",    eFilterLog::LESS_EQUAL_EMERGENCY},
                {"LESS_EQUAL_FATAL",        eFilterLog::LESS_EQUAL_FATAL}
            };

            int filter = 0;
            std::size_t indexSub = 0;
            std::size_t indexNewLine = str.find_first_of("|,+");
            if (indexNewLine == std::string::npos)
            {
                std::map<std::string, int, InsensitiveCompare>::const_iterator it = mapStrToFilter.find(str);
                if (it != mapStrToFilter.end())
                    filter = it->second;
                return filter;
            }
            while (indexNewLine != std::string::npos)
            {
                const std::string &strKey = parseSimpleTrim(str.substr(indexSub, ++indexNewLine - indexSub - 1));
                indexSub = indexNewLine;
                indexNewLine = str.find_first_of("|,+", indexSub);
                std::map<std::string, int, InsensitiveCompare>::const_iterator it = mapStrToFilter.find(strKey);
                if (it != mapStrToFilter.end())
                    filter |= it->second;
            }
            const std::string &strKey = parseSimpleTrim(str.substr(indexSub, ++indexNewLine - indexSub - 1));
            std::map<std::string, int, InsensitiveCompare>::const_iterator it = mapStrToFilter.find(strKey);
            if (it != mapStrToFilter.end())
                filter |= it->second;
            return filter;
        }

        /**
         * @brief parse open mode configuration
         * 
         * @param str 
         * @return std::ios::openmode 
         */
        static std::ios::openmode parseOpenMode(const std::string &str)
        {
            static std::map<std::string, std::ios::openmode, InsensitiveCompare> mapStrToOpenMode = {
                {"APP",      std::ios::app},
                {"APPEND",   std::ios::app},
                {"TRUNC",    std::ios::trunc},
                {"TRUNCATE", std::ios::trunc}
            };
            std::ios::openmode ret = std::ios::out;
            std::size_t indexSub = 0;
            std::size_t indexNewLine = str.find_first_of("|,+");
            if (indexNewLine == std::string::npos)
            {
                std::map<std::string, std::ios::openmode, InsensitiveCompare>::const_iterator it = mapStrToOpenMode.find(str);
                if (it != mapStrToOpenMode.end())
                    ret = it->second;
                return ret;
            }
            while (indexNewLine != std::string::npos)
            {
                const std::string &strKey = parseSimpleTrim(str.substr(indexSub, ++indexNewLine - indexSub - 1));
                indexSub = indexNewLine;
                indexNewLine = str.find_first_of("|,+", indexSub);
                std::map<std::string, std::ios::openmode, InsensitiveCompare>::const_iterator it = mapStrToOpenMode.find(strKey);
                if (it != mapStrToOpenMode.end())
                    ret |= it->second;
            }
            const std::string &strKey = parseSimpleTrim(str.substr(indexSub, ++indexNewLine - indexSub - 1));
            std::map<std::string, std::ios::openmode, InsensitiveCompare>::const_iterator it = mapStrToOpenMode.find(strKey);
            if (it != mapStrToOpenMode.end())
                ret |= it->second;
            return ret;
        }

        /**
         * @brief parse open mode configuration
         * 
         * @param str 
         * @return std::ios::openmode 
         */
        static std::ostream &parseOutStream(const std::string &str)
        {
            static std::map<std::string, std::ostream&, InsensitiveCompare> mapStrToOutStream = {
                {"COUT",      std::cout},
                {"STD::COUT", std::cout},
                {"STANDARD",  std::cout},
                {"CERR",      std::cerr},
                {"STD::CERR", std::cerr},
                {"ERROR",     std::cerr}
            };
            std::map<std::string, std::ostream&, InsensitiveCompare>::const_iterator it = mapStrToOutStream.find(str);
            if (it != mapStrToOutStream.end())
                return it->second;
            return std::cerr;
        }

        /**
         * @brief parse child configuration
         * 
         * @param str 
         * @return std::set<Loggator *> : list of child
         */
        static std::set<Loggator *> parseChild(const std::string &str)
        {
            std::set<Loggator *> setLog;
            std::size_t indexSub = 0;
            std::size_t indexNewLine = str.find_first_of("|,+");
            if (indexNewLine == std::string::npos)
            {
                if (str.empty() == false)
                {
                    Loggator &child = Loggator::getInstance(str);
                    setLog.insert(&child);
                }
                return setLog;
            }
            while (indexNewLine != std::string::npos)
            {
                const std::string &strKey = parseSimpleTrim(str.substr(indexSub, ++indexNewLine - indexSub - 1));
                indexSub = indexNewLine;
                indexNewLine = str.find_first_of("|,+", indexSub);
                if (strKey.empty() == false)
                { 
                    Loggator &child = Loggator::getInstance(strKey);
                    setLog.insert(&child);
                }
            }
            const std::string &strKey = parseSimpleTrim(str.substr(indexSub, ++indexNewLine - indexSub - 1));
            if (strKey.empty() == false)
            {
                Loggator &child = Loggator::getInstance(strKey);
                setLog.insert(&child);
            }
            return setLog;
        }

        /**
         * @brief parse mute configuration
         * 
         * @param str 
         * @return true : if mute is true
         * @return false : if mute is false
         */
        static bool parseBool(const std::string &str)
        {
            std::string arg = parseSimpleTrim(str);
            for (char &c : arg) {c = std::tolower(c);}
            if (arg == "0" || arg == "f" || arg == "false" || arg.empty())
                return false;
            return true;
        }

        /**
         * @brief trim space characters at begin and end of string
         * 
         * @param str 
         * @return std::string : string without space characters at begin and end
         */
        static std::string parseSimpleTrim(const std::string &str)
        {
            std::size_t start = 0;
            std::size_t end = str.size() - 1;
            while (isspace(str[start]))
                ++start;
            while (end > 0 && isspace(str[end]))
                --end;
            return str.substr(start, end - start + 1);
        }

        /**
         * @brief check if character is comment
         * 
         * @param c 
         * @return true : c is comment character
         * @return false : c is not comment character
         */
        bool isComment(const unsigned char &c) const
        {
            if (c == ';' || c == '#')
                return true;
            return false;
        }

        /**
         * @brief parse and add key in _mapConfig at currentSectionName
         * 
         * @param line 
         * @param currentSectionName 
         * @return true : if line is a key value
         * @return false : if line is not a key value
         */
        bool parseKey(const std::string &line, const std::string &currentSectionName)
        {
            std::size_t startKey;
            std::size_t endKey;
            std::size_t startValue;
            std::size_t endValue;
            std::size_t i = 0;

            while (isspace(line[i]))
                i++;
            if (line[i] == '=')
                return false;
            if (line[i] == '\"')
            {
                i++;
                startKey = i;
                while (line[i] != '\"')
                {
                    if (line[i] == '\0')
                        return false;
                    i++;
                }
                endKey = i;
            }
            else
            {
                startKey = i;
                while (line[i] != '=')
                {
                    if (line[i] == '\0')
                        return false;
                    i++;
                }
                i--;
                while (isspace(line[i]))
                    i--;
                i++;
                endKey = i;
            }
            while (isspace(line[i]))
                i++;
            while (line[i] != '=')
            {
                if (line[i] == '\0')
                    return false;
                i++;
            }
            i++;
            while (isspace(line[i]))
                i++;
            if (line[i] == '\"')
            {
                i++;
                startValue = i;
                while (line[i] != '\"')
                {
                    if (line[i] == '\0')
                        return false;
                    i++;
                }
                endValue = i;
                i++;
            }
            else
            {
                startValue = i;
                while (isComment(line[i]) != true && line[i] != '\0')
                    i++;
                i--;
                while (isspace(line[i]))
                    i--;
                i++;
                endValue = i;
            }
            while (isspace(line[i]))
                i++;
            if (isComment(line[i]) || line[i] == '\0')
            {
                _mapConfig[currentSectionName][line.substr(startKey, endKey - startKey)] = line.substr(startValue, endValue - startValue);
                return true;
            }
            return false;
        }

        /**
         * @brief parse and get section name
         * 
         * @param line 
         * @param retSection : return name of section if found
         * @return true : if section found
         * @return false : if section not found
         */
        bool parseSection(const std::string &line, std::string *retSection) const
        {
            std::size_t i = 0;
            while (isspace(line[i]))
                i++;
            if (line[i] != '[')
                return false;
            i++;
            while (isspace(line[i]))
                i++;
            std::size_t start = i;
            while (line[i] != ']')
            {
                if (line[i] == '\0')
                    return false;
                i++;
            }
            i--;
            while (isspace(line[i]))
                i--;
            i++;
            std::size_t end = i;
            while (line[i] != ']')
                i++;
            i++;
            while (isspace(line[i]))
                i++;
            if (isComment(line[i]) || line[i] == '\0')
            {
                *retSection = line.substr(start, end - start);
                return true;
            }
            return false;
        }

        /**
         * @brief detect if line is empty or comment
         * 
         * @param line 
         * @return true : line is empty or comment
         * @return false : line is not empty or comment
         */
        bool emptyOrComment(const std::string &line) const
        {
            std::size_t i = 0;
            while (isspace(line[i]))
                i++;
            if (isComment(line[i]) || line[i] == '\0')
                return true;
            return false;
        }

        /**
         * @brief parse and fill the map from fileStream
         * 
         * @param fileStream 
         */
        void readStream(std::istream &fileStream)
        {
            std::string currentSectionName = "";

            std::string line;
            while(std::getline(fileStream, line))
            {
                if (!emptyOrComment(line)
                &&  !parseSection(line, &currentSectionName))
                    parseKey(line, currentSectionName);
            }
        }

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
        Stream(const Loggator &loggator, const eTypeLog &type = eTypeLog::DEBUG, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}, bool flush = false):
        _log(loggator),
        _type(type),
        _sourceInfos(sourceInfos),
        _flush(flush)
        {
            return ;
        }

        /**
         * @brief Construct a new Send Stream object
         * 
         */
        Stream(Stream &&stream):
        _log(stream._log),
        _type(stream._type),
        _sourceInfos(stream._sourceInfos),
        _flush(stream._flush)
        {
            return ;
        }

        /**
         * @brief Destroy the Send Stream object
         * 
         */
        ~Stream(void)
        {
            if (_type != eTypeLog::NONE)
            {
                _cacheStream.put('\n');
                std::string cacheStr = _cacheStream.str();
                if (cacheStr.size() > 1 && cacheStr[cacheStr.size() - 2] == '\n')
                    cacheStr.pop_back();
                _log.sendToOutStream(cacheStr, _type, _sourceInfos, _flush);
            }
            return ;
        }

        /*********************************************************************/

        /**
         * @brief use str function of stringStream
         * 
         * @return std::string : copy of string from stringStream
         */
        std::string str(void) const
        {
            return _cacheStream.str();
        }

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
            if (manip == &(std::flush<char, std::char_traits<char>>)
            ||  manip == &(std::endl<char, std::char_traits<char>>))
            {
                _flush = true;
            }
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
        bool                _flush;
    }; // end class Stream

public:

    /**
     * @brief Construct a new Loggator object
     * 
     */
    Loggator(void):
    _name(std::string()),
    _filter(eFilterLog::ALL),
    _flushFilter(eFilterLog::ALL),
    _outStream(&std::cerr),
    _indexTimeNano(std::string::npos),
    _mute(false),
    _formatNewLine(true)
    {
        setFormat(LOGGATOR_DEFAULT_FORMAT);
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param filter
     */
    Loggator(int filter):
    _name(std::string()),
    _filter(filter),
    _flushFilter(eFilterLog::ALL),
    _outStream(&std::cerr),
    _indexTimeNano(std::string::npos),
    _mute(false),
    _formatNewLine(true)
    {
        setFormat(LOGGATOR_DEFAULT_FORMAT);
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param oStream 
     * @param filter 
     */
    Loggator(std::ostream &oStream, int filter = eFilterLog::ALL):
    _name(std::string()),
    _filter(filter),
    _flushFilter(eFilterLog::ALL),
    _outStream(&oStream),
    _indexTimeNano(std::string::npos),
    _mute(false),
    _formatNewLine(true)
    {
        setFormat(LOGGATOR_DEFAULT_FORMAT);
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
    Loggator(const std::string &name, const std::string &path, std::ofstream::openmode openMode = std::ofstream::app, int filter = eFilterLog::ALL):
    _name(name),
    _filter(filter),
    _flushFilter(eFilterLog::ALL),
    _fileStream(path, std::ofstream::out | openMode),
    _outStream(&std::cerr),
    _indexTimeNano(std::string::npos),
    _mute(false),
    _formatNewLine(true)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        setFormat(LOGGATOR_DEFAULT_FORMAT);
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
     * @param filter 
     */
    Loggator(const std::string &name, int filter):
    _name(name),
    _filter(filter),
    _flushFilter(eFilterLog::ALL),
    _outStream(&std::cerr),
    _indexTimeNano(std::string::npos),
    _mute(false),
    _formatNewLine(true)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        setFormat(LOGGATOR_DEFAULT_FORMAT);
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
     * @param oStream 
     * @param filter 
     */
    Loggator(const std::string &name, std::ostream &oStream = std::cerr, int filter = eFilterLog::ALL):
    _name(name),
    _filter(filter),
    _flushFilter(eFilterLog::ALL),
    _outStream(&oStream),
    _indexTimeNano(std::string::npos),
    _mute(false),
    _formatNewLine(true)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        setFormat(LOGGATOR_DEFAULT_FORMAT);
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
    Loggator(const std::string &name, Loggator &loggator):
    _name(name),
    _outStream(nullptr)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        _filter = loggator._filter;
        _flushFilter = loggator._flushFilter;
        _format = loggator._format;
        _indexTimeNano = loggator._indexTimeNano;
        _mute = loggator._mute;
        _formatNewLine = loggator._formatNewLine;
        loggator._logParents.insert(this);
        _logChilds.insert(&loggator);
        _mapIndexFormatKey = loggator._mapIndexFormatKey;
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
    Loggator(Loggator &loggator):
    _name(std::string()),
    _outStream(nullptr)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardChild(loggator._mutex);
        _filter = loggator._filter;
        _flushFilter = loggator._flushFilter;
        _format = loggator._format;
        _indexTimeNano = loggator._indexTimeNano;
        _mute = loggator._mute;
        _formatNewLine = loggator._formatNewLine;
        loggator._logParents.insert(this);
        _logChilds.insert(&loggator);
        _mapIndexFormatKey = loggator._mapIndexFormatKey;
        _mapCustomFormatKey = loggator._mapCustomFormatKey;
        _mapCustomValueKey = loggator._mapCustomValueKey;
        return ;
    }

    /**
     * @brief Construct a new Loggator object
     * 
     * @param name 
     * @param loggator 
     */
    Loggator(const std::string &name, const Loggator &loggator):
    _name(name),
    _outStream(&std::cerr)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardParent(loggator._mutex);
        _filter = loggator._filter;
        _flushFilter = loggator._flushFilter;
        _format = loggator._format;
        _indexTimeNano = loggator._indexTimeNano;
        _mute = loggator._mute;
        _formatNewLine = loggator._formatNewLine;
        for (Loggator *child : loggator._logChilds)
        {
            std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
            child->_logParents.insert(this);
        }
        _logChilds = loggator._logChilds;
        _mapIndexFormatKey = loggator._mapIndexFormatKey;
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
    Loggator(const Loggator &loggator):
    _name(std::string()),
    _outStream(&std::cerr)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::lock_guard<std::mutex> lockGuardParent(loggator._mutex);
        _filter = loggator._filter;
        _flushFilter = loggator._flushFilter;
        _format = loggator._format;
        _indexTimeNano = loggator._indexTimeNano;
        _mute = loggator._mute;
        _formatNewLine = loggator._formatNewLine;
        for (Loggator *child : loggator._logChilds)
        {
            std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
            child->_logParents.insert(this);
        }
        _logChilds = loggator._logChilds;
        _mapIndexFormatKey = loggator._mapIndexFormatKey;
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
    Loggator &operator=(const Loggator &rhs)
    {
        std::lock_guard<std::mutex> lockGuard(this->_mutex);
        std::lock_guard<std::mutex> lockGuardParent(rhs._mutex);
        this->_filter = rhs._filter;
        this->_flushFilter = rhs._flushFilter;
        this->_format = rhs._format;
        this->_indexTimeNano = rhs._indexTimeNano;
        this->_mute = rhs._mute;
        this->_formatNewLine = rhs._formatNewLine;
        for (Loggator *child : rhs._logChilds)
        {
            std::lock_guard<std::mutex> lockGuardChild(child->_mutex);
            child->_logParents.insert(this);
        }
        this->_logChilds = rhs._logChilds;
        this->_mapIndexFormatKey = rhs._mapIndexFormatKey;
        this->_mapCustomFormatKey = rhs._mapCustomFormatKey;
        this->_mapCustomValueKey = rhs._mapCustomValueKey;
        return *this;
    }

    /**
     * @brief Destroy the Loggator object
     * 
     */
    ~Loggator(void)
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
     * 
     * @param name 
     * @return Loggator& 
     */
    static Loggator &getInstance(const std::string &name)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        try
        {
            return *(sMapLoggators().at(name));
        }
        catch (std::out_of_range &e)
        {
            throw std::out_of_range("Loggator instance \"" + name + "\" not found (" + e.what() + ").");
        }
    }

    /**
     * @brief Get the Instance Loggator
     * juste for simply use macro LOGGATOR
     * 
     * @param loggator 
     * @return Loggator& 
     */
    static Loggator &getInstance(Loggator &loggator)
    {
        return loggator;
    }

    /**
     * @brief Load config from filename
     * create loggators and store in unique_ptr
     * 
     * @param filename 
     * @return true : config is read
     * @return false : config is not read
     */
    static bool openConfig(const std::string &filename)
    {
        static std::mutex confMutex;
        static std::unordered_map<std::string, std::unique_ptr<Loggator> > mapLoggator;

        Config conf(filename);
        Config::InsensitiveCompare insensitiveCompare;
        std::lock_guard<std::mutex> lockGuardStatic(confMutex);
        for(const std::pair<std::string, Config::MapSection> &sectionItem : conf.getConfig())
        {
            if (insensitiveCompare(sectionItem.first.substr(0, 8), "LOGGATOR:") == false)
                continue;
            if (mapLoggator.find(sectionItem.first) != mapLoggator.end())
                continue;
            std::string sectionName = sectionItem.first.substr(9);
            std::unique_ptr<Loggator> uniquePtr(new Loggator(sectionName));
            Config::setLoggatorCommon(*(uniquePtr.get()), sectionItem.second);
            mapLoggator.emplace(sectionItem.first, std::move(uniquePtr));
        }
        for (std::pair<const std::string, std::unique_ptr<Loggator> > &mapLoggatorItem : mapLoggator)
        {
            Config::MapConfig::const_iterator iteratorSection = conf.getConfig().find(mapLoggatorItem.first);
            if (iteratorSection == conf.getConfig().end())
                continue;
            Config::setLoggatorChilds(*(mapLoggatorItem.second.get()), iteratorSection->second);
        }

        return conf.isRead();
    }

    /*************************************************************************/

    /**
     * @brief Set the Out Stream object
     * 
     * @param os 
     * @return Loggator& 
     */
    Loggator &setOutStream(std::ostream &os)
    {
        close();
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _outStream = &os;
        return *this;
    }

    /**
     * @brief Set the Name object
     * 
     * @param name 
     * @return Loggator& 
     */
    Loggator &setName(const std::string &name)
    {
        std::lock_guard<std::mutex> lockGuardStatic(sMapMutex());
        std::lock_guard<std::mutex> lockGuard(_mutex);
        if (_name == name)
            return *this;
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
        return *this;
    }

    /**
     * @brief Set the Filter object
     * 
     * @param filter 
     * @return Loggator& 
     */
    Loggator &setFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter = filter;
        return *this;
    }

    /**
     * @brief Add a Filter object
     * 
     * @param filter 
     * @return Loggator& 
     */
    Loggator &addFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter |= filter;
        return *this;
    }

    /**
     * @brief Sub a Filter object
     * 
     * @param filter 
     * @return Loggator& 
     */
    Loggator &subFilter(int filter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _filter &= ~filter;
        return *this;
    }


    /**
     * @brief Set the Flush Filter object
     * 
     * @param flushFilter 
     * @return Loggator& 
     */
    Loggator &setFlushFilter(int flushFilter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _flushFilter = flushFilter;
        return *this;
    }

    /**
     * @brief Add a Flush Filter object
     * 
     * @param flushFilter 
     * @return Loggator& 
     */
    Loggator &addFlushFilter(int flushFilter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _flushFilter |= flushFilter;
        return *this;
    }

    /**
     * @brief Sub a Flush Filter object
     * 
     * @param flushFilter 
     * @return Loggator& 
     */
    Loggator &subFlushFilter(int flushFilter)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _flushFilter &= ~flushFilter;
        return *this;
    }

    /**
     * @brief Add child in list of object
     *        Add parent in list of @param
     * 
     * @param loggator 
     * @return Loggator& : instance of current object
     */
    Loggator &addChild(Loggator &loggator)
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
    Loggator &subChild(Loggator &loggator)
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
    Loggator &listen(Loggator &loggator)
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
    Loggator &unlisten(Loggator &loggator)
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
    Loggator &setKey(const std::string &key, const std::string &value, bool overrideDefaultKey = false)
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
     * @return Loggator& 
     */
    Loggator &setMute(bool mute)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _mute = mute;
        return *this;
    }

    /**
     * @brief Set the Format New Line object
     * 
     * @param formatNewLine 
     * @return Loggator& 
     */
    Loggator &setFormatNewLine(bool formatNewLine)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _formatNewLine = formatNewLine;
        return *this;
    }

    /**
     * @brief Set the Format object
     * 
     * @param format 
     * @return Loggator& 
     */
    Loggator &setFormat(const std::string &format)
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
                    _mapCustomFormatKey[key] = LOGGATOR_DEFAULT_TIME_FORMAT;
                    _indexTimeNano = _mapCustomFormatKey.at(key).find("%N");
                    if (_indexTimeNano != std::string::npos)
                        _mapCustomFormatKey.at(key).erase(_indexTimeNano, 2);
                }
                else
                {
                    _mapCustomFormatKey[key] = "%s";
                }
                _mapIndexFormatKey.push_back(std::move(std::pair<std::string, std::size_t>(typeFormatConvert(key), indexStart)));
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
            _mapIndexFormatKey.push_back(std::move(std::pair<std::string, std::size_t>(typeFormatConvert(key), indexStart)));
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
        return *this;
    }

    const std::string &getName(void) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        return _name;
    }

    /**
     * @brief Check if object is mute
     * 
     * @return true  : is mute
     * @return false : is not mute
     */
    bool isMute(void) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
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
    bool open(const std::string &path, std::ofstream::openmode openMode = std::ofstream::app)
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
    void close(void)
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
    bool isOpen(void) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        return _fileStream.is_open();
    }

    /**
     * @brief flush this loggator and child
     * 
     */
    void flush(void) const
    {
        _mutex.lock();
        if (_outStream != nullptr && _mute == false)
        {
            _outStream->flush();
        }
        if (_logChilds.empty())
        {
            _mutex.unlock();
            return;
        }
        _mutex.unlock();
        std::set<const Loggator*> setLog = {this};
        flushChild(setLog, *this);
    }

    /*************************************************************************/

    /**
     * @brief Function send without argument
     * 
     * @return Stream : temporary instance of Stream
     */
    Stream send(void) const
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
    Stream send(const eTypeLog &type, const char *format, ...) const __attribute__((__format__(__printf__, 3, 4)))
    {
        char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];
        Stream      stream(*this, type);
        va_list     vargs;
        va_start(vargs, format);
        int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);
        if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)
            buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;
        stream.write(buffer, buffSize);
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
    Stream send(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) const
    {
        return Stream(*this, type, sourceInfos);
    }

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
    Stream send(const eTypeLog &type, const SourceInfos &sourceInfos, const char *format, ...) const __attribute__((__format__(__printf__, 4, 5)))
    {
        char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];
        Stream      stream(*this, type, sourceInfos);
        va_list     vargs;
        va_start(vargs, format);
        int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);
        if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)
            buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;
        stream.write(buffer, buffSize);
        va_end(vargs);
        return stream;
    }

    #define LOGGATOR_FUNCTION_TYPE(_type, _func)                                                                                \
    Stream _func(void) const                                                                                                    \
    {                                                                                                                           \
        return Stream(*this, eTypeLog::_type);                                                                                  \
    }                                                                                                                           \
    Stream _func(const char *format, ...) const __attribute__((__format__(__printf__, 2, 3)))                                   \
    {                                                                                                                           \
        char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];                                                                        \
        Stream      stream(*this, eTypeLog::_type);                                                                             \
        va_list     vargs;                                                                                                      \
        va_start(vargs, format);                                                                                                \
        int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);                                      \
        if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)                                                         \
            buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;                                                                         \
        stream.write(buffer, buffSize);                                                                                         \
        va_end(vargs);                                                                                                          \
        return stream;                                                                                                          \
    }                                                                                                                           \
    Stream _func(const SourceInfos &sourceInfos, const char *format, ...) const __attribute__((__format__(__printf__, 3, 4)))   \
    {                                                                                                                           \
        char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];                                                                        \
        Stream      stream(*this, eTypeLog::_type, sourceInfos);                                                                \
        va_list     vargs;                                                                                                      \
        va_start(vargs, format);                                                                                                \
        int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);                                      \
        if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)                                                         \
            buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;                                                                         \
        stream.write(buffer, buffSize);                                                                                         \
        va_end(vargs);                                                                                                          \
        return stream;                                                                                                          \
    }                                                                                                                           \
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

    LOGGATOR_FUNCTION_TYPE(DEBUG,   debug);
    LOGGATOR_FUNCTION_TYPE(INFO,    info);
    LOGGATOR_FUNCTION_TYPE(WARN,    warn);
    LOGGATOR_FUNCTION_TYPE(WARN,    warning);
    LOGGATOR_FUNCTION_TYPE(ERROR,   error);
    LOGGATOR_FUNCTION_TYPE(CRIT,    crit);
    LOGGATOR_FUNCTION_TYPE(CRIT,    critical);
    LOGGATOR_FUNCTION_TYPE(ALERT,   alert);
    LOGGATOR_FUNCTION_TYPE(EMERG,   emerg);
    LOGGATOR_FUNCTION_TYPE(EMERG,   emergency);
    LOGGATOR_FUNCTION_TYPE(FATAL,   fatal);

    #undef LOGGATOR_FUNCTION_TYPE

    /*************************************************************************/

    /**
     * @brief override operator << to object
     * 
     * @param type 
     * @return Stream : temporary instance of Stream
     */
    Stream operator<<(const eTypeLog &type) const
    {
        return Stream(*this, type);
    }

    /**
     * @brief overide operator << to object
     * 
     * @param manip function pointer (std::endl, std::flush, ...)
     * @return Stream : temporary instance of Stream
     */
    Stream operator<<(std::ostream&(*manip)(std::ostream&)) const
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
    Stream operator()(void) const
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
    Stream operator()(const char * format, ...) const __attribute__((__format__(__printf__, 2, 3)))
    {
        char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];
        Stream      stream(*this);
        va_list     vargs;
        va_start(vargs, format);
        int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);
        if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)
            buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;
        stream.write(buffer, buffSize);
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
    Stream operator()(const T &var) const
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
    Stream operator()(const eTypeLog &type, const SourceInfos &sourceInfos = {nullptr, 0, nullptr}) const
    {
        return Stream(*this, type, sourceInfos);
    }

    /**
     * @brief override operator () to object
     * 
     * @param type 
     * @param format 
     * @param ... 
     * @return Stream : temporary instance of Stream
     */
    Stream operator()(const eTypeLog &type, const char * format, ...) const __attribute__((__format__(__printf__, 3, 4)))
    {
        char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];
        Stream      stream(*this, type);
        va_list     vargs;
        va_start(vargs, format);
        int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);
        if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)
            buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;
        stream.write(buffer, buffSize);
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
    Stream operator()(const eTypeLog &type, const SourceInfos &sourceInfos, const char * format, ...) const __attribute__((__format__(__printf__, 4, 5)))
    {
        char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];
        Stream      stream(*this, type, sourceInfos);
        va_list     vargs;
        va_start(vargs, format);
        int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);
        if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)
            buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;
        stream.write(buffer, buffSize);
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
     * @brief 
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
    void sendToOutStream(const std::string &str, const eTypeLog &type, const SourceInfos &source, bool flush) const
    {
        TimeInfo timeInfo;
        timeInfo.msec[0] = '\0';
        std::string stringThreadID;

        _mutex.lock();
        if (_outStream != nullptr && _mute == false && _filter & static_cast<int>(type))
        {
            std::string tmpPrompt = prompt(this->_name, type, timeInfo, source, stringThreadID, _mapCustomValueKey);
            if (_formatNewLine)
            {
                std::size_t indexSub = 0;
                std::size_t indexNewLine = str.find('\n');
                while (indexNewLine != std::string::npos)
                {
                    *(_outStream) << tmpPrompt << str.substr(indexSub, ++indexNewLine - indexSub);
                    indexSub = indexNewLine;
                    indexNewLine = str.find('\n', indexSub);
                }
            }
            else
            {
                *(_outStream) << tmpPrompt << str;
            }
            if (flush || _flushFilter & static_cast<int>(type))
            {
                _outStream->flush();
            }
        }
        if (_logChilds.empty())
        {
            _mutex.unlock();
            return;
        }
        _mutex.unlock();
        std::set<const Loggator*> tmpsetLog = {this};
        sendChild(tmpsetLog, *this, this->_name, type, timeInfo, source, stringThreadID, str, flush);
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
    void sendChild(std::set<const Loggator*> &setLog, const Loggator &loggator, const std::string &name, const eTypeLog &type, TimeInfo &timeInfo, const SourceInfos &source, std::string &stringThreadID, const std::string &str, bool flush) const
    {
        // create a cpy of LogChild for no dead lock
        loggator._mutex.lock();
        std::set<Loggator*> cpyLogChilds = loggator._logChilds;
        loggator._mutex.unlock();
        for (const Loggator *child : cpyLogChilds)
        {
            if (setLog.insert(child).second == false)
                continue;
            child->_mutex.lock();
            if (child->_outStream != nullptr && child->_mute == false && child->_filter & static_cast<int>(type))
            {
                _mutex.lock();
                std::string tmpPrompt = child->prompt(name, type, timeInfo, source, stringThreadID, _mapCustomValueKey);
                _mutex.unlock();
                if (child->_formatNewLine)
                {
                    std::size_t indexSub = 0;
                    std::size_t indexNewLine = str.find('\n');
                    while (indexNewLine != std::string::npos)
                    {
                        *(child->_outStream) << tmpPrompt << str.substr(indexSub, ++indexNewLine - indexSub);
                        indexSub = indexNewLine;
                        indexNewLine = str.find('\n', indexSub);
                    }
                }
                else
                {
                    *(child->_outStream) << tmpPrompt << str;
                }
                if (flush || child->_flushFilter & static_cast<int>(type))
                {
                    child->_outStream->flush();
                }
            }
            if (child->_logChilds.empty())
            {
                child->_mutex.unlock();
                continue;
            }
            child->_mutex.unlock();
            sendChild(setLog, *child, name, type, timeInfo, source, stringThreadID, str, flush);
        }
    }

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
    void flushChild(std::set<const Loggator*> &setLog, const Loggator &loggator) const
    {
        // create a cpy of LogChild for no dead lock
        loggator._mutex.lock();
        std::set<Loggator*> cpyLogChilds = loggator._logChilds;
        loggator._mutex.unlock();
        for (const Loggator *child : cpyLogChilds)
        {
            if (setLog.insert(child).second == false)
                continue;
            child->_mutex.lock();
            if (child->_outStream != nullptr && child->_mute == false)
            {
                child->_outStream->flush();
            }
            if (child->_logChilds.empty())
            {
                child->_mutex.unlock();
                continue;
            }
            child->_mutex.unlock();
            flushChild(setLog, *child);
        }
    }

    /**
     * @brief get string format time from TimeInfo
     * 
     * @param timeInfo 
     * @return std::string 
     */
    std::string formatTime(TimeInfo &timeInfo) const
    {
        char bufferFormatTime[LOGGATOR_FORMAT_BUFFER_SIZE];
        std::string retStr = _mapCustomFormatKey.at("TIME");
        if (_indexTimeNano != std::string::npos)
            retStr.insert(_indexTimeNano, timeInfo.msec, 6);
        return std::string(bufferFormatTime, 0, std::strftime(bufferFormatTime, LOGGATOR_FORMAT_BUFFER_SIZE - 1, retStr.c_str(), &timeInfo.tm));
    }

    /**
     * @brief Get the Current TimeInfo object
     * 
     * @return TimeInfo 
     */
    void getCurrentTimeInfo(TimeInfo &timeInfo) const
    {
        if (timeInfo.msec[0] != '\0')
            return ;
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::chrono::nanoseconds duration = now.time_since_epoch();
        std::time_t timer = std::chrono::system_clock::to_time_t(now);
        #ifdef __GNUC__
            localtime_r(&timer, &timeInfo.tm);
        #else
            localtime_s(&timeInfo.tm, &timer);
        #endif
        std::snprintf(timeInfo.msec, 7, "%06lu", std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000);
        timeInfo.msec[6] = '\0';
    }

    /**
     * @brief Get the Thread Id object
     * 
     * @return threadId 
     */
    void getThreadId(std::string &threadId) const
    {
        if (threadId.empty() == false)
            return ;
        std::stringstream streamThreadID;
        streamThreadID << std::hex << std::uppercase << std::this_thread::get_id();
        threadId = streamThreadID.str();
    }

    /**
     * @brief convert eTypeLog to const char *
     * 
     * @param type 
     * @return const char* 
     */
    const char *typeToStr(const eTypeLog &type) const
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

    std::string typeFormatConvert(const std::string &type) const
    {
        std::string ret = "0";
        if (type == "TIME")
            ret[0] = -1;
        else if (type == "TYPE")
            ret[0] = -2;
        else if (type == "NAME")
            ret[0] = -3;
        else if (type == "FUNC")
            ret[0] = -4;
        else if (type == "PATH")
            ret[0] = -5;
        else if (type == "FILE")
            ret[0] = -6;
        else if (type == "LINE")
            ret[0] = -7;
        else if (type == "THREAD_ID")
            ret[0] = -8;
        else
            return type;
        return ret;
    }

    /**
     * @brief get custom key with format
     * 
     * @param mapCustomValueKey 
     * @param threadId 
     * @param key 
     * @return std::string : format custom key
     */
    std::string formatCustomKey(const std::unordered_map<std::string, std::string> &mapCustomValueKey, const std::string &threadId, const std::string &key) const
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
        char buffer[LOGGATOR_FORMAT_KEY_BUFFER_SIZE];
        return std::string(buffer, 0, std::snprintf(buffer, LOGGATOR_FORMAT_KEY_BUFFER_SIZE, itFormatMap->second.c_str(), itValueMap->second.c_str()));
    }

    /**
     * @brief get format of common key
     * 
     * @param key 
     * @param value 
     * @return std::string : format common key
     */
    std::string formatKey(const std::string &key, const std::string &value) const
    {
        if (value.empty())
            return "";
        char buffer[LOGGATOR_FORMAT_KEY_BUFFER_SIZE];
        return std::string(buffer, 0, std::snprintf(buffer, LOGGATOR_FORMAT_KEY_BUFFER_SIZE, _mapCustomFormatKey.at(key).c_str(), value.c_str()));
    }

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
    std::string prompt(const std::string &name, const eTypeLog &type, TimeInfo &timeInfo, const SourceInfos &source, std::string &stringThreadID, const std::unordered_map<std::string, std::string> &mapCustomValueKey) const
    {
        std::string prompt;
        std::size_t lastIndex = 0;
        for (const std::pair<std::string, std::size_t> &itemFormatKey : _mapIndexFormatKey)
        {
            for (;lastIndex < itemFormatKey.second; ++lastIndex)
                prompt.push_back(_format[lastIndex]);
            if (itemFormatKey.first[0] == -1) // TIME
            {
                getCurrentTimeInfo(timeInfo);
                prompt.append(formatTime(timeInfo));
            }
            else if (itemFormatKey.first[0] == -2) // TYPE
            {
                prompt.append(formatKey("TYPE", typeToStr(type)));
            }
            else if (itemFormatKey.first[0] == -3) // NAME
            {
                if (name.empty())
                    continue;
                prompt.append(formatKey("NAME", name));
            }
            else if (itemFormatKey.first[0] == -4) // FUNC
            {
                if (source.func == nullptr)
                    continue;
                prompt.append(formatKey("FUNC", source.func));
            }
            else if (itemFormatKey.first[0] == -5) // PATH
            {
                if (source.filename == nullptr)
                    continue;
                prompt.append(formatKey("PATH", source.filename));
            }
            else if (itemFormatKey.first[0] == -6) // FILE
            {
                if (source.filename == nullptr)
                    continue;
                const char *rchr = strrchr(source.filename, '/');
                if (rchr == nullptr)
                    rchr = source.filename;
                else
                    ++rchr;
                prompt.append(formatKey("FILE", rchr));
            }
            else if (itemFormatKey.first[0] == -7) // LINE
            {
                if (source.line == 0)
                    continue;
                prompt.append(formatKey("LINE", std::to_string(source.line)));
            }
            else if (itemFormatKey.first[0] == -8) // THREAD_ID
            {
                getThreadId(stringThreadID);
                prompt.append(formatKey("THREAD_ID", stringThreadID));
            }
            else
            {
                getThreadId(stringThreadID);
                prompt.append(formatCustomKey(mapCustomValueKey, stringThreadID, itemFormatKey.first));
            }
        }
        for (;lastIndex < _format.size(); ++lastIndex)
            prompt.push_back(_format[lastIndex]);
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