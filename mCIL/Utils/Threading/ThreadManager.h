#pragma once
#include "../../cil-system.h"
#include "JobResult.h"
#include "ThreadJob.h"
#include "FunctionJob.h"
#include "Worker.h"
#include "JobQueue.h"

class ThreadManager
{
public:
	ThreadManager(size_t thread_count);
	~ThreadManager();

	template <typename Ret, typename... Ts>
	const JobResult<Ret>* schedule_func(std::function<Ret(Ts...)>, Ts... Args);

	void wait(JobID id);
	void wait_all();

	void resize(size_t thread_count);
private:
	std::mutex lock_;
	static std::mutex static_lock_;

	static size_t total_thread_count_;
	size_t local_thread_count_;

	static JobID next_id_;

	JobQueue jobs_;

	std::vector<Worker*> workers_;
};

template<typename Ret, typename ...Ts>
const JobResult<Ret>* ThreadManager::schedule_func(std::function<Ret(Ts...)> func, Ts ...Args)
{
	std::unique_lock slock{ static_lock_ };
	std::unique_lock lock{ lock_ };

	FunctionJob<Ret>* job = new FunctionJob<Ret>(ThreadManager::next_id_++, func, Args...);
	const JobResult<Ret>* result = job->get_result();

	jobs_.push_job(job);

	return result;
}
