#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "linde.h"

#include <thread>
#include <future>
#include <queue>
#include <functional>

namespace linde
{

class WorkerThread
{
    std::thread                             m_thread;
    std::mutex                              m_mutex;
    std::condition_variable                 m_condition;

    std::queue<std::packaged_task<void()> > m_tasks;

    std::atomic_bool                        m_terminate;


public:
    WorkerThread();
    ~WorkerThread();
    void start();
    std::future<void>  execute(std::function<void()> f);
};

WorkerThread::WorkerThread() :
    m_mutex(),
    m_condition(),
    m_tasks(),
    m_terminate(false)
{

}

WorkerThread::~WorkerThread()
{
    m_terminate.store(true);
    m_condition.notify_all();
    if (m_thread.joinable())
        m_thread.join();
}

void WorkerThread::start()
{
    m_terminate.store(false);

    m_thread = std::thread([&]()
    {
        while(!m_terminate.load())
        {
            std::packaged_task<void()> f;
            {
                std::unique_lock<std::mutex> l(m_mutex);
                if (m_tasks.empty())
                {
                    m_condition.wait(l, [&]{return !m_tasks.empty();});
                }
                f = std::move(m_tasks.front());
                m_tasks.pop();
            }
            if (!f.valid()) return;
            f();
        }
    });
}

std::future<void> WorkerThread::execute(std::function<void()> f)
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

}// namespace linde

#endif // WORKERTHREAD_H
