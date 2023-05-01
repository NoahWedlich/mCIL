#pragma once
#include "../../cil-system.h"
#include "ThreadJob.h"
#include "JobResult.h"

template <typename Ret>
class FunctionJob : public ThreadJob
{
public:
	template <typename ...Ts>
	FunctionJob(JobID id, std::function<Ret(Ts...)>, Ts... args);

	const JobResult<Ret>* get_result() const;
private:
	virtual void run_() override;

	std::function<Ret()> func_;
	JobResult<Ret>* result_;
};

template <typename Ret>
template <typename... Ts>
FunctionJob<Ret>::FunctionJob(JobID id, std::function<Ret(Ts...)> func, Ts... args)
	: ThreadJob(id)
{
	func_ = std::bind(func, args...);
	result_ = new JobResult<Ret>();
}

template <typename Ret>
const JobResult<Ret>* FunctionJob<Ret>::get_result() const
{ return result_; }

template <typename Ret>
void FunctionJob<Ret>::run_()
{
	Ret val = func_();
	result_->set_value(val);
}