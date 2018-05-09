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

void CalculateDCTransform(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	// @REFACTOR: so it wont be only for 2x2 matrices

	// Create tasks
	empty_task* root = new(task::allocate_root()) empty_task();

	Phase3Task* p3t1 = new(task::allocate_root()) Phase3Task(alpha, rr, rrr); 
	p3t1->AddSuccessor(root);
	Phase3Task* p3t2 = new(task::allocate_root()) Phase3Task(alpha, rr, rrr);
	p3t2->AddSuccessor(root);

	Phase2Task* p2t1 = new(task::allocate_root()) Phase2Task(r, c, rr);
	p2t1->AddSuccessor(p3t1);
	Phase2Task* p2t2 = new(task::allocate_root()) Phase2Task(r, c, rr);
	p2t2->AddSuccessor(p3t1);
	Phase2Task* p2t3 = new(task::allocate_root()) Phase2Task(r, c, rr);
	p2t3->AddSuccessor(p3t2);
	Phase2Task* p2t4 = new(task::allocate_root()) Phase2Task(r, c, rr);
	p2t4->AddSuccessor(p3t2);

	Phase1Task* p1t1 = new(task::allocate_root()) Phase1Task(in, c, r);
	p1t1->AddSuccessor(p2t1);
	p1t1->AddSuccessor(p2t2);
	Phase1Task* p1t2 = new(task::allocate_root()) Phase1Task(in, c, r);
	p1t2->AddSuccessor(p2t1);
	p1t1->AddSuccessor(p2t2);
	Phase1Task* p1t3 = new(task::allocate_root()) Phase1Task(in, c, r);
	p1t3->AddSuccessor(p2t3);
	p1t1->AddSuccessor(p2t4);
	Phase1Task* p1t4 = new(task::allocate_root()) Phase1Task(in, c, r);
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

void Phase::AddSuccessor(task* t)
{
	successors.push_back(t);
}

///////// Phase1Task

Phase1Task::Phase1Task(Matrix* in, Matrix* c, Matrix* r)
	: matrixIn(in), matrixC(c), matrixR(r)
{
}

Phase1Task::~Phase1Task()
{
}

tbb::task* Phase1Task::execute()
{
	cout << "Phase 1 task." << endl;

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

Phase2Task::Phase2Task(Matrix* r, Matrix* c, Matrix* rr)
	: matrixR(r), matrixC(c), matrixRR(rr)
{
}

Phase2Task::~Phase2Task()
{
}

tbb::task* Phase2Task::execute()
{
	cout << "Phase 2 task." << endl;

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

Phase3Task::Phase3Task(Matrix* alp, Matrix* rr, Matrix* rrr)
	: matrixAlpha(alp), matrixRR(rr), matrixRRR(rrr)
{
}

Phase3Task::~Phase3Task()
{
}

tbb::task* Phase3Task::execute()
{
	cout << "Phase 3 task." << endl;

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
