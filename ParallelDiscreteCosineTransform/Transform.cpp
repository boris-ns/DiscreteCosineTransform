#include <iostream>
#include <iomanip>
#include <fstream>
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

/* Writes passed matrix to the file. */
void WriteMatrixToFile(const Matrix& matrix, const string& path)
{
	ofstream outFile(path);

	for (size_t i = 0; i < matrix.size(); ++i)
	{
		for (size_t j = 0; j < matrix[i].size(); ++j)
			outFile << fixed << setprecision(1) << matrix[i][j] << " ";

		outFile << endl;
	}

	outFile.close();
}

/* Functions that fills up matrix with random values. */
void CreateMatrixWithRandomElements(Matrix& mat)
{
	for (int i = 0; i < mat.size(); ++i)
	{
		for (int j = 0; j < mat[i].size(); ++j)
			mat.at(i).at(j) = (float)(rand() % 100);
	}
}

/* Serial method just for testing tasks. */
void CalculateDCTransformSerial(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	size_t matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;
	int row = 0, col = 0;

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

		phase1Tasks[i] = new(task::allocate_root()) Phase1Task(in, c, r, row, col);
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

/* Method that creates graph of tasks with buffer tasks. Main function for parallel program. */
void CalculateDCTransformParallel(Matrix* alpha, Matrix* in, Matrix* c, Matrix* r, Matrix* rr, Matrix* rrr)
{
	size_t matrixDim = in->size();
	size_t numOfP1P2tasks = matrixDim * matrixDim;

	vector<Phase1Task*> phase1Tasks(numOfP1P2tasks);
	vector<Phase2Task*> phase2Tasks(numOfP1P2tasks);
	vector<Phase3Task*> phase3Tasks(matrixDim);
	vector<Buffer*>     buffers(matrixDim);

	// Create tasks
	empty_task* root = new(task::allocate_root()) empty_task();
	root->set_ref_count(phase3Tasks.size() + 1);

	for (size_t i = 0; i < phase3Tasks.size(); ++i)
	{
		phase3Tasks[i] = new(task::allocate_root()) Phase3Task(alpha, rr, rrr, i, 0);
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

		phase2Tasks[i] = new(task::allocate_root()) Phase2Task(r, c, rr, row, col, col);
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

		phase1Tasks[i] = new(task::allocate_root()) Phase1Task(in, c, r, row, col);
		phase1Tasks[i]->AddSuccessor(buffers[bufferIndex]);
		++col;
	}

	// Spawn tasks
	for (size_t i = 0; i < phase1Tasks.size(); ++i)
		root->spawn(*(phase1Tasks[i]));

	root->wait_for_all();
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
