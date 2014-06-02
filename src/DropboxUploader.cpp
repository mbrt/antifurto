#include "DropboxUploader.hpp"
#include "meta/ToString.hpp"
#include <sstream>

namespace antifurto {

DropboxUploader::
DropboxUploader(std::string baseDir, std::string configFile)
    : baseDir_(baseDir), configFile_(configFile)
    , uploaderProcess_("./dropbox_uploader.sh")
{ }

void DropboxUploader::
uploadFile(const std::string& sourceFile, const std::string& destFile)
{
    std::ostringstream args;
    args << "-f " << configFile_
         << " upload " << sourceFile
         << ' ' << destFile;

    int retval =  uploaderProcess_.run(args.str(), baseDir_);
    if (retval != 0) {
        std::ostringstream err;
        err << "Cannot run drobpox uploader process; return code: " << retval;
        throw DropboxUploaderException(err.str());
    }

    std::string stdout = uploaderProcess_.getStdOut();
    if (!stdout.find("DONE"))
        throw DropboxUploaderException(meta::toString(
                "Cannot upload file ", sourceFile,
                "\nlog: ", stdout));
}


} // namespace antifurto
