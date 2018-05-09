#include <iostream>
#include "Transform.h"

using namespace std;
using namespace tbb;

void CalculateDCTransform()
{
	empty_task* root = new(task::allocate_root()) empty_task();



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
