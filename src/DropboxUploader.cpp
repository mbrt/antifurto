#include "DropboxUploader.hpp"
#include "Log.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "fs/Paths.hpp"
#include "text/ToString.hpp"
#include "text/TextReplace.hpp"

#include <sstream>
#include <fstream>

namespace antifurto {

DropboxUploader::
DropboxUploader(std::string baseDir, std::string configFile)
    : state_(State::UNKNOWN)
    , baseDir_(std::move(baseDir)), configFile_(std::move(configFile))
    , uploaderProcess_(fs::concatPaths(baseDir_, "dropbox_uploader.sh"))
{
    std::ifstream config(configFile_.c_str());
    if (!config.good())
        throw DropboxUploaderException("Config file not found");
}

void DropboxUploader::
uploadFile(const std::string& sourceFile, const std::string& destFile) const
{
    std::ostringstream args;
    args << "-f " << configFile_
         << " upload " << sourceFile
         << ' ' << destFile;

    runUploaderProcess(args.str());

    std::string stdout = uploaderProcess_.getStdOut();
    if (!stdout.find("DONE"))
        throw DropboxUploaderException(text::toString(
                "Cannot upload file ", sourceFile,
                                           "\nlog: ", stdout));
}

bool DropboxUploader::good()
{
    if (state_ != State::UNKNOWN)
        return state_ == State::GOOD;

    try {
        std::ostringstream args;
        args << "-f " << configFile_ << " info";
        runUploaderProcess(args.str());
        state_ = State::GOOD;
        return true;
    }
    catch (...) {
        state_ = State::BAD;
        return false;
    }
}

void DropboxUploader::runUploaderProcess(const std::string& args) const
{
    int retval =  uploaderProcess_.run(args, baseDir_);
    if (retval != 0) {
        std::ostringstream err;
        err << "Cannot run drobpox uploader process; return code: " << retval;
        LOG_ERROR << "Error uploading file. Log:\n"
                  << uploaderProcess_.getStdOut() << std::endl;
        throw DropboxUploaderException(err.str());
    }
}


DropboxUploader configureDropboxUploader(const Configuration& c,
                                         const std::string& baseDir)
{
    std::string cfg{fs::concatPaths(config::tmpDir(), "dropbox.cfg")};
    std::ifstream in{fs::concatPaths(baseDir, "config", "dropbox.cfg.in")};
    std::ofstream out{cfg};
    if (!in || !out)
        throw DropboxUploaderException("Error opening files");

    text::TextReplace replace;
    replace.addVariable("APP_KEY", c.dropbox.appKey);
    replace.addVariable("APP_SECRET", c.dropbox.appSecret);
    replace.addVariable("OAUTH_TOKEN", c.dropbox.oauthToken);
    replace.addVariable("OAUTH_TOKEN_SECRET", c.dropbox.oauthTokenSecret);
    replace.replaceVariables(in, out);
    out.close(); // explicitly close out file to catch errors
    return DropboxUploader(baseDir, cfg);
}


} // namespace antifurto
