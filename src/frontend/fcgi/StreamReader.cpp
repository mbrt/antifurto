#include "StreamReader.hpp"

#include <iostream>

namespace antifurto {
namespace fcgi {

StreamReader::StreamReader(std::istream& in, std::size_t maxSize)
    : in_(in), maxSize_(maxSize), buffer_(1024), count_(0)
{ }

std::size_t StreamReader::read(std::size_t count)
{
    if (count > maxSize_)
        count = maxSize_;
    if (buffer_.size() < count)
        buffer_.resize(count);
    in_.read(&buffer_[0], count);
    count_ = in_.gcount();
    return count_;
}

StreamReader::Range
StreamReader::getLastReadContents() const
{
    return { &buffer_[0], &buffer_[0] + count_ };
}

void StreamReader::emptyStream()
{
    do
        in_.ignore(1024);
    while (in_.gcount() == 1024);
}

}} // namespace antifurto::fcgi
