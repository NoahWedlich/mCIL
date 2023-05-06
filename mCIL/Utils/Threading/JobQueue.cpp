#include "JobQueue.h"

bool JobQueueCmp::operator()(const JobQueueEntry& a, const JobQueueEntry& b)
{
	return a.priority < b.priority;
}

JobQueue::JobQueue()
	: std::priority_queue<JobQueueEntry, std::vector<JobQueueEntry>, JobQueueCmp>()
{}

void JobQueue::push_job(ThreadJob* job)
{
	//TODO: Add priority
	this->emplace(
		job,
		0,
		job->id
	);
	job_available_.notify_one();
}

ThreadJob* JobQueue::request_job()
{
	std::unique_lock lock{ lock_ };
	while (empty())
	{
		job_available_.wait(lock, [this] {return !empty(); });
	}
	JobQueueEntry entry = top();
	pop();
	job_assigned_.notify_all();
	return entry.job;
}

const JobQueueEntry& JobQueue::get_job_entry(JobID id) const
{
	for (const JobQueueEntry& entry : this->c)
	{
		if (entry.id == id)
		{
			return entry;
		}
	}
	throw std::invalid_argument("No Job with given id found");
}

void JobQueue::wait_for_assign()
{
	size_t orig_size = size();
	std::unique_lock lock{ lock_ };
	job_assigned_.wait(lock, [this, orig_size] {return size() < orig_size; });
}
