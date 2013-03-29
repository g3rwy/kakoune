#ifndef string_hh_INCLUDED
#define string_hh_INCLUDED

#include <string>
#include <boost/regex.hpp>

#include "memoryview.hh"
#include "units.hh"
#include "utf8.hh"

namespace Kakoune
{

typedef boost::regex Regex;

class String : public std::string
{
public:
   String() {}
   String(const char* content) : std::string(content) {}
   String(std::string content) : std::string(std::move(content)) {}
   explicit String(char content, CharCount count = 1) : std::string((size_t)(int)count, content) {}
   explicit String(Codepoint cp, CharCount count = 1) { utf8::dump(back_inserter(*this), cp); }
   template<typename Iterator>
   String(Iterator begin, Iterator end) : std::string(begin, end) {}

   std::string& stdstr() { return *this; }
   const std::string& stdstr() const { return *this; }

   char      operator[](ByteCount pos) const { return std::string::operator[]((int)pos); }
   char&     operator[](ByteCount pos) { return std::string::operator[]((int)pos); }
   ByteCount length() const { return ByteCount{(int)std::string::length()}; }
   CharCount char_length() const { return utf8::distance(begin(), end()); }
   ByteCount byte_count_to(CharCount count) const { return utf8::advance(begin(), end(), (int)count) - begin(); }
   CharCount char_count_to(ByteCount count) const { return utf8::distance(begin(), begin() + (int)count); }

   String  operator+(const String& other) const { return String{stdstr() + other.stdstr()}; }
   String& operator+=(const String& other) { std::string::operator+=(other); return *this; }
   String  operator+(const char* other) const { return String{stdstr() + other}; }
   String& operator+=(const char* other) { std::string::operator+=(other); return *this; }
   String  operator+(char other) const { return String{stdstr() + other}; }
   String& operator+=(char other) { std::string::operator+=(other); return *this; }
   String  operator+(Codepoint cp) const { String res = *this; utf8::dump(back_inserter(res), cp); return res; }
   String& operator+=(Codepoint cp) { utf8::dump(back_inserter(*this), cp); return *this; }

   memoryview<char> data()  const { return memoryview<char>(std::string::data(), size()); }

   String substr(ByteCount pos, ByteCount length = -1) const { return String{std::string::substr((int)pos, (int)length)}; }
   String substr(CharCount pos, CharCount length = INT_MAX) const
   {
       auto b = utf8::advance(begin(), end(), (int)pos);
       auto e = utf8::advance(b, end(), (int)length);
       return String(b,e);
   }
   String replace(const String& expression, const String& replacement) const;
};

inline String operator+(const char* lhs, const String& rhs)
{
    return String(lhs) + rhs;
}

inline String operator+(char lhs, const String& rhs)
{
    return String(lhs) + rhs;
}

inline String operator+(Codepoint lhs, const String& rhs)
{
    return String(lhs) + rhs;
}


String int_to_str(int value);
int    str_to_int(const String& str);
std::vector<String> split(const String& str, char separator);

inline String operator"" _str(const char* str, size_t)
{
    return String(str);
}

inline String codepoint_to_str(Codepoint cp)
{
    std::string str;
    utf8::dump(back_inserter(str), cp);
    return String(str);
}

}

namespace std
{
    template<>
    struct hash<Kakoune::String> : hash<std::string>
    {
        size_t operator()(const Kakoune::String& str) const
        {
            return hash<std::string>::operator()(str);
        }
    };
}

#endif // string_hh_INCLUDED

