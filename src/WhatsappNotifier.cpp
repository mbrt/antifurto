#include "WhatsappNotifier.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "text/ToString.hpp"
#include "text/TextReplace.hpp"

#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace antifurto {

WhatsappNotifier::WhatsappNotifier(std::string baseDir, std::string configFile)
    : baseDir_(baseDir)
    , configFile_(configFile)
    , yowsupProcess_("./yowsup-cli")
{ }

void WhatsappNotifier::send(const std::string& dest, const std::string& msg)
{
    std::ostringstream args;
    args << "-c " << configFile_
         << " -s " << dest
         << " \"" << msg << '\"';

    int retval =  yowsupProcess_.run(args.str(), baseDir_);
    if (retval != 0) {
        std::ostringstream err;
        err << "Cannot run yowsup process; return code: " << retval;
        throw WhatsappNotifierException(err.str());
    }

    std::string stdout = yowsupProcess_.getStdOut();
    if (!stdout.find("Sent message"))
        throw WhatsappNotifierException(
                text::toString("Cannot send message\nlog: ",
                               stdout));
}

WhatsappNotifier
configureWhatsappNotifier(const Configuration& c, const std::string& baseDir)
{
    fs::path cfgTemplate{baseDir};
    cfgTemplate /= "config/whatsapp.cfg.in";
    fs::path cfg{config::tmpDir()};
    cfg /= "whatsapp.cfg";

    {
        std::ifstream in{cfgTemplate.string()};
        std::ofstream out{cfg.string()};
        if (!in || !out)
            throw WhatsappNotifierException("Error opening files");

        text::TextReplace replace;
        replace.addVariable("SRC_PHONE", c.whatsapp.src);
        replace.addVariable("PWD", c.whatsapp.pwd);
        replace.addVariable("CC", c.whatsapp.cc);
        replace.replaceVariables(in, out);
    } // close files here
    return WhatsappNotifier(baseDir, cfg.string());
}

} // namespace antifurto
