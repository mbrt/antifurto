#pragma once

#include "meta/SmartPtr.hpp"

#include <string>
#include <vector>
#include <chrono>

namespace antifurto {

/// This class represents the configuration for antifurto
class Configuration
{
public:
    Configuration();

    using StringList = std::vector<std::string>;
    struct Whatsapp {
        std::string cc;
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
    struct Log {
        enum class Level {
            DEBUG, INFO
        };
        std::string dir;
        Level level;
    };
    struct Recording {
        unsigned int maxDays;
        std::string archiveDir;
    };

    std::chrono::seconds startupTimeout;
    Whatsapp whatsapp;
    Dropbox dropbox;
    Log log;
    Recording recording;
};


// fwd reference
class ConfigurationParserImpl;

/// This class fills a Configuration by parsing the command line and a
/// configuration file.
///
/// Note: call parseCmdLine and/or parseConfigFile in order of priority; in case
/// of conflicting options, the first is considered.
class ConfigurationParser
{
public:
    ConfigurationParser();

    void parseCmdLine(int argc, char* argv[]);
    void parseConfigFile(const char* configFile);
    Configuration getConfiguration();
    bool canContinue() const;

private:
    meta::ErasedUniquePtr<ConfigurationParserImpl> impl_;
};

} // namespace antifurto
