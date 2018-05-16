//==============================================================================
// Name        : Main.cpp
// Author      : Boris Sulicenko
// Date created: 09.05.2018.
// Description : Parallel implementation of discrete cosine transform using TBB.
//==============================================================================

#include <iostream>
#include <chrono>
#include <string>
#include "Transform.h"
#include "Utils.h"

using namespace std;
using namespace chrono;

/* Initializes matrices according to the passed cmd-line arguments*/
void InitMatrices(int argc, char* argv[], int size, Matrix& a, Matrix& in, Matrix& c,
					Matrix& result)
{
	ResizeMatrix(a, size);
	ResizeMatrix(in, size);
	ResizeMatrix(c, size);
	ResizeMatrix(result, size);

	if (argc == 2) // Program will work with matrices with random elements
	{
		CreateMatrixWithRandomElements(a);
		CreateMatrixWithRandomElements(in);
		CreateMatrixWithRandomElements(c);
	}
	else // Program will work with matrices that are loaded from files
	{
		LoadMatrixFromFile(a, argv[2]);
		LoadMatrixFromFile(in, argv[3]);
		LoadMatrixFromFile(c, argv[4]);
	}
}

/* 
	Calls calculate serial w/ tasks function 
	and prints informations to the console.
*/
void CalculateSerial(Matrix* alpha, Matrix* in, Matrix* c, Matrix* serialResult)
{
	cout << "Started calculating, serial with tasks..." << endl;

	steady_clock::time_point startSerial = steady_clock::now();
	CalculateDCTransformSerial(alpha, in, c, serialResult);
	steady_clock::time_point finishSerial = steady_clock::now();

	cout << "Finished calculating, serial." << endl;

	duration<double, milli> elapsedTimeSerial = finishSerial - startSerial;
	cout << "Calculating time (serial w/ tasks): " 
		 << elapsedTimeSerial.count() << " ms" << endl << endl;
}

/*
	Calls function for parallel DCT calculation 
	and prints information to the console.
*/
void CalculateParallel(Matrix* alpha, Matrix* in, Matrix* c, Matrix* parallelResult)
{
	Matrix r;
	Matrix rr;
	ResizeMatrix(r, in->size());
	ResizeMatrix(rr, in->size());

	cout << "Started calculating, parallel..." << endl;
	
	steady_clock::time_point startParallel = steady_clock::now();
	CalculateDCTransformParallel3(alpha, in, c, &r, &rr, parallelResult);
	steady_clock::time_point finishParallel = steady_clock::now();
	
	cout << "Finished calculating, parallel." << endl;

	duration<double, milli> elapsedTimeParallel = finishParallel - startParallel;
	cout << "Calculating time (parallel): " 
		 << elapsedTimeParallel.count() << " ms" << endl << endl;
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

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
	matrixSize = 2000;

	Matrix matrixIn;
	Matrix matrixC;
	Matrix matrixAlpha;
	Matrix result;

	InitMatrices(argc, argv, matrixSize, matrixAlpha, matrixIn, matrixC, result);

	cout << "Matrix dimension: " << matrixSize << "x" << matrixSize << endl << endl;

	/* If you want to run serial and parallel at the same time
	 * don't go with matrix dimension higher than 1000.
	 * Program will become buggy and it might crash. */
	//CalculateSerial(&matrixAlpha, &matrixIn, &matrixC, &result);
	CalculateParallel(&matrixAlpha, &matrixIn, &matrixC, &result);

	// If matrices are loaded from files, there must be result to compare
	if (argc != 2)
	{
		Matrix resultFromFile;
		ResizeMatrix(resultFromFile, matrixSize);
		LoadMatrixFromFile(resultFromFile, argv[5]);

		if (CompareMatrices(result, resultFromFile))
			cout << "Matrices are equal!";
		else
			cout << "Matrices are not equal!";
	}

	//cout << endl << "Result matrix:" << endl;
	//PrintMatrix(parallelResult);

	char x;
	cin >> x;
	return 0;
}