#include <boost/lockfree/spsc_queue.hpp>
#include <atomic>
#include <thread>

#include "binsem.hpp"

int semaphore_signal = 0;

using Queue =
        boost::lockfree::spsc_queue<
            int,
            boost::lockfree::capacity<1024>>;

    class Worker
    {
    public:
        Worker()
            : working_(ATOMIC_FLAG_INIT), done_(false), semaphore_(0)
            , worker_([this]{ work(); })
        { } 
    
        ~Worker() {
            // exit even if the work has not been completed
            done_ = true;
            semaphore_.signal();
            worker_.join();
        }
    
        bool enqueue(int value) {
            bool enqueued = queue_.push(value);
            if (!working_.test_and_set()) {
                ++semaphore_signal;
                semaphore_.signal();
            }
            return enqueued;
        }
    
        void work() {
            int value;
            while (!done_) {
                semaphore_.wait();
                while (!done_ && queue_.pop(value)) {
                    std::cout << value << std::endl;
                    //std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
                working_.clear();
                while (!done_ && queue_.pop(value)) {
                    std::cout << value << std::endl;
                    //std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            }
        }
    
    private:
        std::atomic_flag working_;
        std::atomic<bool> done_;
        binsem semaphore_;
        Queue queue_;
        std::thread worker_;
    };

int main()
{
    Worker w;
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

    Worker w2;
    std::cout << "Semaphore signal: " << semaphore_signal << std::endl;
}
