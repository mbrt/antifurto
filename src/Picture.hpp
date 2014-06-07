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
    // see: http://stackoverflow.com/questions/12139402/initialize-an-opencv-mat-with-an-2d-array
    Picture(Picture&& ) noexcept(false) = default;
    Picture& operator =(Picture&& ) noexcept(false) = default;

    // copy
    Picture(Picture const& p)
    { p.data_.copyTo(data_); }

    Picture& operator =(Picture const& p)
    { p.data_.copyTo(data_); return *this; }

    Picture& operator =(cv::Mat const& data)
    { data.copyTo(data_); return *this; }

    // accessors
    using iterator = cv::MatIterator_<uchar>;
    using const_iterator = cv::MatConstIterator_<uchar>;

    iterator begin()
    { return data_.begin<uchar>(); }

    iterator end()
    { return data_.end<uchar>(); }

    const_iterator begin() const
    { return data_.begin<uchar>(); }

    const_iterator end() const
    { return data_.end<uchar>(); }

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
