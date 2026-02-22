#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <thread>
#include <map>

class WriterPrioritySharedMutex {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int readers = 0;
    int writers_waiting = 0;
    bool writer_active = false;

public:
    // Exclusive lock
    void lock()
    {
        std::unique_lock<std::mutex> lock(mtx);
        writers_waiting++;

        // Wait until no active writer AND no active readers
        std::cout << "Wait to write!" << std::endl;
        cv.wait(lock, [this] { return !writer_active && readers == 0; });
        writers_waiting--;
        writer_active = true;
        std::cout << "Write Now!" << std::endl;
    }

    void unlock()
    {
        std::lock_guard<std::mutex> lock(mtx);
        writer_active = false;
        // Notify all: wakes up both waiting writers AND waiting readers
        // Readers will re-check 'writers_waiting == 0'
        cv.notify_all(); 
    }

    // Shared lock
    void lock_shared()
    {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait if a writer is active OR if writers are waiting
        std::cout << "Wait to read!" << std::endl;

        cv.wait(lock, [this] { return !writer_active && writers_waiting == 0; });
        readers++;

        std::cout << "Read Now!" << std::endl;
    }

    void unlock_shared()
    {
        std::lock_guard<std::mutex> lock(mtx);
        readers--;
        // Only notify if we were the last reader (likely waking a writer)
        if (readers == 0) {
            cv.notify_all();
        }
    }
};

// Your class protecting a simple shared resource
class ThreadSafeRegistry {
    std::map<std::string, std::string> config;
    WriterPrioritySharedMutex rw_mutex;

public:
    // Multiple readers can call this simultaneously as long as no writer is waiting
    std::string getattr(const std::string& key) {
        rw_mutex.lock_shared();
        std::string val = config.count(key) ? config[key] : "NOT_FOUND";
        rw_mutex.unlock_shared();
        return val;
    }

    // This will block new readers immediately to ensure the update happens ASAP
    void update(const std::string& key, const std::string& val) {
        rw_mutex.lock();
        config[key] = val;
        rw_mutex.unlock();
    }
};

int main()
{
    ThreadSafeRegistry registry;
    registry.update("api_url", "https://api.v1.com");

    // Scenario: High-frequency reader threads
    auto reader_func = [&]() {
        for(int i = 0; i < 10000; ++i) {
            std::cout << "Reader saw: " << registry.getattr("api_url") << "\n";
        }
    };

    // Scenario: Occasional writer thread
    auto writer_func = [&]() {
        for(int i = 0; i < 10000; ++i)
        {
            std::string url = "https://api.v" + std::to_string(i) + ".com";
            registry.update("api_url", url);
            std::cout << "Reader saw: " << registry.getattr("api_url") << "\n";
        }

        std::cout << "--- WRITER UPDATED URL ---" << std::endl;
    };

    std::thread r1(reader_func), r2(reader_func), w1(writer_func);

    r1.join(); r2.join(); w1.join();
    return 0;
}
