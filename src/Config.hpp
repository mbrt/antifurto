#pragma once

#if defined(ANTIFURTO_RASPBERRY)
#   include "PiCamera.hpp"
#else
#   include "CvCamera.hpp"
#endif

#include "meta/SmartPtr.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace antifurto {
namespace config {

#if defined(ANTIFURTO_RASPBERRY)
    using Camera = antifurto::PiCamera;
#else
    using Camera = antifurto::CvCamera;
#endif

///@{ Static configurations
///
using Milliseconds = std::chrono::milliseconds;
constexpr Milliseconds monitorCycleDuration() { return Milliseconds{ 500 }; }
constexpr Milliseconds monitorCycleDurationOnAlarm() { return Milliseconds{ 200 }; }

constexpr const char* configDir() { return "config"; }
constexpr const char* archiveDir() { return "/tmp"; }
constexpr const char* configFile() { return "config/antifurto.cfg"; }
constexpr const char* dropboxConfigFile() { return "config/dropbox.cfg"; }
constexpr const char* whatsappConfigFile() { return "config/whatsapp.cfg"; }

constexpr unsigned int maxArchiveDays() { return 7; }
///@}

} // namespace config


/// This class represents the configuration of antifurto
class Configuration
{
public:
    using StringList = std::vector<std::string>;
    struct Whatsapp {
        std::string src;
        std::string pwd;
        StringList destinations;
    };
    struct Dropbox {
        std::string appKey;
        std::string appSecret;
        std::string oauthToken;
        std::string oauthTokenSecret;
    };

    Whatsapp whatsapp;
    Dropbox dropbox;
};


// fwd reference
class ConfigurationParserImpl;

/// This class fills a Configuration by parsing the command line and a
/// configuration file
class ConfigurationParser
{
public:
    ConfigurationParser();

    void parseCmdLine(int argc, char* argv[]);
    void parseConfigFile(const char* configFile);
    Configuration getConfiguration();

private:
    meta::ErasedUniquePtr<ConfigurationParserImpl> impl_;
};

} // namespace antifurto
