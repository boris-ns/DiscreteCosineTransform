//============================================================================
// Name        : Main.cpp
// Author      : Boris Sulicenko
// Date created: 08.05.2018.
// Description : Serial implementation of discrete cosine transform.
//============================================================================

#include <iostream>
#include <chrono>
#include "Transform.h"

#define PRINT_MATRIX 0

using namespace std;
using namespace chrono;

void CreateMatrixWithRandomElements(Matrix& mat)
{
	for (int i = 0; i < mat.size(); ++i)
	{
		for (int j = 0; j < mat[i].size(); ++j)
		{
			mat.at(i).at(j) = rand() % 100;
		}
	}
}

int main()
{
	/*Matrix matrixLn = { { 0, 1 }, { 2, 3 } };
	Matrix matrixC = { { 4, 5 }, { 6, 7 } };
	Matrix matrixAlpha = { { 8, 9 }, { 10, 11 } };*/

	/*Matrix matrixLn = { { 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 },{ 3, 4, 5, 6, 7 },
	{ 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 } };
	Matrix matrixC = { { 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },
	{ 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 } };
	Matrix matrixAlpha = { { 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },
	{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 } };*/

	size_t size = 2000;

	Matrix matrixIn;
	Matrix matrixC;
	Matrix matrixAlpha;

	InitMatrix(matrixIn, size);
	InitMatrix(matrixC, size);
	InitMatrix(matrixAlpha, size);

	CreateMatrixWithRandomElements(matrixIn);
	CreateMatrixWithRandomElements(matrixC);
	CreateMatrixWithRandomElements(matrixAlpha);

	DCTransform dct(matrixC, matrixIn, matrixAlpha);
	
	cout << "Calculating DCT..." << endl;
	steady_clock::time_point start = steady_clock::now();
	dct.CalculateDCTransform();
	steady_clock::time_point finish = steady_clock::now();
	cout << "Finished calculating DCT." << endl;

	duration<double, milli> elapsedTime = finish - start;

	Matrix result = dct.GetResult();

#if PRINT_MATRIX
	cout << "Result matrix:" << endl;
	PrintMatrix(result);
#endif
	cout << endl << "Matrix dimension: " << result.size() << "x" << result.size() << endl;
	cout << "Calculating time: " << elapsedTime.count() << " ms" << endl;

	char x;
	cin >> x;
	return 0;
}