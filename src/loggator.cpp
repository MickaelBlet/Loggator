/**
 * @file loggator.cpp
 * @author Mickaël BLET
 */

#include "loggator.hpp"

namespace Log
{

//
// PUBLIC
//

/**
 * @brief Construct a new Loggator object
 * 
 */
Loggator::Loggator(void):
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
Loggator::Loggator(int filter):
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
Loggator::Loggator(std::ostream &oStream, int filter):
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
Loggator::Loggator(const std::string &name, const std::string &path, std::ofstream::openmode openMode, int filter):
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
Loggator::Loggator(const std::string &name, int filter):
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
Loggator::Loggator(const std::string &name, std::ostream &oStream, int filter):
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
Loggator::Loggator(const std::string &name, Loggator &loggator):
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
Loggator::Loggator(Loggator &loggator):
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
Loggator::Loggator(const std::string &name, const Loggator &loggator):
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
Loggator::Loggator(const Loggator &loggator):
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
 * @brief override operator = to object
 * 
 * @param rhs 
 * @return Loggator& 
 */
Loggator &Loggator::operator=(const Loggator &rhs)
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
Loggator::~Loggator(void)
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
Loggator &Loggator::getInstance(const std::string &name)
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
Loggator &Loggator::getInstance(Loggator &loggator)
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
bool Loggator::openConfig(const std::string &filename)
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
Loggator &Loggator::setOutStream(std::ostream &os)
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
Loggator &Loggator::setName(const std::string &name)
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
Loggator &Loggator::setFilter(int filter)
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
Loggator &Loggator::addFilter(int filter)
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
Loggator &Loggator::subFilter(int filter)
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
Loggator &Loggator::setFlushFilter(int flushFilter)
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
Loggator &Loggator::addFlushFilter(int flushFilter)
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
Loggator &Loggator::subFlushFilter(int flushFilter)
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
Loggator &Loggator::addChild(Loggator &loggator)
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
Loggator &Loggator::subChild(Loggator &loggator)
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
Loggator &Loggator::listen(Loggator &loggator)
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
Loggator &Loggator::unlisten(Loggator &loggator)
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
Loggator &Loggator::setKey(const std::string &key, const std::string &value, bool overrideDefaultKey)
{
    std::lock_guard<std::mutex> lockGuard(_mutex);
    if (overrideDefaultKey || _mapCustomValueKey.find(key) == _mapCustomValueKey.end())
    {
        // add new value key in custom key map
        _mapCustomValueKey[key] = value;
    }
    std::stringstream streamThreadIDKey;
    streamThreadIDKey << std::hex << std::uppercase << std::this_thread::get_id() << key;
    std::string threadIDKey = streamThreadIDKey.str();
    _mapCustomValueKey[threadIDKey] = value;
    return *this;
}

/**
 * @brief Set the Mute object
 * 
 * @param mute 
 * @return Loggator& 
 */
Loggator &Loggator::setMute(bool mute)
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
Loggator &Loggator::setFormatNewLine(bool formatNewLine)
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
Loggator &Loggator::setFormat(const std::string &format)
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
            _mapIndexFormatKey.emplace_back(typeFormatConvert(key), indexStart);
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
        _mapIndexFormatKey.emplace_back(typeFormatConvert(key), indexStart);
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

/**
 * @brief Get the Name object
 * 
 * @return const std::string& 
 */
const std::string &Loggator::getName(void) const
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
bool Loggator::isMute(void) const
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
bool Loggator::open(const std::string &path, std::ofstream::openmode openMode)
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
void Loggator::close(void)
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
bool Loggator::isOpen(void) const
{
    std::lock_guard<std::mutex> lockGuard(_mutex);
    return _fileStream.is_open();
}

/**
 * @brief flush this loggator and child
 * 
 */
void Loggator::flush(void) const
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
Loggator::Stream Loggator::send(void) const
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
Loggator::Stream Loggator::send(const eTypeLog &type, const char *format, ...) const
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
Loggator::Stream Loggator::send(const eTypeLog &type, const SourceInfos &sourceInfos) const
{
    return Stream(*this, type, sourceInfos);
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
Loggator::Stream Loggator::send(const eTypeLog &type, const SourceInfos &sourceInfos, const char *format, ...) const
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

#define LOGGATOR_FUNCTION_TYPE(_type, _func)                                                    \
Loggator::Stream Loggator::_func(void) const                                                    \
{                                                                                               \
    return Stream(*this, eTypeLog::_type);                                                      \
}                                                                                               \
Loggator::Stream Loggator::_func(const char *format, ...) const                                 \
{                                                                                               \
    char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];                                            \
    Stream      stream(*this, eTypeLog::_type);                                                 \
    va_list     vargs;                                                                          \
    va_start(vargs, format);                                                                    \
    int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);          \
    if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)                             \
        buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;                                             \
    stream.write(buffer, buffSize);                                                             \
    va_end(vargs);                                                                              \
    return stream;                                                                              \
}                                                                                               \
Loggator::Stream Loggator::_func(const SourceInfos &sourceInfos, const char *format, ...) const \
{                                                                                               \
    char        buffer[LOGGATOR_FORMAT_BUFFER_SIZE];                                            \
    Stream      stream(*this, eTypeLog::_type, sourceInfos);                                    \
    va_list     vargs;                                                                          \
    va_start(vargs, format);                                                                    \
    int buffSize = std::vsnprintf(buffer, LOGGATOR_FORMAT_BUFFER_SIZE, format, vargs);          \
    if (buffSize > LOGGATOR_FORMAT_BUFFER_SIZE - 1 || buffSize < 0)                             \
        buffSize = LOGGATOR_FORMAT_BUFFER_SIZE - 1;                                             \
    stream.write(buffer, buffSize);                                                             \
    va_end(vargs);                                                                              \
    return stream;                                                                              \
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
Loggator::Stream Loggator::operator<<(const eTypeLog &type) const
{
    return Stream(*this, type);
}

/**
 * @brief overide operator << to object
 * 
 * @param manip function pointer (std::endl, std::flush, ...)
 * @return Stream : temporary instance of Stream
 */
Loggator::Stream Loggator::operator<<(std::ostream&(*manip)(std::ostream&)) const
{
    Stream stream(*this);
    stream << manip;
    return stream;
}

/**
 * @brief override operator () to object
 * 
 * @return Stream : temporary instance of Stream
 */
Loggator::Stream Loggator::operator()(void) const
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
Loggator::Stream Loggator::operator()(const char * format, ...) const
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
 * @param type 
 * @param sourceInfos 
 * @return Stream : temporary instance of Stream
 */
Loggator::Stream Loggator::operator()(const eTypeLog &type, const SourceInfos &sourceInfos) const
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
Loggator::Stream Loggator::operator()(const eTypeLog &type, const char * format, ...) const
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
Loggator::Stream Loggator::operator()(const eTypeLog &type, const SourceInfos &sourceInfos, const char * format, ...) const
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

//
// PROTECTED
//

/**
 * @brief write in _outStream and _outStream_child
 * 
 * @param str 
 * @param type 
 * @param source 
 */
void Loggator::sendToOutStream(const std::string &str, const eTypeLog &type, const SourceInfos &source, bool flush) const
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
void Loggator::sendChild(std::set<const Loggator*> &setLog, const Loggator &loggator, const std::string &name, const eTypeLog &type, TimeInfo &timeInfo, const SourceInfos &source, std::string &stringThreadID, const std::string &str, bool flush) const
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
 */
void Loggator::flushChild(std::set<const Loggator*> &setLog, const Loggator &loggator) const
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
std::string Loggator::formatTime(TimeInfo &timeInfo) const
{
    char bufferFormatTime[LOGGATOR_FORMAT_BUFFER_SIZE];
    std::string retStr = _mapCustomFormatKey.at("TIME");
    if (_indexTimeNano != std::string::npos)
        retStr.insert(_indexTimeNano, timeInfo.msec, 6);
    std::strftime(bufferFormatTime, LOGGATOR_FORMAT_BUFFER_SIZE, retStr.c_str(), &timeInfo.tm);
    return bufferFormatTime;
}

/**
 * @brief Get the Current TimeInfo object
 * 
 * @return TimeInfo 
 */
void Loggator::getCurrentTimeInfo(TimeInfo &timeInfo) const
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
    std::snprintf(timeInfo.msec, 8, "%06d", static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000));
    timeInfo.msec[6] = '\0';
}

