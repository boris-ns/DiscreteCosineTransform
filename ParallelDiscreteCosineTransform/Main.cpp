//==============================================================================
// Name        : Main.cpp
// Author      : Boris Sulicenko
// Date created: 09.05.2018.
// Description : Parallel implementation of discrete cosine transform using TBB.
//==============================================================================

#include <iostream>
#include <chrono>
#include "Transform.h"

#define MATRIX_SIZE  1000
#define PRINT_MATRIX 0
#define CALC_SERIAL  1

using namespace std;
using namespace chrono;

int main()
{
	// Test matrices 2x2
	/*Matrix matrixIn = { { 0, 1 },{ 2, 3 } };
	Matrix matrixC = { { 4, 5 },{ 6, 7 } };
	Matrix matrixAlpha = { { 8, 9 },{ 10, 11 } };*/

	// Test matrices 5x5
	/*Matrix matrixIn = { { 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 }, {3, 4, 5, 6, 7},
						{ 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 }};
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

	Matrix matrixR;
	Matrix matrixRR;
	Matrix matrixRRR;

	InitMatrix(matrixR, MATRIX_SIZE);
	InitMatrix(matrixRR, MATRIX_SIZE);
	InitMatrix(matrixRRR, MATRIX_SIZE);

	cout << "Matrix dimension: " << MATRIX_SIZE << "x" << MATRIX_SIZE << endl << endl;

#if CALC_SERIAL
	cout << "Started calculating, serial with tasks..." << endl;
	steady_clock::time_point startSerial = steady_clock::now();
	CalculateDCTransformSerial(&matrixAlpha, &matrixIn, &matrixC, &matrixR, &matrixRR, &matrixRRR);
	steady_clock::time_point finishSerial = steady_clock::now();
	cout << "Finished calculating, serial." << endl;
	
	duration<double, milli> elapsedTimeSerial   = finishSerial - startSerial;
	cout << "Calculating time (serial w/ tasks): " << elapsedTimeSerial.count() << " ms" << endl << endl;
#endif

	cout << "Started calculating, parallel..." << endl;
	steady_clock::time_point startParallel = steady_clock::now();
	CalculateDCTransformParallel(&matrixAlpha, &matrixIn, &matrixC, &matrixR, &matrixRR, &matrixRRR);
	steady_clock::time_point finishParallel = steady_clock::now();
	cout << "Finished calculating, parallel." << endl;

	duration<double, milli> elapsedTimeParallel = finishParallel - startParallel;
	cout << "Calculating time (parallel): " << elapsedTimeParallel.count() << " ms" << endl << endl;

	WriteMatrixToFile(matrixRRR, "../results/parallel_result.txt");

#if PRINT_MATRIX
	cout << endl << "Result matrix:" << endl;
	PrintMatrix(matrixRRR);
#endif

	char x;
	cin >> x;
	return 0;
}