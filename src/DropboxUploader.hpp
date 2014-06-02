#pragma once

#include <string>
#include "Exception.hpp"
#include "ipc/Process.hpp"

namespace antifurto {

/// This class manages notifications using WhatsApp
class DropboxUploader
{
public:
    DropboxUploader(std::string baseDir, std::string configFile);
    void uploadFile(const std::string& sourceFile, const std::string& destFile);

private:
    std::string baseDir_;
    std::string configFile_;
    ipc::Process uploaderProcess_;
};

class DropboxUploaderException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto
