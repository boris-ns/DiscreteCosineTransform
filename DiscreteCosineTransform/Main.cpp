//============================================================================
// Name        : Main.cpp
// Author      : Boris Sulicenko
// Date created: 08.05.2018.
// Description : Serial implementation of discrete cosine transform.
//============================================================================

#include <iostream>
#include <chrono>
#include "Transform.h"

using namespace std;
using namespace chrono;

int main()
{
	Matrix matrixLn = { { 0, 1 }, { 2, 3 } };
	Matrix matrixC = { { 4, 5 }, { 6, 7 } };
	Matrix matrixAlpha = { { 8, 9 }, { 10, 11 } };

	DCTransform dct(matrixC, matrixLn, matrixAlpha);
	
	steady_clock::time_point start = steady_clock::now();
	dct.CalculateDCTransform();
	steady_clock::time_point finish = steady_clock::now();

	duration<double, milli> elapsedTime = finish - start;

	Matrix result = dct.GetResult();

	cout << "Result matrix:" << endl;
	PrintMatrix(result);

	cout << endl << "Matrix dimension: " << result.size() << "x" << result.size() << endl;
	cout << "Calculating time: " << elapsedTime.count() << " ms" << endl;

	char x;
	cin >> x;
	return 0;
}