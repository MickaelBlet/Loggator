/**
 * @file loggator_stream.cpp
 * @author Mickaël BLET
 */

#include "loggator.hpp"

namespace Log
{

//
// PUBLIC
//

/**
 * @brief Construct a new Send Stream object
 * 
 * @param loggator 
 * @param type 
 * @param sourceInfos 
 */
Loggator::Stream::Stream(const Loggator &loggator, const eTypeLog &type, const SourceInfos &sourceInfos, bool flush):
_parent(*this),
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
Loggator::Stream::Stream(Stream &&stream):
_parent(*this),
_log(std::move(stream._log)),
_type(std::move(stream._type)),
_sourceInfos(std::move(stream._sourceInfos)),
_flush(std::move(stream._flush))
{
    return ;
}

/**
 * @brief Destroy the Send Stream object
 * 
 */
Loggator::Stream::~Stream(void)
{
    if (_type != eTypeLog::NONE)
    {
        this->put('\n');
        std::string cacheStr = this->str();
        if (cacheStr.size() > 1 && cacheStr[cacheStr.size() - 2] == '\n')
            cacheStr.pop_back();
        _log.sendToOutStream(cacheStr, _type, _sourceInfos, _flush);
    }
    return ;
}

/*********************************************************************/

/**
 * @brief override operator << to object
 * 
 * @param type : new type of instance
 * @return Stream& : instance of current object
 */
Loggator::Stream &Loggator::Stream::operator<<(const eTypeLog &type)
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
Loggator::Stream &Loggator::Stream::operator<<(const SourceInfos &sourceInfos)
{
    _sourceInfos = sourceInfos;
    return *this;
}

/**
 * @brief overide operator << to object
 * 
 * @param manip : function pointer (std::endl, std::flush, ...)
 * @return Stream& : instance of current object
 */
Loggator::Stream &Loggator::Stream::operator<<(std::ostream&(*manip)(std::ostream&))
{
    if (manip == &(std::flush<char, std::char_traits<char>>)
    ||  manip == &(std::endl<char, std::char_traits<char>>))
    {
        _flush = true;
    }
    manip(*this);
    return *this;
}

} // end namespace Log