#include "ipc/PosixSignalHandler.hpp"
#include "ipc/Fork.hpp"

#include <iostream>
#include <thread>
#include <signal.h>

using namespace antifurto;
using namespace antifurto::ipc;

// NOTE: Cannot use boost test as it uses execution monitor (not signal friendly)
void check(bool pred, const char* file, int line)
{
    if (!pred) {
        std::cerr << file << ":" << line << "Assertion failed\n";
        ::exit(EXIT_FAILURE);
    }
}

#define CHECK(PRED) check(PRED, __FILE__, __LINE__)

constexpr std::chrono::milliseconds pollingTime() {
    return std::chrono::milliseconds{1};
}


void handleSignal(int sig)
{
    std::cout << "Caught signal " << sig << std::endl;
    std::cout.flush();
}

int termChild()
{
    bool usr2Called = false;
    PosixSignalHandler handler{pollingTime()};
    handler.setSignalHandler(SIGUSR1, &handleSignal);
    handler.setSignalHandler(SIGTERM, [&](int signal){
        handleSignal(signal);
        handler.leaveSignalHandlingLoop();
    });
    handler.setSignalHandler(SIGINT, [&](int signal){
        handleSignal(signal);
        handler.leaveSignalHandlingLoop();
    });
    handler.setSignalHandler(SIGUSR2, [&](int signal){
        handleSignal(signal);
        usr2Called = true;
    });
    handler.enterSignalHandlingLoop();
    return usr2Called;
}

void forkTest()
{
    ipc::ChildProcess c = ipc::forkAndCall([]{
       return 38;
    });
    int res = c.wait();
    CHECK(res == 38);
}

void termTest()
{
    ipc::ChildProcess c = ipc::forkAndCall(termChild);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    c.kill(SIGUSR1);
    c.kill(SIGTERM);
    int res = c.wait();
    CHECK(res == 0);
}

// test that the wait does not block other threads
void threadsTest()
{
    ipc::ChildProcess c = ipc::forkAndCall(termChild);
    int counter = 0;
    std::thread t1{[&]{
        while (counter < 10) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++counter;
        }
    }};
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    c.kill(SIGUSR1);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread t2{[&]{
        t1.join();
        CHECK(counter == 10);
        c.kill(SIGTERM);
    }};
    c.wait();
    t2.join();
}

void signalHandledTest()
{
    ipc::ChildProcess c = ipc::forkAndCall(termChild);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    c.kill(SIGUSR2);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    c.kill(SIGTERM);
    int result = c.wait();
    CHECK(result != 0);
}

struct Handler
{
    Handler(int signal)
        : signal(signal), received(-1)
    { }

    void operator()()
    {
        PosixSignalHandler handler{pollingTime()};
        handler.setSignalHandler(signal, [&](int sig){
            received = sig;
            handler.leaveSignalHandlingLoop();
        });
        handler.enterSignalHandlingLoop();
    }

    int signal;
    int received;
};

int multiHandlerProc()
{
    Handler h1{SIGUSR1};
    std::thread t{[&]{
        h1();
    }};
    Handler h2{SIGUSR2};
    h2();
    t.join();
    return h1.received == SIGUSR1 && h2.received == SIGUSR2;
}

// test if two different handlers can live together
void multiHandlerTest()
{
    ipc::ChildProcess p = ipc::forkAndCall(multiHandlerProc);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    p.kill(SIGUSR1);
    p.kill(SIGUSR2);
    int result = p.wait();
    CHECK(result != 0);
}

int blockProc()
{
    bool called = false;
    bool term = false;
    PosixSignalHandler handler{pollingTime()};
    handler.setSignalHandler(SIGUSR1, [&](int){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        called = true;
    });
    handler.ignoreSignal(SIGUSR2);
    handler.setSignalHandler(SIGTERM, [&](int){
        term = true;
        handler.leaveSignalHandlingLoop();
    });
    handler.enterSignalHandlingLoop();
    return called && term;
}

// test block signals without terminate
void blockTest()
{
    ipc::ChildProcess p = ipc::forkAndCall(blockProc);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    p.kill(SIGUSR1);
    p.kill(SIGUSR1);
    p.kill(SIGUSR2);
    p.kill(SIGTERM);
    int result = p.wait();
    CHECK(result);
}


int main(int argc, char**)
{
    if (argc > 1) {
        return termChild();
    }
    forkTest();
    termTest();
    threadsTest();
    signalHandledTest();
    //multiHandlerTest(); // this fails: no multi handlers allowed now
    blockTest();
    return 0;
}
