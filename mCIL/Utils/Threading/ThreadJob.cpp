#include "ThreadJob.h"

ThreadJob::ThreadJob(JobID id)
	: id(id), status(JobStatus::PENDING), worker(nullptr)
{}

void ThreadJob::execute()
{
	status = JobStatus::RUNNING;
	try
	{ run_(); }
	catch (const std::exception& e)
	{
		status = JobStatus::ERROR;
		throw e;
	}
	status = JobStatus::FINISHED;
}
