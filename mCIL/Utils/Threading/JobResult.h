#pragma once

enum class ResultStatus
{
	RUNNING,
	FINISHED
};

template <typename Ret>
struct JobResult
{
	JobResult();
	void set_value(Ret value);

	ResultStatus status;
	Ret value;
};

template<typename Ret>
JobResult<Ret>::JobResult()
	: status(ResultStatus::RUNNING), value()
{}

template<typename Ret>
void JobResult<Ret>::set_value(Ret value)
{
	this->value = value;
	status = ResultStatus::FINISHED;
}
