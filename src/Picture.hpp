#pragma once

#include <opencv2/core/core.hpp>

namespace antifurto {

/// This is the Picture value
class Picture
{
public:
    // default
    Picture() = default;

    // construct with data
    explicit Picture(cv::Mat data)
        : data_(std::move(data)) { }

    // move
    Picture(Picture&& ) = default;
    Picture& operator =(Picture&& ) = default;

    // copy
    Picture(Picture const& p)
        : data_(p.data_.clone()) { }

    Picture& operator =(Picture const& p)
    { data_ = p.data_.clone(); return *this; }

    Picture& operator =(cv::Mat const& data)
    { data_ = data; return *this; }

    // conversion
    operator cv::Mat()
    { return data_; }

    operator cv::Mat() const
    { return data_; }

private:
    cv::Mat data_;
};

} // namespace antifurto
