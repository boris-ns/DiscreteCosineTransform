#include <iostream>
#include "Transform.h"

using namespace std;
using namespace tbb;

/* Resizes passed matrix to dimensions size*size. */
void InitMatrix(Matrix& matrix, size_t size)
{
	matrix.resize(size);

	for (size_t i = 0; i < matrix.size(); ++i)
		matrix[i].resize(size);
}

/* Prints passed matrix to the console. */
void PrintMatrix(const Matrix& matrix)
{
	for (size_t i = 0; i < matrix.size(); ++i)
	{
		for (size_t j = 0; j < matrix[i].size(); ++j)
			cout << matrix[i][j] << " ";

		cout << endl;
	}
}

/* Serial method just for testing tasks. */
void CalculateDCTransformSerial(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	size_t matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;
	size_t numOfP3tasks   = matrixDim;
	int row = 0, col = 0;

	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(numOfP3tasks);

	for (size_t i = 0; i < phase1Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
		}

		phase1Tasks[i] = new(task::allocate_root()) Phase1Task(in, c, r, row, col);
		phase1Tasks[i]->execute();
	
		++col;
	}
	
	row = 0;
	col = 0;
	for (size_t i = 0; i < phase2Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
		}

		phase2Tasks[i] = new(task::allocate_root()) Phase2Task(r, c, rr, row, col, col);
		phase2Tasks[i]->execute();

		++col;
	}

	for (size_t i = 0; i < phase3Tasks.size(); ++i)
	{
		phase3Tasks[i] = new(task::allocate_root()) Phase3Task(alpha, rr, rrr, i, 0);
		phase3Tasks[i]->execute();
	}
}

/* Method that creates graph of tasks. Main function for parallel program. */
/* @WARNING: This method only works for 2x2 matrices. */
void CalculateDCTransformParallel(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	// Create tasks
	empty_task* root = new(task::allocate_root()) empty_task();

	Phase3Task* p3t1 = new(task::allocate_root()) Phase3Task(alpha, rr, rrr, 0, 0); 
	p3t1->AddSuccessor(root);
	Phase3Task* p3t2 = new(task::allocate_root()) Phase3Task(alpha, rr, rrr, 1, 0);
	p3t2->AddSuccessor(root);

	Phase2Task* p2t1 = new(task::allocate_root()) Phase2Task(r, c, rr, 0, 0, 0);
	p2t1->AddSuccessor(p3t1);
	Phase2Task* p2t2 = new(task::allocate_root()) Phase2Task(r, c, rr, 0, 1, 1);
	p2t2->AddSuccessor(p3t1);
	Phase2Task* p2t3 = new(task::allocate_root()) Phase2Task(r, c, rr, 1, 0, 0);
	p2t3->AddSuccessor(p3t2);
	Phase2Task* p2t4 = new(task::allocate_root()) Phase2Task(r, c, rr, 1, 1, 1);
	p2t4->AddSuccessor(p3t2);

	Phase1Task* p1t1 = new(task::allocate_root()) Phase1Task(in, c, r, 0, 0);
	p1t1->AddSuccessor(p2t1);
	p1t1->AddSuccessor(p2t2);
	Phase1Task* p1t2 = new(task::allocate_root()) Phase1Task(in, c, r, 0, 1);
	p1t2->AddSuccessor(p2t1);
	p1t1->AddSuccessor(p2t2);
	Phase1Task* p1t3 = new(task::allocate_root()) Phase1Task(in, c, r, 1, 0);
	p1t3->AddSuccessor(p2t3);
	p1t1->AddSuccessor(p2t4);
	Phase1Task* p1t4 = new(task::allocate_root()) Phase1Task(in, c, r, 1, 1);
	p1t4->AddSuccessor(p2t3);
	p1t1->AddSuccessor(p2t4);

	// Set ref counts
	root->set_ref_count(3);
	
	p3t1->set_ref_count(2);
	p3t2->set_ref_count(2);

	p2t1->set_ref_count(2);
	p2t2->set_ref_count(2);
	p2t3->set_ref_count(2);
	p2t4->set_ref_count(2);

	// Spawn tasks
	root->spawn(*p1t1);
	root->spawn(*p1t2);
	root->spawn(*p1t3);
	root->spawn_and_wait_for_all(*p1t4);

	task::destroy(*root);
}

/* Method that creates graph of tasks. Main function for parallel program. */
void CalculateDCTransformParallel2(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	size_t matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;

	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);

	// Create tasks
	empty_task* root = new(task::allocate_root()) empty_task();
	root->set_ref_count(phase3Tasks.size() + 1);

	for (size_t i = 0; i < phase3Tasks.size(); ++i)
	{
		phase3Tasks[i] = new(task::allocate_root()) Phase3Task(alpha, rr, rrr, i, 0);
		phase3Tasks[i]->AddSuccessor(root);
		phase3Tasks[i]->set_ref_count(matrixDim);
	}

	int row = 0, col = 0;
	int phase3Suc = 0;
	for (size_t i = 0; i < phase2Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
		}

		phase2Tasks[i] = new(task::allocate_root()) Phase2Task(r, c, rr, row, col, col);
		phase2Tasks[i]->set_ref_count(matrixDim);

		if (i != 0 && i % matrixDim == 0)
			++phase3Suc;

		phase2Tasks[i]->AddSuccessor(phase3Tasks[phase3Suc]);

		++col;
	}

	row = 0;
	col = 0;
	int step = 0;

	for (size_t i = 0; i < phase1Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
			step += matrixDim;
		}

		phase1Tasks[i] = new(task::allocate_root()) Phase1Task(in, c, r, row, col);

		for (int j = 0; j < matrixDim; ++j)
			phase1Tasks[i]->AddSuccessor(phase2Tasks[j + step]);

		++col;
	}

	/*for (size_t i = 0; i < phase2Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			step += matrixDim;
		}

		for (int j = 0; j < matrixDim; ++j)
			phase1Tasks[j + step]->AddSuccessor(phase2Tasks[i]);
	}*/
	
	// Spawn tasks
	for (size_t i = 0; i < phase1Tasks.size(); ++i)
		root->spawn(*(phase1Tasks[i]));

	root->wait_for_all();
	
	task::destroy(*root);
}

