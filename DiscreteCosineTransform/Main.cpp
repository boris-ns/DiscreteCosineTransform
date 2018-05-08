//============================================================================
// Name        : Main.cpp
// Author      : Boris Sulicenko
// Date created: 08.05.2018.
// Description : Serial implementation of discrete cosine transform.
//============================================================================

#include <iostream>
#include "Transform.h"

using namespace std;

int main()
{
	Matrix matrixLn = { { 0, 1 }, { 2, 3 } };
	Matrix matrixC = { { 4, 5 }, { 6, 7 } };
	Matrix matrixAlpha = { { 8, 9 }, { 10, 11 } };

	DCTransform dct(matrixC, matrixLn, matrixAlpha);
	dct.CalculateDCTransform();

	Matrix result = dct.GetResult();

	cout << "Result matrix:" << endl;
	PrintMatrix(result);

	char x;
	cin >> x;
	return 0;
}