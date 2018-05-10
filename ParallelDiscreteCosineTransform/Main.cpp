//==============================================================================
// Name        : Main.cpp
// Author      : Boris Sulicenko
// Date created: 09.05.2018.
// Description : Parallel implementation of discrete cosine transform using TBB.
//==============================================================================

#include <iostream>
#include <chrono>
#include "Transform.h"

using namespace std;
using namespace chrono;

int main()
{
	Matrix matrixIn = { { 0, 1 },{ 2, 3 } };
	Matrix matrixC = { { 4, 5 },{ 6, 7 } };
	Matrix matrixAlpha = { { 8, 9 },{ 10, 11 } };
	Matrix matrixR;
	Matrix matrixRR;
	Matrix matrixRRR;

	size_t size = matrixIn.size();
	InitMatrix(matrixR, size);
	InitMatrix(matrixRR, size);
	InitMatrix(matrixRRR, size);

	steady_clock::time_point start = steady_clock::now();
	CalculateDCTransformParallel(&matrixAlpha, &matrixIn, &matrixC, &matrixR, &matrixRR, &matrixRRR);
	//CalculateDCTransformSerial(&matrixAlpha, &matrixIn, &matrixC, &matrixR, &matrixRR, &matrixRRR);
	steady_clock::time_point finish = steady_clock::now();

	duration<double, milli> elapsedTime = finish - start;

	cout << "Result matrix:" << endl;
	PrintMatrix(matrixRRR);
	cout << endl << "Matrix dimension: " << size << "x" << size << endl;
	cout << "Calculating time: " << elapsedTime.count() << " ms" << endl;

	char x;
	cin >> x;
	return 0;
}