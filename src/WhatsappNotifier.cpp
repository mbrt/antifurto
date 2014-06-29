#include "WhatsappNotifier.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "fs/Paths.hpp"
#include "text/ToString.hpp"
#include "text/TextReplace.hpp"

#include <fstream>
#include <sstream>

namespace antifurto {

WhatsappNotifier::WhatsappNotifier(std::string baseDir, std::string configFile)
    : baseDir_(std::move(baseDir))
    , configFile_(std::move(configFile))
    , yowsupProcess_(fs::concatPaths(baseDir_, "yowsup-cli"))
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
    std::string cfg{fs::concatPaths(config::tmpDir(), "whatsapp.cfg")};
    std::ifstream in{fs::concatPaths(baseDir, "config", "whatsapp.cfg.in")};
    std::ofstream out{cfg};
    if (!in || !out)
        throw WhatsappNotifierException("Error opening files");

    text::TextReplace replace;
    replace.addVariable("SRC_PHONE", c.whatsapp.src);
    replace.addVariable("PWD", c.whatsapp.pwd);
    replace.addVariable("CC", c.whatsapp.cc);
    replace.replaceVariables(in, out);
    out.close(); // explicitly close out file to catch errors
    return WhatsappNotifier(baseDir, cfg);
}

} // namespace antifurto
