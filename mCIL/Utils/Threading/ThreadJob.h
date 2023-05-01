#pragma once
#include "../../cil-system.h"

typedef unsigned int JobID;

enum class JobStatus
{
	PENDING,
	RUNNING,
	FINISHED,
	ERROR
};

class ThreadJob
{
public:
	ThreadJob(JobID id);
	void execute();

	JobID id;
	JobStatus status;
private:
	virtual void run_() = 0;
};