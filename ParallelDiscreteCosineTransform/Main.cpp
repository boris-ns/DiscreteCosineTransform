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
	/*Matrix matrixIn = { { 0, 1 },{ 2, 3 } };
	Matrix matrixC = { { 4, 5 },{ 6, 7 } };
	Matrix matrixAlpha = { { 8, 9 },{ 10, 11 } };*/

	/*Matrix matrixIn = { { 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 }, {3, 4, 5, 6, 7},
						{ 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 }};
	Matrix matrixC = { { 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },
						{ 0, 1, 2, 3, 4 },{ 2, 3, 4, 5, 6 } };
	Matrix matrixAlpha = { { 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 },
						{ 2, 3, 4, 5, 6 },{ 2, 3, 4, 5, 6 } };*/

	size_t size = 100;

	Matrix matrixIn;
	Matrix matrixC;
	Matrix matrixAlpha;

	InitMatrix(matrixIn, size);
	InitMatrix(matrixC, size);
	InitMatrix(matrixAlpha, size);
	
	CreateMatrixWithRandomElements(matrixIn);
	CreateMatrixWithRandomElements(matrixC);
	CreateMatrixWithRandomElements(matrixAlpha);

	Matrix matrixR;
	Matrix matrixRR;
	Matrix matrixRRR;

	InitMatrix(matrixR, size);
	InitMatrix(matrixRR, size);
	InitMatrix(matrixRRR, size);

	steady_clock::time_point startSerial = steady_clock::now();
	//CalculateDCTransformSerial(&matrixAlpha, &matrixIn, &matrixC, &matrixR, &matrixRR, &matrixRRR);
	steady_clock::time_point finishSerial = steady_clock::now();
	
	steady_clock::time_point startParallel = steady_clock::now();
	CalculateDCTransformParallel2(&matrixAlpha, &matrixIn, &matrixC, &matrixR, &matrixRR, &matrixRRR);
	steady_clock::time_point finishParallel = steady_clock::now();

	duration<double, milli> elapsedTimeSerial   = finishSerial - startSerial;
	duration<double, milli> elapsedTimeParallel = finishParallel - startParallel;

	cout << "Result matrix:" << endl;
	//PrintMatrix(matrixRRR);
	cout << endl << "Matrix dimension: " << size << "x" << size << endl;
	cout << "Calculating time (serial w/ tasks): " << elapsedTimeSerial.count() << " ms" << endl;
	cout << "Calculating time (parallel): " << elapsedTimeParallel.count() << " ms" << endl;

	char x;
	cin >> x;
	return 0;
}