#pragma once
#include "../../cil-system.h"

typedef unsigned int JobID;
class Worker;

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
	Worker* worker;
private:
	virtual void run_() = 0;
};