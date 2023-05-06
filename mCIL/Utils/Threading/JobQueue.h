#pragma once
#include "../../cil-system.h"
#include "ThreadJob.h"

struct JobQueueEntry
{
	ThreadJob* job;
	int priority;
	JobID id;
};

struct JobQueueCmp
{
	bool operator()(const JobQueueEntry& a, const JobQueueEntry& b);
};

class JobQueue : public std::priority_queue<JobQueueEntry, std::vector<JobQueueEntry>, JobQueueCmp>
{
public:
	JobQueue();

	void push_job(ThreadJob* job);
	ThreadJob* request_job();

	const JobQueueEntry& get_job_entry(JobID id) const;

	void wait_for_assign();
private:
	std::mutex lock_;
	std::condition_variable job_available_;
	std::condition_variable job_assigned_;
};