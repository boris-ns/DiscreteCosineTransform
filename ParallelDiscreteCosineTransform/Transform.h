#pragma once

#include <vector>
#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"

typedef std::vector<std::vector<float>> Matrix;


void InitMatrix(Matrix& matrix, size_t size);
void PrintMatrix(const Matrix& matrix);
void CalculateDCTransformSerial(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr);
void CalculateDCTransformParallel(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr);


/* Abstract class that represents basic tbb task with successors. */
class Phase : public tbb::task
{
public:
	Phase(int row_, int col_);
	virtual tbb::task* execute() = 0;
	void AddSuccessor(tbb::task* t);

protected:
	std::vector<task*> successors;
	int row, col;
};


class Phase1Task : public Phase
{
public:
	Phase1Task(Matrix* in, Matrix* c, Matrix* r, int row_, int col_);
	~Phase1Task();

	tbb::task* execute();

	Matrix* matrixIn;
	Matrix* matrixC;
	Matrix* matrixR;
};

class Phase2Task : public Phase
{
public:
	Phase2Task(Matrix* r, Matrix* c, Matrix* rr, int row_, int col_, int row2_);
	~Phase2Task();

	tbb::task* execute();

	int row2;

	Matrix* matrixR;
	Matrix* matrixC;
	Matrix* matrixRR;
};

class Phase3Task : public Phase
{
public:
	Phase3Task(Matrix* alp, Matrix* rr, Matrix* rrr, int row_, int col_);
	~Phase3Task();

	tbb::task* execute();

	Matrix* matrixAlpha;
	Matrix* matrixRR;
	Matrix* matrixRRR;
};