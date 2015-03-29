#include "MailNotifier.hpp"
#include "text/ToString.hpp"

#include <sstream>

namespace antifurto {

MailNotifier::MailNotifier(std::string const& baseDir)
    : mailProcess_(baseDir + "/send_mail.sh")
{ }

void MailNotifier::
send(const MailNotifier::ContactList& dest,
     const std::string& sender,
     const std::string& subject,
     const std::string& body)
{
    std::ostringstream params;
    params << "antifurto "
           << '\"' << sender << "\" "
           << '\"' << subject << "\" "
           << '\"' << body << "\" ";
    for (auto const& d : dest)
        params << ' ' << d;
    int retval = mailProcess_.run(params.str());
    if (retval)
        throw MailNotifierException(
                text::toString("Cannot send mail, return code: ", retval));
}

} // namespace antifurto
