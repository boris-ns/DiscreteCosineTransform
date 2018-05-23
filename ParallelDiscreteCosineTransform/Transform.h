#pragma once

#include <vector>
#include "tbb/task.h"

typedef std::vector<std::vector<long long>> Matrix;

/* Serial DCT calculation using tbb tasks. rrr is final result.*/
void CalculateDCTransformSerial(Matrix* alpha, Matrix* in, Matrix* c, Matrix* rrr);

/* Parallel implementation of DCT. rrr is final result. */
void CalculateDCTransformParallel(Matrix* alpha, Matrix* in, Matrix* c, Matrix* rrr);

/* Parallel implementation of DCT w/out buffers. rrr is final result. */
void CalculateDCTransformParallel2(Matrix* alpha, Matrix*in, Matrix* c, Matrix* rrr);

void CalculateDCTransformParallel3(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr);


/* 
	Abstract class that represents basic tbb task with successors. 
*/
class Phase : public tbb::task
{
public:
	Phase(int row_, int col_);
	virtual tbb::task* execute() = 0;
	virtual void DoWork() {};
	void AddSuccessor(tbb::task* t);

	std::vector<task*> successors;

protected:
	int row, col;
};

/* 
	Buffer class that is used as buffer (holds successors) between phases 1 and 2. 
*/
class Buffer : public Phase
{
public:
	Buffer();
	~Buffer();

	tbb::task* execute();
};

/* 
	Phase1 - regular matrix multiplication (row x col). 
*/
class Phase1Task : public Phase
{
public:
	Phase1Task(Matrix* in, Matrix* c, Matrix* r, int row_, int col_);
	~Phase1Task();

	void DoWork();
	tbb::task* execute();

	Matrix* matrixIn;
	Matrix* matrixC;
	Matrix* matrixR;
};

/* 
	Phase2 - matrix multiplication (row x row). 
*/
class Phase2Task : public Phase
{
public:
	Phase2Task(Matrix* r, Matrix* c, Matrix* rr, int row_, int col_, int row2_);
	~Phase2Task();

	void DoWork();
	tbb::task* execute();

	int row2;

	Matrix* matrixR;
	Matrix* matrixC;
	Matrix* matrixRR;
};

/* 
	Phase3 - Scalar matrix multiplication. Multiplies elements of 2 rows.
	col is unused
*/
class Phase3Task : public Phase
{
public:
	Phase3Task(Matrix* alp, Matrix* rr, Matrix* rrr, int row_, int col_);
	~Phase3Task();

	void DoWork();
	tbb::task* execute();

	Matrix* matrixAlpha;
	Matrix* matrixRR;
	Matrix* matrixRRR;
};