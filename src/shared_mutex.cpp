#include <iostream>
#include <chrono>
#include <thread>

/* code from
https://github.com/vsoftco/qpp/blob/master/include/classes/timer.h
 */
/**
 * \class oqs::Timer
 * \brief High resolution timer
 * \note Code from
 * https://github.com/vsoftco/qpp/blob/master/include/classes/timer.h
 * \tparam T Tics duration, default is std::chrono::duration<double>,
 * i.e. seconds in double precision
 * \tparam CLOCK_T Clock's type, default is std::chrono::steady_clock,
 * not affected by wall clock changes during runtime
 */
template <typename T = std::chrono::duration<double>,
          typename CLOCK_T = std::chrono::steady_clock>
class Timer {
  protected:
    typename CLOCK_T::time_point start_, end_;

  public:
    /**
     * \brief Constructs an instance with the current time as the start point
     */
    Timer() noexcept : start_{CLOCK_T::now()}, end_{start_} {}

    /**
     * \brief Resets the chronometer
     *
     * Resets the start/end point to the current time
     *
     * \return Reference to the current instance
     */
    Timer& tic() noexcept {
        start_ = end_ = CLOCK_T::now();

        return *this;
    }

    /**
     * \brief Stops the chronometer
     *
     * Set the current time as the end point
     *
     * \return Reference to the current instance
     */
    Timer& toc() & noexcept {
        end_ = CLOCK_T::now();

        return *this;
    }

    /**
     * \brief Time passed in the duration specified by T
     *
     * \return Number of tics (specified by T) that passed between the
     * instantiation/reset and invocation of oqs::Timer::toc()
     */
    double tics() const noexcept {
        return static_cast<double>(
            std::chrono::duration_cast<T>(end_ - start_).count());
    }

    /**
     * \brief Duration specified by U
     *
     * \tparam U Duration, default is T, which defaults to
     * std::chrono::duration<double>, i.e. seconds in double precision
     *
     * \return Duration that passed between the
     * instantiation/reset and invocation of oqs::Timer::toc()
     */
    template <typename U = T>
    U get_duration() const noexcept {
        return std::chrono::duration_cast<U>(end_ - start_);
    }

    /**
     * \brief Default virtual destructor
     */
    virtual ~Timer() = default;

    friend std::ostream& operator<<(std::ostream& os, const Timer& rhs) {
        return os << rhs.tics();
    }
}; // class Timer

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <syncstream>
#include <thread>
 
class ThreadSafeCounter
{
public:
    ThreadSafeCounter() = default;
 
    // Multiple threads/readers can read the counter's value at the same time.
    unsigned int get() const
    {
        std::shared_lock lock(mutex_);
        std::cout << "READER: Value: " << value_ << std::endl;
        return value_;
    }

    // Only one thread/writer can increment/write the counter's value.
    void increment()
    {
        std::unique_lock lock(mutex_);
        ++value_;
        std::cout << "WRITER: Value: " << value_ << std::endl;
    }
 
    // Only one thread/writer can reset/write the counter's value.
    void reset()
    {
        std::unique_lock lock(mutex_);
        value_ = 0;
    }
 
private:
    mutable std::shared_mutex mutex_;
    unsigned int value_;
};
 


int main(int argn, char **argv)
{
    ThreadSafeCounter counter;

    auto print_counter = [&counter]()
    {
        for (int32_t i = 0; i < 10000; ++i)
        {
            std::cout <<std::this_thread::get_id() << ' ' << counter.get() << '\n';
        }
    };

    auto increment_counter = [&counter]()
    {
        for (int i = 0; i < 1000; ++i)
            counter.increment();
    };
 
    std::thread *thread[32];
    for (int i = 0; i < 32; ++i)
    {
        thread[i] = new std::thread(print_counter);
    }
    std::thread writer(increment_counter);
    std::thread writer2(increment_counter);
    std::thread writer3(increment_counter);
    for (int i = 0; i < 32; ++i)
    {
        thread[i]->join();
    }
    writer.join();
    writer2.join();
    writer3.join();

    Timer<std::chrono::milliseconds> t;
    std::this_thread::sleep_for(std::chrono::milliseconds(1232));
    t.toc();

    std::cout << "Time " << t << " ms" << std::endl;

    for (int i = 0; i < 100; ++i) {
        t.tic();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        t.toc();
        std::cout << "Time " << t << " ms" << std::endl;
    }
}