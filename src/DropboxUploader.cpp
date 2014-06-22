#include "DropboxUploader.hpp"
#include "Log.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "text/ToString.hpp"
#include "text/TextReplace.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace antifurto {

DropboxUploader::
DropboxUploader(std::string baseDir, std::string configFile)
    : baseDir_(baseDir), configFile_(configFile)
    , uploaderProcess_("./dropbox_uploader.sh")
{
    std::ifstream config(configFile.c_str());
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

bool DropboxUploader::good() const
{
    try {
        std::ostringstream args;
        args << "-f " << configFile_ << " info";
        runUploaderProcess(args.str());
        return true;
    }
    catch (...) {
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
    fs::path cfgTemplate{baseDir};
    cfgTemplate /= "dropbox.cfg.in";
    fs::path cfg{config::tmpDir()};
    cfg /= "dropbox.cfg";

    std::ifstream in{cfgTemplate.string()};
    std::ofstream out{cfg.string()};
    if (!in || !out)
        throw DropboxUploaderException("Error opening files");

    text::TextReplace replace;
    replace.addVariable("APP_KEY", c.dropbox.appKey);
    replace.addVariable("APP_SECRET", c.dropbox.appSecret);
    replace.addVariable("OAUTH_TOKEN", c.dropbox.oauthToken);
    replace.addVariable("OAUTH_TOKEN_SECRET", c.dropbox.oauthTokenSecret);
    replace.replaceVariables(in, out);
    return DropboxUploader(baseDir, cfg.string());
}


} // namespace antifurto
