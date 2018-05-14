//============================================================================
// Name        : Main.cpp
// Author      : Boris Sulicenko
// Date created: 08.05.2018.
// Description : Serial implementation of discrete cosine transform.
//============================================================================

#include <iostream>
#include <chrono>
#include "Transform.h"

#define MATRIX_SIZE  1700
#define PRINT_MATRIX 0

using namespace std;
using namespace chrono;

int main()
{
	// Test matrices, 2x2
	/*Matrix matrixIn = { { 0, 1 }, { 2, 3 } };
	Matrix matrixC = { { 4, 5 }, { 6, 7 } };
	Matrix matrixAlpha = { { 8, 9 }, { 10, 11 } };*/

	// Test matrices, 5x5
	/*Matrix matrixIn = { { 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 },{ 3, 4, 5, 6, 7 },
						{ 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 } };
	Matrix matrixC = { { 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },
						{ 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 } };
	Matrix matrixAlpha = { { 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },
						{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 } };*/

	// Test matrices 10x10
	/*Matrix matrixIn = { { 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 },{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 },
	{ 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 },{ 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 },
	{ 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 },{ 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 } };
	Matrix matrixC = { { 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 },{ 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },
	{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 },{ 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 },{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 },
	{ 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 },{ 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 } };
	Matrix matrixAlpha = { { 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 }, { 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 }, { 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 },
	{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 },{ 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 },{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 },
	{ 2, 3, 4, 5, 6, 2, 3, 4, 2, 3 },{ 0, 1, 2, 3, 4, 2, 3, 4, 5, 6 },{ 3, 4, 5, 6, 7, 2, 3, 4, 5, 6 } };*/

	Matrix matrixIn;
	Matrix matrixC;
	Matrix matrixAlpha;

	InitMatrix(matrixIn, MATRIX_SIZE);
	InitMatrix(matrixC, MATRIX_SIZE);
	InitMatrix(matrixAlpha, MATRIX_SIZE);

	CreateMatrixWithRandomElements(matrixIn);
	CreateMatrixWithRandomElements(matrixC);
	CreateMatrixWithRandomElements(matrixAlpha);

	DCTransform dct(matrixC, matrixIn, matrixAlpha);
	
	cout << "Calculating DCT..." << endl;
	steady_clock::time_point start = steady_clock::now();
	Matrix result = dct.CalculateDCTransform();
	steady_clock::time_point finish = steady_clock::now();
	cout << "Finished calculating DCT." << endl << endl;

	duration<double, milli> elapsedTime = finish - start;

	cout << endl << "Matrix dimension: " << result.size() << "x" << result.size() << endl;
	cout << "Calculating time: " << elapsedTime.count() << " ms" << endl;

#if PRINT_MATRIX
	cout << "Result matrix:" << endl;
	PrintMatrix(result);
	WriteMatrixToFile(result, "../results/serial_result.txt");
#endif

	char x;
	cin >> x;
	return 0;
}