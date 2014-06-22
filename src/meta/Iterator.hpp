#pragma once

namespace antifurto {
namespace meta {

template <typename Iterator>
class IteratorPair {
public:
    IteratorPair (Iterator first, Iterator last) : f_ (first), l_ (last) {}
    Iterator begin() const { return f_; }
    Iterator end() const { return l_; }

private:
    Iterator f_;
    Iterator l_;
};

template <typename Iterator>
IteratorPair<Iterator> makeIteratorPair(Iterator begin, Iterator end)
{
    return { begin, end };
}

} // namespace meta
} // namespace antifurto
