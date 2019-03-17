/**
 * @file loggator_config.cpp
 * @author Mickaël BLET
 */

#include "loggator.hpp"

namespace Log
{

//
// PUBLIC
//

bool Loggator::Config::InsensitiveCompare::operator()(const std::string & s1, const std::string & s2) const
{
    return std::lexicographical_compare(
            s1.begin (), s1.end (),   // source range
            s2.begin (), s2.end (),   // dest range
            [](const unsigned char&c1, const unsigned char&c2) // lambda compare
            {
                return std::tolower(c1) < std::tolower(c2);
            });  // comparison
}

/**
 * @brief Construct a new Config object
 * 
 * @param filename 
 */
Loggator::Config::Config(const std::string &filename):
_isRead(false)
{
    readFile(filename);
}

/**
 * @brief Destroy the Config object
 * 
 */
Loggator::Config::~Config(void)
{
    return ;
}

/*********************************************************************/

/**
 * @brief check if config is read
 * 
 * @return true : if config is read
 * @return false : if config is not read
 */
bool Loggator::Config::isRead(void) const
{
    return _isRead;
}

/**
 * @brief Get the Config object
 * 
 * @return const MapConfig& 
 */
const Loggator::Config::MapConfig &Loggator::Config::getConfig(void) const
{
    return _mapConfig;
}

/**
 * @brief Set the Loggator object from map section
 * 
 * @param loggator 
 * @param mapSection 
 */
void Loggator::Config::setLoggatorCommon(Loggator &loggator, const MapSection &mapSection)
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
void Loggator::Config::setLoggatorChilds(Loggator &loggator, const MapSection &mapSection)
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

//
// PRIVATE
//

/**
 * @brief parse filter configuration
 * 
 * @param str 
 * @return int : filter
 */
int Loggator::Config::parseFilter(const std::string &str)
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
std::ios::openmode Loggator::Config::parseOpenMode(const std::string &str)
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
std::ostream &Loggator::Config::parseOutStream(const std::string &str)
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
std::set<Loggator *> Loggator::Config::parseChild(const std::string &str)
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
bool Loggator::Config::parseBool(const std::string &str)
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
std::string Loggator::Config::parseSimpleTrim(const std::string &str)
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
 * @brief read config in filename
 * 
 * @param filename 
 * @return true : config is read
 * @return false : config is not read
 */
bool Loggator::Config::readFile(const std::string& filename)
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
 * @brief check if character is comment
 * 
 * @param c 
 * @return true : c is comment character
 * @return false : c is not comment character
 */
bool Loggator::Config::isComment(const unsigned char &c) const
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
bool Loggator::Config::parseKey(const std::string &line, const std::string &currentSectionName)
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
bool Loggator::Config::parseSection(const std::string &line, std::string *retSection) const
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
bool Loggator::Config::emptyOrComment(const std::string &line) const
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
void Loggator::Config::readStream(std::istream &fileStream)
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

} // end namespace Log