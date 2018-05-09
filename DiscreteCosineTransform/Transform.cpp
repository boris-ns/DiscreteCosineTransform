#include <iostream>
#include "Transform.h"

using namespace std;

DCTransform::DCTransform(Matrix& c, Matrix& ln, Matrix& alpha)
	: matrixC(c), matrixLn(ln), matrixAlpha(alpha)
{
	size_t matrixDimension = c.size();

	InitMatrix(matrixR, matrixDimension);
	InitMatrix(matrixRR, matrixDimension);
	InitMatrix(matrixRRR, matrixDimension);
}

DCTransform::~DCTransform()
{
}

/* Resizes passed matrix to dimensions size*size. */
void DCTransform::InitMatrix(Matrix& matrix, size_t size)
{
	matrix.resize(size);

	for (size_t i = 0; i < matrix.size(); ++i)
		matrix[i].resize(size);
}

/* Calls phases of transformation and returns final result. */
Matrix& DCTransform::CalculateDCTransform()
{
	Phase1MatrixMultiplication();
	Phase2MatrixMultiplication();
	Phase3MatrixMultiplication();

	return matrixRRR;
}

/* Getter for matrixRRR. */
Matrix& DCTransform::GetResult()
{
	return matrixRRR;
}

/* 
	Phase 1. Matrix multiplication between Ln and C matrices.
	Result is stored in matrixR.
*/
void DCTransform::Phase1MatrixMultiplication()
{
	size_t size = matrixLn.size();

	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < size; ++j)
		{
			matrixR[i][j] = 0;

			for (size_t k = 0; k < size; ++k)
				matrixR[i][j] += matrixLn[i][k] * matrixC[k][j];
		}
	}
}

/*
	Phase 2. Multiplication (rows*rows) between R and C matrices.
	Result is stored in matrixRR.
*/
void DCTransform::Phase2MatrixMultiplication()
{
	size_t size = matrixR.size();

	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < size; ++j)
		{
			matrixRR[i][j] = 0;

			for (size_t k = 0; k < size; ++k)
				matrixRR[i][j] += matrixR[i][k] * matrixC[j][k];
		}
	}
}

/*
	Phase 3. Scalar matrix multiplication between Alpha and RR matrices.
	Result is stored in matrixRRR. This is the final phase of transformation.
*/
void DCTransform::Phase3MatrixMultiplication()
{
	size_t size = matrixRR.size();

	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < size; ++j)
			matrixRRR[i][j] = matrixAlpha[i][j] * matrixRR[i][j];
	}
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
