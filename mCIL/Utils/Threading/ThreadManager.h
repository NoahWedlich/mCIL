#pragma once
#include "../../cil-system.h"
#include "JobResult.h"
#include "ThreadJob.h"
#include "FunctionJob.h"
#include "Worker.h"

class ThreadManager
{
public:
	ThreadManager(size_t thread_count);
	~ThreadManager();

	template <typename Ret, typename... Ts>
	JobResult<Ret>* schedule_func(std::function<Ret(Ts...)>, Ts... Args);

	void wait(JobID id);
	void wait_all();

	void request_job();

	void resize(size_t thread_count);
private:
	std::mutex lock_;
	static std::mutex static_lock_;

	static size_t total_thread_count_;
	size_t local_thread_count_;

	static size_t next_id_;

	struct JobQueueEntry
	{
		ThreadJob* job;
		int priority;
	};

	struct JobQueueCmp
	{
		bool operator()(const JobQueueEntry& a, const JobQueueEntry& b);
	};

	std::priority_queue<JobQueueEntry, std::vector<JobQueueEntry>, JobQueueCmp> jobs_;

	std::vector<Worker*> workers_;
};

template<typename Ret, typename ...Ts>
inline JobResult<Ret>* ThreadManager::schedule_func(std::function<Ret(Ts...)> func, Ts ...Args)
{
	std::unique_lock lock(id_lock_);
	FunctionJob<Ret>* job = new FunctionJob<Ret>(ThreadManager::next_id_++, func, Args...);
	JobResult<Ret>* result = job->get_result();
	return result;
}