/* Method that creates graph of tasks with buffer tasks. Main function for parallel program. */
void CalculateDCTransformParallel3(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	size_t matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;

	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);
	vector<Buffer*>     buffersP1P2(matrixDim);
	vector<Buffer*>     buffersP2P3(matrixDim);

	// Create tasks
	empty_task* root = new(task::allocate_root()) empty_task();
	root->set_ref_count(phase3Tasks.size() + 1);

	for (size_t i = 0; i < phase3Tasks.size(); ++i)
	{
		phase3Tasks[i] = new(task::allocate_root()) Phase3Task(alpha, rr, rrr, i, 0);
		phase3Tasks[i]->AddSuccessor(root);
		phase3Tasks[i]->set_ref_count(1);
	}

	int row = 0, col = 0;
	int phase3Suc = 0;
	int bufferIndex = 0;

	buffersP2P3[bufferIndex] = new(task::allocate_root()) Buffer();
	buffersP2P3[bufferIndex]->set_ref_count(matrixDim);
	buffersP2P3[bufferIndex]->AddSuccessor(phase3Tasks[phase3Suc]);

	for (size_t i = 0; i < phase2Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
			++bufferIndex;
			++phase3Suc;
			buffersP2P3[bufferIndex] = new(task::allocate_root()) Buffer();
			buffersP2P3[bufferIndex]->set_ref_count(matrixDim);
			buffersP2P3[bufferIndex]->AddSuccessor(phase3Tasks[phase3Suc]);
		}

		phase2Tasks[i] = new(task::allocate_root()) Phase2Task(r, c, rr, row, col, col);
		phase2Tasks[i]->set_ref_count(1);
		phase2Tasks[i]->AddSuccessor(buffersP2P3[bufferIndex]);

		++col;
	}

	row = 0;
	col = 0;
	bufferIndex = 0;
	int step = 0;
	
	buffersP1P2[bufferIndex] = new(task::allocate_root()) Buffer();
	buffersP1P2[bufferIndex]->set_ref_count(matrixDim);
	buffersP1P2[bufferIndex]->successors.insert(buffersP1P2[bufferIndex]->successors.end(), phase2Tasks.begin(), phase2Tasks.begin() + matrixDim);

	for (size_t i = 0; i < phase1Tasks.size(); ++i)
	{
		if (i != 0 && i % matrixDim == 0)
		{
			++row;
			col = 0;
			++bufferIndex;
			step += matrixDim;
			buffersP1P2[bufferIndex] = new(task::allocate_root()) Buffer();
			buffersP1P2[bufferIndex]->set_ref_count(matrixDim);
			buffersP1P2[bufferIndex]->successors.insert(buffersP1P2[bufferIndex]->successors.end(), phase2Tasks.begin() + step, phase2Tasks.begin() + step + matrixDim);
		}

		phase1Tasks[i] = new(task::allocate_root()) Phase1Task(in, c, r, row, col);
		phase1Tasks[i]->AddSuccessor(buffersP1P2[bufferIndex]);
		
		++col;
	}

	// Spawn tasks
	for (size_t i = 0; i < phase1Tasks.size(); ++i)
		root->spawn(*(phase1Tasks[i]));

	root->wait_for_all();

	task::destroy(*root);
}

Phase::Phase(int row_, int col_)
	: row(row_), col(col_)
{
}

void Phase::AddSuccessor(task* t)
{
	successors.push_back(t);
}

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

tbb::task* Phase1Task::execute()
{
	// Calculating sum
	float suma = 0.0f;
	
	for (size_t i = 0; i < matrixIn->size(); ++i)
		suma += (*matrixIn)[row][i] * (*matrixC)[i][col];

	(*matrixR)[row][col] = suma;

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

///////// Phase2Task

Phase2Task::Phase2Task(Matrix* r, Matrix* c, Matrix* rr, int row_, int col_, int row2_)
	: matrixR(r), matrixC(c), matrixRR(rr), Phase(row_, col_), row2(row2_)
{
}

Phase2Task::~Phase2Task()
{
}

tbb::task* Phase2Task::execute()
{
	// Calculating sum
	float suma = 0.0f;

	for (size_t i = 0; i < matrixR->size(); ++i)
		suma += (*matrixR)[row][i] * (*matrixC)[row2][i];

	(*matrixRR)[row][col] = suma;

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

///////// Phase3Task

Phase3Task::Phase3Task(Matrix* alp, Matrix* rr, Matrix* rrr, int row_, int col_)
	: matrixAlpha(alp), matrixRR(rr), matrixRRR(rrr), Phase(row_, col_)
{
}

Phase3Task::~Phase3Task()
{
}

tbb::task* Phase3Task::execute()
{
	// Calculating sum
	float suma = 0.0f;

	for (size_t i = 0; i < matrixRR->size(); ++i)
		(*matrixRRR)[row][i] = (*matrixAlpha)[row][i] * (*matrixRR)[row][i];

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
