#include <iostream>
#include <iomanip>
#include <fstream>
#include "Transform.h"
#include "Utils.h"

#include <chrono>

using namespace std;
using namespace chrono;
using namespace tbb;

/*
	Serial DCT calculation with tbb tasks. 
	Tasks are executed sequentialy, starting with phase 1.
*/
void CalculateDCTransformSerial(Matrix* alpha, Matrix* in, Matrix* c, Matrix* rrr)
{
	size_t matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;
	int row = 0, col = 0;
	
	Matrix r, rr;
	ResizeMatrix(r, matrixDim);
	ResizeMatrix(rr, matrixDim);

	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);

	for (size_t i = 0; i < phase1Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
		}

		phase1Tasks[i] = new(task::allocate_root()) Phase1Task(in, c, &r, row, col);
		phase1Tasks[i]->execute();
		++col;
	}

	row = 0; col = 0;
	for (size_t i = 0; i < phase2Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
		}

		phase2Tasks[i] = new(task::allocate_root()) Phase2Task(&r, c, &rr, row, col, col);
		phase2Tasks[i]->execute();
		++col;
	}

	for (int i = 0; i < phase3Tasks.size(); ++i)
	{
		phase3Tasks[i] = new(task::allocate_root()) Phase3Task(alpha, &rr, rrr, i, 0);
		phase3Tasks[i]->execute();
	}
}

/* Method that creates graph of tasks with buffer tasks. Main function for parallel program. */
void CalculateDCTransformParallel(Matrix* alpha, Matrix* in, Matrix* c, Matrix* rrr)
{
	int matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;

	Matrix r;
	Matrix rr;

	ResizeMatrix(r, matrixDim); // @Slowing down
	ResizeMatrix(rr, matrixDim);

	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);
	vector<Buffer*>     buffers(matrixDim);

	// Create tasks
	empty_task* root = new(task::allocate_root()) empty_task();
	root->set_ref_count(phase3Tasks.size() + 1);

	for (int i = 0; i < phase3Tasks.size(); ++i)
	{
		phase3Tasks[i] = new(task::allocate_root()) Phase3Task(alpha, &rr, rrr, i, 0);
		phase3Tasks[i]->AddSuccessor(root);
		phase3Tasks[i]->set_ref_count(matrixDim);
	}

	// Create phase2 tasks
	int row = 0, col = 0, phase3Suc = 0;
	for (size_t i = 0; i < phase2Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			++phase3Suc;
			col = 0;
		}

		phase2Tasks[i] = new(task::allocate_root()) Phase2Task(&r, c, &rr, row, col, col);
		phase2Tasks[i]->set_ref_count(1);
		phase2Tasks[i]->AddSuccessor(phase3Tasks[phase3Suc]);
		++col;
	}

	// Create phase1 tasks
	int bufferIndex = 0, step = 0;
	row = 0; col = 0;
	
	buffers[bufferIndex] = new(task::allocate_root()) Buffer();
	buffers[bufferIndex]->set_ref_count(matrixDim);
	buffers[bufferIndex]->successors.insert(buffers[bufferIndex]->successors.end(), phase2Tasks.begin(), phase2Tasks.begin() + matrixDim);

	for (size_t i = 0; i < phase1Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			++bufferIndex;
			step += matrixDim;
			col = 0;

			buffers[bufferIndex] = new(task::allocate_root()) Buffer();
			buffers[bufferIndex]->set_ref_count(matrixDim);
			buffers[bufferIndex]->successors.insert(buffers[bufferIndex]->successors.end(), phase2Tasks.begin() + step, phase2Tasks.begin() + step + matrixDim);
		}

		phase1Tasks[i] = new(task::allocate_root()) Phase1Task(in, c, &r, row, col);
		phase1Tasks[i]->AddSuccessor(buffers[bufferIndex]);
		++col;
	}

	// Spawn tasks
	for (size_t i = 0; i < phase1Tasks.size(); ++i)
		root->spawn(*(phase1Tasks[i]));

	root->wait_for_all();
	task::destroy(*root);
}

