//////////////////////////////////////////////////////////////////////
/// Desc  StringRef
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ls-config.hpp"
#include <string>
#include <cstring>

NS_LS_BEGIN

class LS_API StringRef
{
    static const size_t EMPTY_HASH = (size_t)-1;
public:
    StringRef()
    {}

    StringRef(const char *str, size_t length = 0)
        : str_(str)
        , length_(length > 0 ? length : strlen(str))
    {}

    StringRef(const std::string &str)
        : str_(str.c_str())
        , length_(str.length())
    {}

    bool existHash() const { return hash_ != EMPTY_HASH; }

    size_t hash() const
    {
        if(hash_ == EMPTY_HASH)
        {
            hash_ = computeHash();
        }
        return hash_;
    }

    const char* data() const { return str_; }
    const char* c_str() const { return str_; }

    const char* begin() const { return str_; }
    const char* end() const { return str_ + length_; }
    
    std::string std_str() const { return std::string(str_, length()); }

    bool empty() const { return length_ == 0; }

    size_t length() const { return length_;}
    size_t size() const { return length_; }

    char front() const { return length_ > 0 ? str_[0] : 0; }
    char back() const { return length_ > 0 ? str_[length_ - 1] : 0; }

    const StringRef& operator = (const char *str)
    {
        assign(str, strlen(str));
        return *this;
    }

    const StringRef& operator = (const std::string &str)
    {
        assign(str.c_str(), str.length());
        return *this;
    }

    const StringRef& operator = (const StringRef &str)
    {
        str_ = str.str_;
        length_ = str.length_;
        hash_ = str.hash_;
        return *this;
    }

    void assign(const char *str, size_t length)
    {
        str_ = str;
        length_ = length;
        hash_ = EMPTY_HASH;
    }

    char operator[](size_t index) const
    {
        return index < length_ ? str_[index] : 0;
    }

    bool operator == (const StringRef &str) const
    {
        if(length_ != str.length_ || (existHash() && str.existHash() && hash() != str.hash()))
        {
            return false;
        }

        return 0 == strncmp(str_, str.str_, length_);
    }

    bool operator != (const StringRef &str) const
    {
        return !(*this == str);
    }

    bool operator < (const StringRef &str) const
    {
        return hash() < str.hash();
    }
    
    std::string operator + (const StringRef &str) const
    {
        std::string ret(str_, length_);
        ret.append(str.str_, str.length());
        return ret;
    }

    void assignTo(std::string &dst) const
    {
        if(length_ > 0)
        {
            dst.assign(str_, length_);
        }
    }

    void appendTo(std::string &dst) const
    {
        if(length_ > 0)
        {
            dst.insert(dst.size(), str_, length_);
        }
    }

    size_t find(char ch) const { return find_first_of(ch); }

    size_t find(const StringRef& str) const;
    
    size_t find_first_of(char ch, size_t pos = 0) const
    {
        for(size_t i = pos; i < length_; ++i)
        {
            if(ch == str_[i])
            {
                return i;
            }
        }
        return npos;
    }

    size_t find_last_of(char ch, size_t pos = npos) const
    {
        size_t end = pos < length_ ? pos + 1 : length_;
        for(size_t i = 0; i < end; ++i)
        {
            size_t idx = end - i - 1;
            if(ch == str_[idx])
            {
                return idx;
            }
        }
        return npos;
    }

    StringRef substr(size_t pos, size_t len = npos) const
    {
        if(pos > length_)
        {
            return StringRef();
        }
        len = len < length_ ? len : length_;
        len = len + pos <= length_ ? len : length_ - pos;
        return StringRef(str_ + pos, len);
    }

    static const size_t   npos;


    /** 从start开始(包括start)向后搜索。 如果未找到，则返回-1。*/
    int findFirst(char ch, int start = 0) const
    {
        int i = start;
        for( ; i < (int)length_; ++i)
        {
            if(str_[i] == ch)
            {
                return i;
            }
        }
        return -1;
    }

    /** 从end开始(不包括end)向前搜索。如果未找到，则返回-1。 */
    int findLast(char ch, int end = -1) const
    {
        int i = end;
        if(i < 0)
        {
            i = (int)length_;
        }
        --i;
        for( ; i >= 0; --i)
        {
            if(str_[i] == ch)
            {
                return i;
            }
        }
        return -1;
    }

    /** 切分字符串。切分区间[start, end)，包含start，不含end。
     *  如果start，或end超出长度范围，会被限制到长度范围内。
     *  如果start >= end，返回空字符串。
     */
    StringRef sliceString(int start, int end) const
    {
        if(start < 0)
        {
            start = 0;
        }
        if(end > (int)length_)
        {
            end = (int)length_;
        }
        if(start >= end)
        {
            return "";
        }

        return StringRef(str_ + start, end - start);
    }


private:
    size_t computeHash() const;

    const char*     str_ = "";
    size_t          length_ = 0;
    mutable size_t  hash_ = EMPTY_HASH;
};

NS_LS_END

namespace std
{
    template<> struct hash<NS_LS StringRef>
    {
        typedef NS_LS StringRef argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const
        {
            return s.hash();
        }
    };
}
