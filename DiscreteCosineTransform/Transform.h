#pragma once

#include <vector>

typedef std::vector<std::vector<float>> Matrix;

void InitMatrix(Matrix& matrix, size_t size);	    // Resizes (creates NxN) matrices
void PrintMatrix(const Matrix& matrix);				// Prints matrix to the console
void CreateMatrixWithRandomElements(Matrix& mat);   // Fills up matrix w/ random elements

/*
	Class for storing and calculating phases of DCT.
*/
class DCTransform
{
public:
	DCTransform(Matrix& c, Matrix& in, Matrix& alpha);
	~DCTransform();

	Matrix& CalculateDCTransform();					// Calls multiplication phases, returns final result
	Matrix& GetResult();							// matrixRRR getter

private:
	void Phase1MatrixMultiplication();				// Multiplication (rows*cols), ln * C, result is R
	void Phase2MatrixMultiplication();				// Multiplication (rows*rows), R * C, result is RR
	void Phase3MatrixMultiplication();				// Multiplication (scalar), Alpha * RR, result is RRR

	Matrix matrixC;		// Input matrices
	Matrix matrixIn;
	Matrix matrixAlpha;

	Matrix matrixR;		// Result matrices 
	Matrix matrixRR;
	Matrix matrixRRR;   // Final result
};
