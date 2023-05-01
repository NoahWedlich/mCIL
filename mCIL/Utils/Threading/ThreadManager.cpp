#include "ThreadManager.h"

std::mutex ThreadManager::static_lock_{};

size_t ThreadManager::total_thread_count_ = 0;
size_t ThreadManager::next_id_ = 1;

ThreadManager::ThreadManager(size_t thread_count)
{
}

ThreadManager::~ThreadManager()
{
}

void ThreadManager::wait(JobID id)
{
}

void ThreadManager::wait_all()
{
}

void ThreadManager::request_job()
{
}

void ThreadManager::resize(size_t thread_count)
{
}

bool ThreadManager::JobQueueCmp::operator()(const JobQueueEntry& a, const JobQueueEntry& b)
{
    return false;
}
