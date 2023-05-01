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
	this->emplace(
		job,
		0,
		job->id
	);
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
}
