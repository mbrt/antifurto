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
    // not noexcept because of the cv::Mat copy constructor
    // FIXME: this can be fixed if std::vector<uchar> is used instead of cv::Mat
    Picture(Picture&& ) noexcept(false) = default;
    Picture& operator =(Picture&& ) noexcept(false) = default;

    // copy
    Picture(Picture const& p)
    { p.data_.copyTo(data_); }

    Picture& operator =(Picture const& p)
    { p.data_.copyTo(data_); return *this; }

    Picture& operator =(cv::Mat const& data)
    { data.copyTo(data_); return *this; }

    // conversion
    operator cv::Mat&()
    { return data_; }

    operator cv::Mat const&() const
    { return data_; }

    operator cv::_InputArray const() const
    { return data_; }

    operator cv::_OutputArray const()
    { return data_; }

private:
    cv::Mat data_;
};

} // namespace antifurto
