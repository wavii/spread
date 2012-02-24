#ifndef __SPREAD_HASHERS_H__
#define __SPREAD_HASHERS_H__

#include <string>
#include <boost/cstdint.hpp>

namespace spread {

// hashes to the first space
struct ToSpace
{
   boost::uint32_t operator()(const std::string& data) const;
};

// hashes to the first separator
struct ToSeparator
{
   ToSeparator(char sep);

   boost::uint32_t operator()(const std::string& data) const;

   char sep_;
};

} // spread

#endif // __SPREAD_HASHERS_H__
