#pragma once
#include "../../cil-system.h"
#include "ThreadJob.h"
#include "JobQueue.h"

enum class WorkerStatus
{
	FREE,
	BUSY
};

class Worker
{
public:
	Worker(JobQueue* queue);
	~Worker();

	void wait();

	WorkerStatus status;
	
	JobID current_job_id;
	ThreadJob* current_job;
private:
	void run();

	void get_job_();

	std::thread* thread_;

	JobQueue* job_queue_;

	std::atomic<bool> continue_;
	std::atomic<bool> finished_;

	std::mutex lock_;
	std::condition_variable thread_ready_;
	std::condition_variable job_finished_;
};