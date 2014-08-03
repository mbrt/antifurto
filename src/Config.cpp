#include "Config.hpp"
#include "StaticConfig.hpp"

#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace antifurto {

// default configuration
Configuration::Configuration()
{
    startup.liveView = false;
    startup.monitor = true;
    startup.monitorTimeout = std::chrono::seconds(5);
    log.level = Log::Level::INFO;
    log.dir = config::logDir();
    recording.maxDays = config::maxArchiveDays();
    recording.archiveDir = config::archiveDir();
    liveView.numPictures = 3;
    liveView.filePrefix = config::liveViewFilePrefix();
    liveView.inactivityTimeout = std::chrono::seconds(15);
}


// implementation class for ConfigurationParser
class ConfigurationParserImpl
{
public:
    ConfigurationParserImpl()
    {
        initOptions();
    }

    void parseCmdLine(int argc, char* argv[])
    {
        po::store(po::parse_command_line(argc, argv, cmdLineOpts_), vm_);
        po::notify(vm_);
        if (vm_.count("help")) {
            std::cout << cmdLineOpts_ << std::endl;
            canContinue_ = false;
        }
        else if (vm_.count("version")) {
            std::cout << ANTIFURTO_VERSION_STRING << std::endl;
            canContinue_ = false;
        }
    }

    void parseConfigFile(const char* configFile)
    {
        po::store(po::parse_config_file<char>(configFile, configOpts_), vm_);
        po::notify(vm_);
    }

    Configuration getConfiguration()
    {
        Configuration::Startup& strt = config_.startup;
        Configuration::Recording& rec  = config_.recording;
        Configuration::Log& log = config_.log;
        Configuration::Whatsapp& whatsapp = config_.whatsapp;
        Configuration::Dropbox& dropbox = config_.dropbox;
        Configuration::LiveView& liveView = config_.liveView;

        storeOptionOrDefault("startup.live-view", strt.liveView);
        storeOptionOrDefault("startup.monitor", strt.monitor);
        storeOptionOrDefault("startup.monitor-timeout", strt.monitorTimeout);
        storeOptionOrDefault("recording.archive-dir", rec.archiveDir);
        storeOptionOrDefault("recording.max-days", rec.maxDays);
        storeOptionOrDefault("log.level", log.level);
        storeOptionOrDefault("log.dir", log.dir);
        storeOptionOrDefault("whatsapp.cc", whatsapp.cc);
        storeOptionOrDefault("whatsapp.src", whatsapp.src);
        storeOptionOrDefault("whatsapp.pwd", whatsapp.pwd);
        storeOptionOrDefault("whatsapp.dest", whatsapp.destinations);
        storeOptionOrDefault("dropbox.appkey", dropbox.appKey);
        storeOptionOrDefault("dropbox.appsecret", dropbox.appSecret);
        storeOptionOrDefault("dropbox.oauth-token", dropbox.oauthToken);
        storeOptionOrDefault("dropbox.oauth-secret", dropbox.oauthTokenSecret);
        storeOptionOrDefault("live-view.num-pictures", liveView.numPictures);
        storeOptionOrDefault("live-view.file-prefix", liveView.filePrefix);
        storeOptionOrDefault("live-view.inactivity-timeout", liveView.inactivityTimeout);

        // handle mutually exclusive options
        if (vm_.count("live-view")) {
            strt.liveView = true;
            strt.monitor = false;
        }
        else if (vm_.count("monitor")) {
            strt.liveView = false;
            strt.monitor = true;
        }

        return config_;
    }

    bool canContinue() const
    { return canContinue_; }

private:
    template <typename T>
    bool storeOptionOrDefault(const char* option, T& out)
    {
        bool res = vm_.count(option);
        if (res)
            out = vm_[option].as<T>();
        return res;
    }

    bool storeOptionOrDefault(const char* option, Configuration::Log::Level& out)
    {
        std::string value;
        if (!storeOptionOrDefault(option, value))
            return false;
        out = (value == "debug")
                ? Configuration::Log::Level::DEBUG
                : Configuration::Log::Level::INFO;
        return true;
    }

    bool storeOptionOrDefault(const char* option, std::chrono::seconds& out)
    {
        unsigned int value;
        if (!storeOptionOrDefault(option, value))
            return false;
        out = std::chrono::seconds{value};
        return true;
    }

    void initOptions()
    {
        // declare a group of options available only on command line
        po::options_description generic("Generic options");
        generic.add_options()
            ("help,h", "produce help message")
            ("version,v", "print version string")
            ("live-view", "start live view only")
            ("monitor", "start monitoring only")
            ;
        // declare a group of options available both on command line and in
        // config file
        po::options_description config("Configuration");
        config.add_options()
            ("startup.live-view", po::value<bool>(),
             "start live view by default on startup")
            ("startup.monitor", po::value<bool>(),
             "start monitor by default on startup")
            ("startup.monitor-timeout", po::value<unsigned int>(),
             "seconds before start monitoring")
            ("recording.archive-dir", po::value<std::string>(),
             "picture archive directory")
            ("recording.max-days", po::value<unsigned int>(),
             "max number of recording days to maintain")
            ("log.level", po::value<std::string>(), "log level (debug, info)")
            ("log.dir", po::value<std::string>(), "log directory")
            ("whatsapp.cc", po::value<std::string>(),
             "whatsapp source number country code")
            ("whatsapp.src", po::value<std::string>(), "whatsapp source number")
            ("whatsapp.pwd", po::value<std::string>(), "whatsapp password")
            ("whatsapp.dest", po::value<Configuration::StringList>()->composing(),
             "whatsapp destinations")
            ("dropbox.appkey", po::value<std::string>(), "dropbox app key")
            ("dropbox.appsecret", po::value<std::string>(), "dropbox app secret")
            ("dropbox.oauth-token", po::value<std::string>(), "dropbox oauth token")
            ("dropbox.oauth-secret", po::value<std::string>(),
             "dropbox oauth token secret")
            ("live-view.num-pictures", po::value<unsigned int>(), "number of "
             "pictures to buffer for live view")
            ("live-view.file-prefix", po::value<std::string>(), "file name "
             "prefix in which store the live view pictures")
            ("live-view.inactivity-timeout", po::value<unsigned int>(),
             "number of seconds of inactivity before stopping live view")
            ;

        cmdLineOpts_.add(generic).add(config);
        configOpts_.add(config);
    }

    Configuration config_;
    po::options_description cmdLineOpts_;
    po::options_description configOpts_;
    po::variables_map vm_;
    bool canContinue_ = true;
};


ConfigurationParser::ConfigurationParser()
    : impl_(new ConfigurationParserImpl)
{ }

void ConfigurationParser::parseCmdLine(int argc, char* argv[])
{
    return impl_->parseCmdLine(argc, argv);
}

void ConfigurationParser::parseConfigFile(const char* configFile)
{
    return impl_->parseConfigFile(configFile);
}

Configuration ConfigurationParser::getConfiguration()
{
    return impl_->getConfiguration();
}

bool ConfigurationParser::canContinue() const
{
    return impl_->canContinue();
}

} // namespace antifurto
