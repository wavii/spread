#include <spread/hashers.h>
#include <cctype>
#include <spread/murmur_hash3.hpp>

using namespace spread;
using namespace boost;

uint32_t ToSpace::operator()(const std::string & data) const
{
   const char * c = data.c_str();
   int len;
   for (len = 0; c[len] != 0 && !std::isspace(c[len]); ++len);
   uint32_t out;
   MurmurHash3_x86_32(c, len, 0, &out);
   return out;
}

ToSeparator::ToSeparator(char sep)
: sep_(sep) {}

uint32_t ToSeparator::operator()(const std::string & data) const
{
   const char * c = data.c_str();
   int len;
   for (len = 0; c[len] != 0 && c[len] != sep_; ++len);
   uint32_t out;
   MurmurHash3_x86_32(c, len, 0, &out);
   return out;
}
