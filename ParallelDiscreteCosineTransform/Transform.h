#pragma once

#include <vector>
#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"

typedef std::vector<std::vector<float>> Matrix;

void CalculateDCTransform();

/* Abstract class that represents basic tbb task with successors. */
class Phase : public tbb::task
{
public:
	virtual tbb::task* execute() = 0;
	void AddSuccessor(tbb::task* t);

protected:
	std::vector<task*> successors;
};


class Phase1Task : public Phase
{
public:
	Phase1Task(Matrix* in, Matrix* c, Matrix* r);
	~Phase1Task();

	tbb::task* execute();

private:
	Matrix* matrixIn;
	Matrix* matrixC;
	Matrix* matrixR;
};

class Phase2Task : public Phase
{
public:
	Phase2Task(Matrix* r, Matrix* c, Matrix* rr);
	~Phase2Task();

	tbb::task* execute();

private:
	Matrix* matrixR;
	Matrix* matrixC;
	Matrix* matrixRR;
};

class Phase3Task : public Phase
{
public:
	Phase3Task(Matrix* alp, Matrix* rr, Matrix* rrr);
	~Phase3Task();

	tbb::task* execute();

private:
	Matrix* matrixAlpha;
	Matrix* matrixRR;
	Matrix* matrixRRR;
};