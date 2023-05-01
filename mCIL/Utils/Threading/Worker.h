#pragma once
#include "../../cil-system.h"
#include "ThreadJob.h"

enum class WorkerStatus
{
	FREE,
	BUSY
};

class Worker
{
public:
	Worker();
	~Worker();

	void wait();
	bool schedule_job(ThreadJob* job);

	WorkerStatus status;
	
	JobID current_job_id;
	ThreadJob* current_job;
private:
	void run();

	std::thread* thread_;

	std::atomic<bool> waiting_;
	std::atomic<bool> continue_;
	std::atomic<bool> available_;
	std::atomic<bool> finished_;

	std::mutex lock_;
	std::condition_variable thread_ready_;
	std::condition_variable job_available_;
	std::condition_variable job_finished_;
};