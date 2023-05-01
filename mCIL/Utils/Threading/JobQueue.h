#pragma once
#include "../../cil-system.h"
#include "ThreadJob.h"
#include "Worker.h"

struct JobQueueEntry
{
	ThreadJob* job;
	int priority;
	JobID id;
	Worker* worker = nullptr;
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

	const JobQueueEntry& get_job_entry(JobID id) const;
};