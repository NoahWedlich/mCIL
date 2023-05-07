#include "Worker.h"

Worker::Worker(JobQueue* queue)
    : status(WorkerStatus::FREE), current_job_id(0), current_job(nullptr), lock_(), job_queue_(queue),
      continue_(true), finished_(false)
{
    thread_ = new std::thread(&Worker::run, this);
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

void Worker::run()
{
    while (continue_)
    {
        status = WorkerStatus::FREE;
        thread_ready_.notify_all();

        get_job_();

        finished_.store(false);

        status = WorkerStatus::BUSY;

        current_job->execute();

        finished_.store(true);
        job_finished_.notify_all();
    }
}

void Worker::get_job_()
{
    ThreadJob* job = job_queue_->request_job();
    current_job_id = job->id;
    current_job = job;
}