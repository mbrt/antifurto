#include "ipc/NamedPipe.hpp"

#define BOOST_TEST_MODULE pipe
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <future>
#include <fstream>
#include <thread>
#include <fcntl.h>

using namespace antifurto;
namespace bfs = boost::filesystem;


std::string readFileContents(const char* name)
{
    std::ifstream f{name};
    std::string result;
    std::getline(f, result);
    return result;
}

void writeToFile(const char* name, const char* contents)
{
    std::ofstream f{name};
    f << contents << std::endl;
}

void writeToFileNTimes(const char* name, int num)
{
    std::ofstream f{name};
    for (int i = 0; i < num; ++i)
        f << "write something to this file\n";
}

std::string readFileContentsFd(int fd)
{
    char buf[1024];
    while (::read(fd, buf, sizeof(buf)) > 0) { }
    return buf;
}

BOOST_AUTO_TEST_CASE(createDelete)
{
    const char* filename = "/tmp/tmppipe";
    if (bfs::exists(filename))
        bfs::remove(filename);
    BOOST_CHECK(!bfs::exists(filename));
    {
        ipc::NamedPipe pipe{filename};
        BOOST_CHECK(bfs::exists(filename));
        auto res = std::async(std::launch::async, readFileContents, filename);
        writeToFile(filename, "try to write");
        BOOST_CHECK_EQUAL(res.get(), "try to write");
    }
    BOOST_CHECK(!bfs::exists(filename));
}

BOOST_AUTO_TEST_CASE(readAfterWrite)
{
    const char* filename = "/tmp/tmppipe";
    {
        ipc::NamedPipe pipe{filename};
        int fd = ::open(filename, O_RDONLY | O_NONBLOCK);
        BOOST_CHECK_GT(fd, 0);
        int fd_flags = ::fcntl(fd, F_GETFL);
        BOOST_CHECK_GE(fd_flags, 0);
        int res = ::fcntl(fd, F_SETFL, fd_flags & ~O_NONBLOCK);
        BOOST_CHECK_GE(res, 0);

        auto wres = std::async(std::launch::async, writeToFileNTimes, filename, 1500);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        readFileContentsFd(fd);
        wres.get();
        ::close(fd);
    }
    BOOST_CHECK(!bfs::exists(filename));
}

BOOST_AUTO_TEST_CASE(multipleReadWrite)
{
    const char* filename = "/tmp/tmppipe";
    if (bfs::exists(filename))
        bfs::remove(filename);
    BOOST_CHECK(!bfs::exists(filename));
    {
        ipc::NamedPipe pipe{filename};
        BOOST_CHECK(bfs::exists(filename));
        auto res = std::async(std::launch::async, readFileContents, filename);
        writeToFile(filename, "try to write");
        BOOST_CHECK_EQUAL(res.get(), "try to write");

        res = std::async(std::launch::async, readFileContents, filename);
        writeToFile(filename, "try to write");
        BOOST_CHECK_EQUAL(res.get(), "try to write");
    }
    BOOST_CHECK(!bfs::exists(filename));
}

BOOST_AUTO_TEST_CASE(twoReaders)
{
    const char* filename = "/tmp/tmppipe";
    if (bfs::exists(filename))
        bfs::remove(filename);
    BOOST_CHECK(!bfs::exists(filename));
    {
        ipc::NamedPipe pipe{filename};
        int fd = ::open(filename, O_RDONLY | O_NONBLOCK);
        BOOST_CHECK_GT(fd, 0);
        BOOST_CHECK(bfs::exists(filename));
        auto res = std::async(std::launch::async, readFileContents, filename);
        writeToFile(filename, "try to write");
        ::close(fd);
        BOOST_CHECK_EQUAL(res.get(), "try to write");

        fd = ::open(filename, O_RDONLY | O_NONBLOCK);
        BOOST_CHECK_GT(fd, 0);
        res = std::async(std::launch::async, readFileContents, filename);
        writeToFile(filename, "try to write");
        ::close(fd);
        BOOST_CHECK_EQUAL(res.get(), "try to write");
    }
    BOOST_CHECK(!bfs::exists(filename));
}
