#include "Transform.h"
#include <iostream>
#include "Utils.h"

using namespace std;
using namespace tbb;

/*
	Serial DCT calculation with tbb tasks. 
	Tasks are executed sequentialy, starting with phase 1.
*/
void CalculateDCTransformSerial(Matrix* alpha, Matrix* in, 
								Matrix* c, Matrix* rrr)
{
	size_t matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;
	int row = 0, col = 0;
	
	// Init mid-result matrices
	Matrix r, rr;
	ResizeMatrix(r, matrixDim);
	ResizeMatrix(rr, matrixDim);

	// Vectors of tasks for each phase
	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);

	// Create and execute phase 1 tasks
	for (size_t i = 0; i < phase1Tasks.size(); ++i)
	{
		// Every matrixDim (one calculated row)
		// change to the new row, and start cols at 0
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
		}

		phase1Tasks[i] = new(task::allocate_root()) 
							Phase1Task(in, c, &r, row, col);
		phase1Tasks[i]->execute();
		++col;
	}

	// Create and execute phase 2 tasks
	row = 0; col = 0;
	for (size_t i = 0; i < phase2Tasks.size(); ++i)
	{
		// Every matrixDim change row
		// In this case col represents row of second matrix
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
		}

		phase2Tasks[i] = new(task::allocate_root()) 
							Phase2Task(&r, c, &rr, row, col, col);
		phase2Tasks[i]->execute();
		++col;
	}

	// Create and execute phase 3 tasks
	for (int i = 0; i < phase3Tasks.size(); ++i)
	{
		phase3Tasks[i] = new(task::allocate_root()) 
							Phase3Task(alpha, &rr, rrr, i, 0);
		phase3Tasks[i]->execute();
	}
}

/* Parallel implementation of DCT. rrr is final result. */
void CalculateDCTransformParallel(Matrix* alpha, Matrix* in, Matrix* c, 
									Matrix* r, Matrix* rr, Matrix* rrr)
{
	int matrixDim = in->size();
	int numOfP1P2tasks = matrixDim * matrixDim;
	int row = 0, col = 0, phase3Suc = 0, bufferIndex = 0;

	// Vectors of tasks for each phase
	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);
	vector<Buffer*>     buffers(matrixDim);

	// Create root task
	empty_task* root = new(task::allocate_root()) empty_task();
	root->set_ref_count(matrixDim + 1);

	// Create phase 3 tasks as childs of root task
	for (int i = 0; i < matrixDim; ++i)
	{
		phase3Tasks[i] = new(root->allocate_child()) 
							Phase3Task(alpha, rr, rrr, i, 0);
		phase3Tasks[i]->set_ref_count(matrixDim);
	}

	// Create phase2 tasks as childs of phase 3 tasks
	// Create buffer tasks as childs of phase 2 tasks
	for (int i = 0; i < numOfP1P2tasks; ++i)
	{
		// Counters that change every matrixDim
		if (i != 0 && i % matrixDim == 0)
		{
			++row;              // Keeps track of which row needs to be calculated
			++phase3Suc;        // Index of next succesor for phase 2
			++bufferIndex;      // Index of current buffer
			col = 0;            // In this case col represents row of second matrix
		}

		phase2Tasks[i] = new(phase3Tasks[phase3Suc]->allocate_child())
								Phase2Task(r, c, rr, row, col, col);
		phase2Tasks[i]->set_ref_count(1);
		
		// If buffer isn't created, create new one, otherwise just add successor.
		if (buffers[bufferIndex] == NULL)
		{
			buffers[bufferIndex] = new(phase2Tasks[i]->allocate_child()) Buffer();
			buffers[bufferIndex]->set_ref_count(matrixDim);
		}
		else
		{
			buffers[bufferIndex]->AddSuccessor(phase2Tasks[i]);
		}

		++col;
	}

	// Create phase1 tasks as childs of buffer tasks
	bufferIndex = 0;  // Index of current buffer
	row = 0; col = 0; // Counters for row and col that need to be calculated

	for (int i = 0; i < numOfP1P2tasks; ++i)
	{
		// Every matrixDim change to the nex row and buffer
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			++bufferIndex;
			col = 0;
		}

		phase1Tasks[i] = new(buffers[bufferIndex]->allocate_child()) 
							Phase1Task(in, c, r, row, col);
		++col;
	}

	// Spawn phase 1 tasks, except last one
	for (int i = 0; i < numOfP1P2tasks - 1; ++i)
		root->spawn(*(phase1Tasks[i]));

	// Spawn and wait for the last phase1 task
	root->spawn_and_wait_for_all(*(phase1Tasks[numOfP1P2tasks - 1]));
	task::destroy(*root);
}

///////// Phase

Phase::Phase(int row_, int col_)
	: row(row_), col(col_)
{
}

Phase::~Phase()
{
}

void Phase::DoWork()
{
}

/* Add new successor to the list. */
void Phase::AddSuccessor(task* t)
{
	successors.push_back(t);
}

///////// Buffer

Buffer::Buffer()
	: Phase(0, 0)
{
}

Buffer::~Buffer()
{
}

/* Spawns successor tasks. */
tbb::task* Buffer::execute()
{
	for (size_t i = 0; i < successors.size(); ++i)
	{
		if (task* t = successors[i])
		{
			if (t->decrement_ref_count() == 0)
				spawn(*t);
		}
	}	

	return NULL;
}

///////// Phase1Task

Phase1Task::Phase1Task(Matrix* in, Matrix* c, Matrix* r, int row_, int col_)
	: matrixIn(in), matrixC(c), matrixR(r), Phase(row_, col_)
{
}

Phase1Task::~Phase1Task()
{
}

/* Multiplies row and col, sums it, and stores in result matrix. */
void Phase1Task::DoWork()
{
	long long suma = 0;

	for (size_t i = 0; i < matrixIn->size(); ++i)
		suma += (*matrixIn)[row][i] * (*matrixC)[i][col];

	(*matrixR)[row][col] = suma;
}

tbb::task* Phase1Task::execute()
{
	DoWork();

	return NULL;
}

///////// Phase2Task

Phase2Task::Phase2Task(Matrix* r, Matrix* c, Matrix* rr, int row_, 
						int col_, int row2_)
	: matrixR(r), matrixC(c), matrixRR(rr), Phase(row_, col_), row2(row2_)
{
}

Phase2Task::~Phase2Task()
{
}

/* Multiplies row1 and row2, sums it, and stores in result matrix. */
void Phase2Task::DoWork()
{
	long long suma = 0;

	for (size_t i = 0; i < matrixR->size(); ++i)
		suma += (*matrixR)[row][i] * (*matrixC)[row2][i];

	(*matrixRR)[row][col] = suma;
}

tbb::task* Phase2Task::execute()
{
	DoWork();

	return NULL;
}

///////// Phase3Task

Phase3Task::Phase3Task(Matrix* alp, Matrix* rr, Matrix* rrr, int row_, int col_)
	: matrixAlpha(alp), matrixRR(rr), matrixRRR(rrr), Phase(row_, col_)
{
}

Phase3Task::~Phase3Task()
{
}

/* Multiplies corresponding elements of passed row. */
void Phase3Task::DoWork()
{
	long long suma = 0;

	for (size_t i = 0; i < matrixRR->size(); ++i)
		(*matrixRRR)[row][i] = (*matrixAlpha)[row][i] * (*matrixRR)[row][i];
}

tbb::task* Phase3Task::execute()
{
	DoWork();

	return NULL;
}