/**
 * @brief Get the Thread Id object
 * 
 * @return threadId 
 */
void Loggator::getThreadId(std::string &threadId) const
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
const char *Loggator::typeToStr(const eTypeLog &type) const
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
 * @brief convert format type
 * 
 * @param type 
 * @return std::string 
 */
std::string Loggator::typeFormatConvert(const std::string &type) const
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
std::string Loggator::formatCustomKey(const std::unordered_map<std::string, std::string> &mapCustomValueKey, const std::string &threadId, const std::string &key) const
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
    int bufferSize = std::snprintf(buffer, LOGGATOR_FORMAT_KEY_BUFFER_SIZE, itFormatMap->second.c_str(), itValueMap->second.c_str());
    return std::string(buffer, 0, bufferSize);
}

/**
 * @brief get format of common key
 * 
 * @param key 
 * @param value 
 * @return std::string : format common key
 */
std::string Loggator::formatKey(const std::string &key, const std::string &value) const
{
    if (value.empty())
        return "";
    char buffer[LOGGATOR_FORMAT_KEY_BUFFER_SIZE];
    int bufferSize = std::snprintf(buffer, LOGGATOR_FORMAT_KEY_BUFFER_SIZE, _mapCustomFormatKey.at(key).c_str(), value.c_str());
    return std::string(buffer, 0, bufferSize);
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
std::string Loggator::prompt(const std::string &name, const eTypeLog &type, TimeInfo &timeInfo, const SourceInfos &source, std::string &stringThreadID, const std::unordered_map<std::string, std::string> &mapCustomValueKey) const
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
std::mutex &Loggator::sMapMutex(void)
{
    static std::mutex singletonMapMutex;
    return singletonMapMutex;
}

/**
 * @brief singleton static map Loggator
 * 
 * @return std::map<std::string, Loggator*>& 
 */
std::unordered_map<std::string, Loggator* > &Loggator::sMapLoggators(void)
{
    static std::unordered_map<std::string, Loggator* > singletonMapLoggators;
    return singletonMapLoggators;
}

} // end namespace Log