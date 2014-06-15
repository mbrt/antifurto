#include "Config.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace antifurto {

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
    }

    void parseConfigFile(const char* configFile)
    {
        po::store(po::parse_config_file<char>(configFile, configOpts_), vm_);
        po::notify(vm_);
    }

    Configuration getConfiguration()
    {
        Configuration::Whatsapp& whatsapp = config_.whatsapp;
        Configuration::Dropbox& dropbox = config_.dropbox;

        storeOptionOrDefault("whatsapp.src", whatsapp.src);
        storeOptionOrDefault("whatsapp.pwd", whatsapp.pwd);
        storeOptionOrDefault("whatsapp.dest", whatsapp.destinations);
        storeOptionOrDefault("dropbox.appkey", dropbox.appKey);
        storeOptionOrDefault("dropbox.appsecret", dropbox.appSecret);
        storeOptionOrDefault("dropbox.oauth-token", dropbox.oauthToken);
        storeOptionOrDefault("dropbox.oauth-secret", dropbox.oauthTokenSecret);

        return config_;
    }

private:
    template <typename T>
    bool storeOptionOrDefault(const char* option, T& out)
    {
        bool res = vm_.count(option);
        if (res)
            out = vm_[option].as<T>();
        return res;
    }

    void initOptions()
    {
        // declare a group of options available only on command line
        po::options_description generic("Generic options");
        generic.add_options()
            ("help,h", "produce help message")
            ("version,v", "print version string")
            ;
        // declare a group of options available both on command line and in
        // config file
        po::options_description config("Configuration");
        config.add_options()
            ("whatsapp.src", po::value<std::string>(), "whatsapp source number")
            ("whatsapp.pwd", po::value<std::string>(), "whatsapp password")
            ("whatsapp.dest", po::value<Configuration::StringList>()->composing(),
             "whatsapp destinations")
            ("dropbox.appkey", po::value<std::string>(), "dropbox app key")
            ("dropbox.appsecret", po::value<std::string>(), "dropbox app secret")
            ("dropbox.oauth-token", po::value<std::string>(), "dropbox oauth token")
            ("dropbox.oauth-secret", po::value<std::string>(),
             "dropbox oauth token secret")
            ;

        cmdLineOpts_.add(generic).add(config);
        configOpts_.add(config);
    }

    Configuration config_;
    po::options_description cmdLineOpts_;
    po::options_description configOpts_;
    po::variables_map vm_;
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

} // namespace antifurto