void CalculateDCTransformParallel2(Matrix* alpha, Matrix*in, Matrix* c, Matrix* rrr)
{
	size_t matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;

	Matrix r;
	Matrix rr;
	ResizeMatrix(r, matrixDim); //@Slowing down ?
	ResizeMatrix(rr, matrixDim);

	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);

	 // Create tasks
	empty_task* root = new(task::allocate_root()) empty_task();
	root->set_ref_count(phase3Tasks.size() + 1);

	for (size_t i = 0; i < phase3Tasks.size(); ++i)
	{
		phase3Tasks[i] = new(root->allocate_child()) Phase3Task(alpha, &rr, rrr, i, 0);
		phase3Tasks[i]->set_ref_count(matrixDim);
	}

	int row = 0, col = 0;
	int phase3Suc = 0;
	for (size_t i = 0; i < phase2Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			++phase3Suc;
			col = 0;
		}

		phase2Tasks[i] = new(phase3Tasks[phase3Suc]->allocate_child()) Phase2Task(&r, c, &rr, row, col, col);
		phase2Tasks[i]->set_ref_count(matrixDim);
		++col;
	}

	row = 0;
	col = 0;
	int phase2Suc = 0;
	for (size_t i = 0; i < phase1Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			phase2Suc += matrixDim;
			col = 0;
		}

		phase1Tasks[i] = new(task::allocate_root()) Phase1Task(in, c, &r, row, col);
		
		for (int j = 0; j < matrixDim; ++j)
			phase1Tasks[i]->AddSuccessor(phase2Tasks[phase2Suc + j]);

		++col;
	}

	// Spawn tasks
	for (size_t i = 0; i < phase1Tasks.size() - 1; ++i)
		root->spawn(*(phase1Tasks[i]));

	root->spawn_and_wait_for_all(*(phase1Tasks[phase1Tasks.size() -1]));
	task::destroy(*root);
}

void CalculateDCTransformParallel3(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	int matrixDim = in->size();
	int numOfP1P2tasks = matrixDim * matrixDim;
	int row = 0, col = 0, phase3Suc = 0, bufferIndex = 0, step = 0;

	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);
	vector<Buffer*>     buffers(matrixDim);

	// Create tasks
	empty_task* root = new(task::allocate_root()) empty_task();
	root->set_ref_count(matrixDim + 1);

	for (int i = 0; i < matrixDim; ++i)
	{
		phase3Tasks[i] = new(root->allocate_child()) Phase3Task(alpha, rr, rrr, i, 0);
		phase3Tasks[i]->set_ref_count(matrixDim);
	}

	// Create phase2 tasks
	
	for (int i = 0; i < numOfP1P2tasks; ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			++phase3Suc;
			++bufferIndex;
			step += matrixDim;
			col = 0;
		}

		phase2Tasks[i] = new(phase3Tasks[phase3Suc]->allocate_child()) Phase2Task(r, c, rr, row, col, col);
		phase2Tasks[i]->set_ref_count(1);
		
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

	// Create phase1 tasks
	bufferIndex = 0; 
	row = 0; col = 0;

	for (int i = 0; i < numOfP1P2tasks; ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			++bufferIndex;
			col = 0;
		}

		phase1Tasks[i] = new(buffers[bufferIndex]->allocate_child()) Phase1Task(in, c, r, row, col);
		++col;
	}

	// Spawn tasks
	for (int i = 0; i < numOfP1P2tasks - 1; ++i)
		root->spawn(*(phase1Tasks[i]));

	root->spawn_and_wait_for_all(*(phase1Tasks[numOfP1P2tasks - 1]));
	task::destroy(*root);
}
///////// Phase

Phase::Phase(int row_, int col_)
	: row(row_), col(col_)
{
}

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

void Phase1Task::DoWork()
{
	int suma = 0;

	for (size_t i = 0; i < matrixIn->size(); ++i)
		suma += (*matrixIn)[row][i] * (*matrixC)[i][col];

	(*matrixR)[row][col] = suma;
}

tbb::task* Phase1Task::execute()
{
	// Calculating sum
	DoWork();

	/*for (size_t i = 0; i < successors.size(); ++i)
	{
		if (task* t = successors[i])
		{
			if (t->decrement_ref_count() == 0)
				spawn(*t); 
		}
	}
*/
	return NULL;
}

///////// Phase2Task

Phase2Task::Phase2Task(Matrix* r, Matrix* c, Matrix* rr, int row_, int col_, int row2_)
	: matrixR(r), matrixC(c), matrixRR(rr), Phase(row_, col_), row2(row2_)
{
}

Phase2Task::~Phase2Task()
{
}

void Phase2Task::DoWork()
{
	// Calculating sum
	int suma = 0;

	for (size_t i = 0; i < matrixR->size(); ++i)
		suma += (*matrixR)[row][i] * (*matrixC)[row2][i];

	(*matrixRR)[row][col] = suma;
}

tbb::task* Phase2Task::execute()
{
	DoWork();

	/*for (size_t i = 0; i < successors.size(); ++i)
	{
		if (task* t = successors[i])
		{
			if (t->decrement_ref_count() == 0)
				spawn(*t);
		}
	}*/

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

void Phase3Task::DoWork()
{
	// Calculating sum
	int suma = 0;

	for (size_t i = 0; i < matrixRR->size(); ++i)
		(*matrixRRR)[row][i] = (*matrixAlpha)[row][i] * (*matrixRR)[row][i];
}

tbb::task* Phase3Task::execute()
{
	DoWork();

	/*for (size_t i = 0; i < successors.size(); ++i)
	{
		if (task* t = successors[i])
		{
			if (t->decrement_ref_count() == 0)
				spawn(*t);
		}
	}*/

	return NULL;
}
