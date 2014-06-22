#include "DropboxUploader.hpp"
#include "Log.hpp"
#include "text/ToString.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

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


} // namespace antifurto
