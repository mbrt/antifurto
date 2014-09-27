#pragma once

#include <iosfwd>
#include <vector>
#include <utility>

namespace antifurto {
namespace fcgi {

/// This class read from a stream
class StreamReader
{
public:
    using Range = std::pair<const char*, const char*>;
    
    StreamReader(std::istream& in, std::size_t maxSize);
    std::size_t read(std::size_t count);
    Range getLastReadContents() const;
    void emptyStream();

private:
    std::istream& in_;
    std::size_t maxSize_;
    std::vector<char> buffer_;
    std::size_t count_;
};

}} // namespace antifurto::fcgi