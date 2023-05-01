#include "Worker.h"

Worker::Worker()
    : status(WorkerStatus::FREE), current_job_id(0), current_job(nullptr), lock_(),
      waiting_(false), continue_(true), available_(false), finished_(false)
{
    thread_ = new std::thread(&Worker::run, this);

    std::unique_lock lock{ lock_ };
    thread_ready_.wait(lock, [this] {return waiting_ == true; });
}

Worker::~Worker()
{
    continue_.store(false);
    thread_->join();
    delete thread_;
}

void Worker::wait()
{
    if (!finished_)
    {
        std::unique_lock lock{ lock_ };
        job_finished_.wait(lock, [this] {return finished_ == true; });
    }
}

bool Worker::schedule_job(ThreadJob* job)
{
    if (waiting_)
    {
        current_job_id = job->id;
        current_job = job;
        available_.store(true);
        job_available_.notify_all();
        return true;
    }
    return false;
}

void Worker::run()
{
    while (continue_)
    {
        waiting_.store(true);
        status = WorkerStatus::FREE;
        thread_ready_.notify_all();

        if (!available_)
        {
            std::unique_lock lock{ lock_ };
            job_available_.wait(lock, [this] { return available_ == true; });
        }

        finished_.store(false);
        waiting_.store(false);
        available_.store(false);

        status = WorkerStatus::BUSY;

        current_job->execute();

        finished_.store(true);
        job_finished_.notify_all();
    }
}
