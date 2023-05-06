#pragma once

typedef unsigned int JobID;

enum class ResultStatus
{
	RUNNING,
	FINISHED
};

template <typename Ret>
struct JobResult
{
	JobResult(JobID id);
	void set_value(Ret value);

	ResultStatus status;
	JobID id;
	Ret value;
};

template<typename Ret>
JobResult<Ret>::JobResult(JobID id)
	: status(ResultStatus::RUNNING), value(), id(id)
{}

template<typename Ret>
void JobResult<Ret>::set_value(Ret value)
{
	this->value = value;
	status = ResultStatus::FINISHED;
}
