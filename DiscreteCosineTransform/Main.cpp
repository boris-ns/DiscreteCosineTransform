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
void CalculateSerial(DCTransform& dct)
{
	cout << "Calculating DCT..." << endl;

	steady_clock::time_point start = steady_clock::now();
	dct.CalculateDCTransform();
	steady_clock::time_point finish = steady_clock::now();
	cout << "Finished calculating DCT." << endl << endl;

	duration<double, milli> elapsedTime = finish - start;
	cout << "Calculating time: " << elapsedTime.count() << " ms" << endl;

	//cout << "Result matrix:" << endl;
	//PrintMatrix(result);
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
		
		char x;
		cin >> x;
		return 0;
	}

	int matrixSize = stoi(argv[1]);

	Matrix matrixIn;
	Matrix matrixC;
	Matrix matrixAlpha;

	InitMatrices(argc, argv, matrixSize, matrixAlpha, matrixIn, matrixC);
	DCTransform dct(matrixC, matrixIn, matrixAlpha);
	
	cout << "Matrix dimension: " << matrixSize << "x" << matrixSize << endl;

	CalculateSerial(dct);

	// If matrices are loaded from files, there must be result to compare
	if (argc != 2) 
	{
		Matrix resultFromFile;
		ResizeMatrix(resultFromFile, matrixSize);
		LoadMatrixFromFile(resultFromFile, argv[5]);

		if (CompareMatrices(dct.GetResult(), resultFromFile))
			cout << "Matrices are equal!";
		else
			cout << "Matrices are not equal!";
	}

	char x;
	cin >> x;
	return 0;
}