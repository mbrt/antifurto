#include "concurrency/SpScQueue.hpp"
#include <thread>
#include <iostream>

using namespace antifurto::concurrency;

int main()
{
    auto worker = [](int a) { std::cout << a << std::endl; };
    SpScQueue<int, decltype(worker)> w(worker);
    for (int i = 0; i < 10000; ++i)
        w.enqueue(i);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (int i = 0; i < 1000; ++i)
        w.enqueue(i);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (int i = 0; i < 100000; ++i) {
        w.enqueue(i);
        //std::this_thread::sleep_for(std::chrono::nanoseconds(rand() % 20));
    }

    for (int i = 0; i < 10000; ++i)
        w.enqueue(i);

    SpScQueue<int, decltype(worker)> w2(worker);
    std::cout << "Completed\n";
}
