#include "Config.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace antifurto {
namespace config {
namespace { // anon

template <typename T>
bool storeOptionOrDefault(po::variables_map& vm, const char* option, T& out)
{
    bool res = vm.count(option);
    if (res)
        out = vm[option].as<T>();
    return res;
}

} // anon namespace

Configuration::Configuration(const char* configFile)
{
    parseConfigFile(configFile);
}

void Configuration::
parseConfigFile(const char* configFile)
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
            ("whatsapp.dest", po::value<StringList>()->composing(),
             "Whatsapp destinations")
            ;

    po::options_description cmdline_opts, config_opts;
    cmdline_opts.add(generic).add(config);
    config_opts.add(config);

    // parsing
    po::variables_map vm;
    po::store(po::parse_config_file<char>(configFile, config_opts), vm);
    po::notify(vm);

    // store options
    storeOptionOrDefault(vm, "whatsapp.src", whatsapp.src);
    storeOptionOrDefault(vm, "whatsapp.pwd", whatsapp.pwd);
    storeOptionOrDefault(vm, "whatsapp.dest", whatsapp.destinations);
}

} // namespace config
} // namespace antifurto
