#include <iostream>
#include <fstream>
#include "Transform.h"
#include "Utils.h"

using namespace std;

DCTransform::DCTransform(Matrix& c, Matrix& in, Matrix& alpha)
	: matrixC(c), matrixIn(in), matrixAlpha(alpha)
{
	size_t matrixDimension = c.size();

	ResizeMatrix(matrixR, matrixDimension);
	ResizeMatrix(matrixRR, matrixDimension);
	ResizeMatrix(matrixRRR, matrixDimension);
}

DCTransform::~DCTransform()
{
}

/* Calls phases of transformation and returns final result. */
void DCTransform::CalculateDCTransform()
{
	Phase1MatrixMultiplication();
	Phase2MatrixMultiplication();
	Phase3MatrixMultiplication();
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
	size_t size = matrixIn.size();

	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < size; ++j)
		{
			matrixR[i][j] = 0;

			for (size_t k = 0; k < size; ++k)
				matrixR[i][j] += matrixIn[i][k] * matrixC[k][j];
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
