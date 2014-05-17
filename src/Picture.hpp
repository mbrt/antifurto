#pragma once

#include <opencv2/core/core.hpp>

namespace antifurto {

class Picture
{
public:
    Picture() = default;
    Picture(Picture&& ) = default;
    Picture(Picture const& ) = delete;

    Picture(cv::Mat const& data)
        : data(data)
    { }

    Picture& operator =(Picture&& ) = default;
    Picture& operator =(Picture const& ) = delete;

    Picture& operator =(cv::Mat const& data)
    { this->data = data; return *this; }

private:
    cv::Mat data;
};

} // namespace antifurto
