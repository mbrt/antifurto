#include "WhatsappNotifier.hpp"
#include "meta/ToString.hpp"
#include <sstream>


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
                meta::toString("Cannot send message\nlog: ",
                               stdout));
}

} // namespace antifurto
