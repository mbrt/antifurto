#pragma once

#include <string>
#include "Exception.hpp"
#include "ipc/Process.hpp"

namespace antifurto {

class Configuration;

/// This class uploads files to a Dropbox folder, by using a
/// `dropbox_uploader.sh` script
class DropboxUploader
{
public:
    DropboxUploader() : state_(State::BAD) {}
    DropboxUploader(std::string baseDir, std::string configFile);
    void uploadFile(const std::string& sourceFile,
                    const std::string& destFile) const;
    bool good();

private:
    void runUploaderProcess(const std::string& args) const;

    enum class State {
        UNKNOWN, GOOD, BAD
    };

    State state_;
    std::string baseDir_;
    std::string configFile_;
    mutable ipc::Process uploaderProcess_;
};

/// This function configures a DropboxUploader using a template file and
/// variables stored in the given configuration
DropboxUploader configureDropboxUploader(const Configuration& c,
                                         const std::string& baseDir);

class DropboxUploaderException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto
