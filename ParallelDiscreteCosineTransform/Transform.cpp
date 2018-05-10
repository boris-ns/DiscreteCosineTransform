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
	Phase1Task* p1t1 = new(task::allocate_root()) Phase1Task(in, c, r, 0, 0);
	p1t1->execute();
	Phase1Task* p1t2 = new(task::allocate_root()) Phase1Task(in, c, r, 0, 1);
	p1t2->execute();
	Phase1Task* p1t3 = new(task::allocate_root()) Phase1Task(in, c, r, 1, 0);
	p1t3->execute();
	Phase1Task* p1t4 = new(task::allocate_root()) Phase1Task(in, c, r, 1, 1);
	p1t4->execute();

	cout << "Matrix R:" << endl;
	PrintMatrix(*r);
	cout << endl;

	Phase2Task* p2t1 = new(task::allocate_root()) Phase2Task(r, c, rr, 0, 0, 0);
	p2t1->execute();
	Phase2Task* p2t2 = new(task::allocate_root()) Phase2Task(r, c, rr, 0, 1, 1);
	p2t2->execute();
	Phase2Task* p2t3 = new(task::allocate_root()) Phase2Task(r, c, rr, 1, 0, 0);
	p2t3->execute();
	Phase2Task* p2t4 = new(task::allocate_root()) Phase2Task(r, c, rr, 1, 1, 1);
	p2t4->execute();

	cout << "Matrix RR:" << endl;
	PrintMatrix(*rr);
	cout << endl;

	Phase3Task* p3t1 = new(task::allocate_root()) Phase3Task(alpha, rr, rrr, 0, 0);
	p3t1->execute();
	Phase3Task* p3t2 = new(task::allocate_root()) Phase3Task(alpha, rr, rrr, 1, 0);
	p3t2->execute();

	cout << "Matrix RRR:" << endl;
	PrintMatrix(*rrr);
	cout << endl;
}

/* Method that creates graph of tasks. Main function for parallel program. */
void CalculateDCTransformParallel(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	// @REFACTOR: so it wont be only for 2x2 matrices

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

Phase::Phase(int row_, int col_)
	: row(row_), col(col_)
{
}

void Phase::AddSuccessor(task* t)
{
	successors.push_back(t);
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
