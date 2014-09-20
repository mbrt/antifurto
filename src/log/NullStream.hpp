#pragma once

#include <iostream>

namespace antifurto {
namespace log {

// see http://stackoverflow.com/questions/19200207/redirect-debug-output-to-null-stream-instead-of-stdcerr
template <class cT, class traits = std::char_traits<cT> >
class basic_nullbuf: public std::basic_streambuf<cT, traits> {
    typename traits::int_type overflow(typename traits::int_type c)
    {
        return traits::not_eof(c); // indicate success
    }
};

template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream: public std::basic_ostream<cT, traits> {
public:
    basic_onullstream():
    std::basic_ios<cT, traits>(&m_sbuf),
    std::basic_ostream<cT, traits>(&m_sbuf)
    {
        this->init(&m_sbuf);
    }

private:
    basic_nullbuf<cT, traits> m_sbuf;
};

/// A null stream is a stream that ignores inputs
typedef basic_onullstream<char> onullstream;
typedef basic_onullstream<wchar_t> wonullstream;

}} // namespace antifurto::log