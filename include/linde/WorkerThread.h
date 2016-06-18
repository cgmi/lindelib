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
    std::mutex                              m_mutex;
    std::mutex                              m_workMutex;
    std::condition_variable                 m_condition;
    std::queue<std::packaged_task<void()> > m_tasks;
    std::future<void>                       m_guard;
    bool                                    m_working;

    bool keepWorking();
    void stopWorking();

public:
    WorkerThread();
    ~WorkerThread();
    void start();
    std::future<void>  execute(std::function<void()> f);
};

bool WorkerThread::keepWorking()
{
    std::unique_lock<std::mutex> lock(m_workMutex);
    return m_working;
}

void WorkerThread::stopWorking()
{
    std::unique_lock<std::mutex> lock(m_workMutex);
    m_working = false;
}

WorkerThread::WorkerThread() {}

WorkerThread::~WorkerThread()
{
    stopWorking();
    m_condition.notify_all();
    if (m_guard.valid())
        m_guard.wait();
}

void WorkerThread::start()
{
    m_working = true;

    m_guard = std::async(std::launch::async, [&]()
    {
        while(keepWorking())
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
