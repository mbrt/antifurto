#pragma once

#include <string>
#include <vector>

#include "Exception.hpp"
#include "ipc/Process.hpp"


namespace antifurto {

/// This class manages notifications using mail program.
///
/// This class assumes postfix daemon is configured.
class MailNotifier
{
public:
    using ContactList = std::vector<std::string>;

    MailNotifier();

    /// Send a mail to a list of recipients with the given subject and
    /// message body.
    void send(ContactList const& dest,
              std::string const& subject,
              std::string const& body);

private:
    ipc::Process mailProcess_;
};


class MailNotifierException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto
