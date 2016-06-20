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

    std::queue<std::packaged_task<void()> > m_tasks;

    std::atomic_bool                        m_terminate;


public:
    Worker();
    ~Worker();

    void start();

    auto execute(std::function<void()> f);
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
                std::unique_lock<std::mutex> l(m_mutex);
                if (m_tasks.empty())
                {
                    m_condition.wait(l, [&]
                    {
                        return !m_tasks.empty();
                    });
                }
                f = std::move(m_tasks.front());
                m_tasks.pop();
            }
            f();
        }
    });
}

auto Worker::execute(std::function<void()> f)
{
    std::packaged_task<void()> p(f);
    auto r = p.get_future();
    {
        std::unique_lock<std::mutex> l(m_mutex);
        m_tasks.push(std::move(p));
        m_condition.notify_one();
    }
    return r;
}

} // namespace thread

}// namespace linde

#endif // WORKERTHREAD_H
