#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "linde.h"

#include <thread>
#include <future>
#include <queue>
#include <functional>

namespace linde
{

namespace thread
{

template <typename T, typename Work>
auto then(std::future<T> f, Work w) -> std::future<decltype(w(f.get()))>
{
    return std::async([](std::future<T> f, Work w)
                      { return w(f.get()); }, std::move(f), std::move(w));
}

template <typename Work>
auto then(std::future<void> f, Work w) -> std::future<decltype(w())>
{
    return std::async([](std::future<void> f, Work w)
                      { f.wait(); return w(); }, std::move(f), std::move(w));
}

class Worker
{
    std::thread                             m_thread;
    std::mutex                              m_mutex;
    std::condition_variable                 m_condition;

    std::deque<std::packaged_task<void()> > m_tasks;

    std::atomic_bool                        m_terminate;

    void start();

public:
    Worker();
    ~Worker();

    template <class Function>
    std::future<typename std::result_of<Function()>::type> submit(Function &&function);

};

Worker::Worker() :
    m_mutex(),
    m_condition(),
    m_tasks(),
    m_terminate(false)
{
    start();
}

Worker::~Worker()
{
    m_terminate = true;

    m_condition.notify_all();

    if (m_thread.joinable())
        m_thread.join();
}

void Worker::start()
{
    m_terminate = false;

    m_thread = std::thread([&]()
    {
        while(!m_terminate)
        {
            std::packaged_task<void()> f;

            std::unique_lock<std::mutex> lock(m_mutex);

            while (m_tasks.empty())
            {
                m_condition.wait(lock);
                if (m_terminate)
                {
                    return;
                }
            }

            f = std::move(m_tasks.front());
            m_tasks.pop_front();

            lock.unlock();

            f();
        }
    });
}

template <class Function>
std::future<typename std::result_of<Function()>::type> Worker::submit(Function &&function)
{
    std::packaged_task<typename std::result_of<Function()>::type()> task(std::move(function));
    auto result = task.get_future();
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.emplace_back(std::move(task));
    }
    m_condition.notify_one();
    return result;
}

} // namespace thread

}// namespace linde

#endif // WORKERTHREAD_H
