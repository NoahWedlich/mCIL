#include "ThreadManager.h"

std::mutex ThreadManager::static_lock_{};

size_t ThreadManager::total_thread_count_ = 0;
JobID ThreadManager::next_id_ = 1;

ThreadManager::ThreadManager(size_t thread_count)
    : lock_(), local_thread_count_(thread_count), jobs_(), workers_()
{
    std::unique_lock static_lock{ static_lock_ };
    ThreadManager::total_thread_count_++;

    workers_.reserve(thread_count);

    for (size_t i = 0; i < thread_count; i++)
    {
        workers_.push_back(new Worker(&jobs_));
    }
}

ThreadManager::~ThreadManager()
{
    for (Worker* worker : workers_)
    {
        delete worker;
    }
}

void ThreadManager::wait(JobID id)
{
    const JobQueueEntry& entry = jobs_.get_job_entry(id);
    while (!entry.job->worker)
    {
        jobs_.wait_for_assign();
    }
    entry.job->worker->wait();
}

void ThreadManager::wait_all()
{
    while (!jobs_.empty())
    {
        jobs_.wait_for_assign();
    }
    for (Worker* worker : workers_)
    {
        worker->wait();
    }
}

void ThreadManager::resize(size_t thread_count)
{
    //TODO: Add ThreadManager::resize
}
