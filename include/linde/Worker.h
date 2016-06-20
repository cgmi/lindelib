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


class Worker
{
    std::thread                             m_thread;
    std::mutex                              m_mutex;
    std::condition_variable                 m_condition;

    std::deque<std::packaged_task<void()> > m_tasks;

    std::atomic_bool                        m_terminate;


public:
    Worker();
    ~Worker();

    void start();

    template <typename Ret>
    std::future<Ret> execute(std::function<Ret()> &&f);
};

Worker::Worker() :
    m_mutex(),
    m_condition(),
    m_tasks(),
    m_terminate(false)
{

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
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                while (m_tasks.empty())
                {
                    m_condition.wait(lock, [&]
                    {
                        return !m_tasks.empty();
                    });
                }
                f = std::move(m_tasks.front());
                m_tasks.pop_front();
            }
            f();
        }
    });
}

template <typename Ret>
std::future<Ret> Worker::execute(std::function<Ret()> &&f)
{
    std::packaged_task<Ret()> p(std::move(f));
    std::future<Ret> r = p.get_future();
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.emplace_back(std::move(p));
    }
    m_condition.notify_one();
    return r;
}

} // namespace thread

}// namespace linde

#endif // WORKERTHREAD_H
