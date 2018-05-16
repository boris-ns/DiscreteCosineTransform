//============================================================================
// Name        : Main.cpp
// Author      : Boris Sulicenko
// Date created: 08.05.2018.
// Description : Serial implementation of discrete cosine transform.
//============================================================================

#include <iostream>
#include <chrono>
#include <string>
#include "Transform.h"
#include "Utils.h"

using namespace std;
using namespace chrono;

/* Initializes matrices according to the passed cmd-line arguments*/
void InitMatrices(int argc, char* argv[], int size, Matrix& a, Matrix& in, Matrix& c)
{
	if (argc == 2) // Program will work with matrices with random elements
	{
		ResizeMatrix(a, size);
		ResizeMatrix(in, size);
		ResizeMatrix(c, size);

		CreateMatrixWithRandomElements(a);
		CreateMatrixWithRandomElements(in);
		CreateMatrixWithRandomElements(c);
	}
	else // Program will work with matrices that are loaded from files
	{
		ResizeMatrix(a, size);
		ResizeMatrix(in, size);
		ResizeMatrix(c, size);

		LoadMatrixFromFile(a, argv[2]);
		LoadMatrixFromFile(in, argv[3]);
		LoadMatrixFromFile(c, argv[4]);
	}
}

/*
 * Calls DCT calculation method, measueres time 
 * and prints information to the console. 
 */
Matrix CalculateSerial(Matrix& a, Matrix& in, Matrix& c)
{
	DCTransform dct(c, in, a);

	cout << endl << "Matrix dimension: " << a.size() << "x" << a.size() << endl;
	cout << "Calculating DCT..." << endl;

	steady_clock::time_point start = steady_clock::now();
	Matrix result = dct.CalculateDCTransform();
	steady_clock::time_point finish = steady_clock::now();
	cout << "Finished calculating DCT." << endl << endl;

	duration<double, milli> elapsedTime = finish - start;
	cout << "Calculating time: " << elapsedTime.count() << " ms" << endl;

	//cout << "Result matrix:" << endl;
	//PrintMatrix(result);

	return result;
}

int main(int argc, char* argv[])
{
	if (argc != 2 && argc != 6)
	{
		cout << "Wrong number of command line arguments." << endl;
		cout << "You can run program:" << endl;
		cout << "  1) serial.exe <matrix_dimension>" << endl;
		cout << "  2) serial.exe <matrix_dimension> <alpha_file> "
	         << "<in_file> <c_file> <result_file>";
		
		return 0;
	}

	int matrixSize = stoi(argv[1]);

	Matrix matrixIn;
	Matrix matrixC;
	Matrix matrixAlpha;

	InitMatrices(argc, argv, matrixSize, matrixAlpha, matrixIn, matrixC);
	Matrix result = CalculateSerial(matrixAlpha, matrixIn, matrixC);

	// Means we loaded matrices from files, so there must be result file to compare
	if (argc != 2) 
	{
		Matrix resultFromFile;
		ResizeMatrix(resultFromFile, matrixSize);
		LoadMatrixFromFile(resultFromFile, argv[5]);

		if (CompareMatrices(result, resultFromFile))
			cout << "Matrices are equivalent!";
		else
			cout << "Matrices are not equivalent!";
	}

	char x;
	cin >> x;
	return 0;
}