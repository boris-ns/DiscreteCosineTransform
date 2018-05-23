#include "Utils.h"
#include <iostream>
#include <fstream>

using namespace std;

/* Resizes passed matrix to dimensions size*size. */
void ResizeMatrix(Matrix& matrix, size_t size)
{
	matrix.resize(size);

	for (size_t i = 0; i < matrix.size(); ++i)
		matrix[i].resize(size);
}

/* Prints passed matrix to the console. */
void PrintMatrix(const Matrix& matrix)
{
	for (size_t i = 0; i < matrix.size(); ++i)
	{
		for (size_t j = 0; j < matrix[i].size(); ++j)
			cout << matrix[i][j] << " ";

		cout << endl;
	}
}

/* Writes passed matrix to the file. */
void WriteMatrixToFile(const Matrix& matrix, const string& path)
{
	ofstream outFile(path);

	for (size_t i = 0; i < matrix.size(); ++i)
	{
		for (size_t j = 0; j < matrix[i].size(); ++j)
			outFile << matrix[i][j] << " ";

		outFile << '\n';
	}

	outFile.close();
}

/* Functions that fills up matrix with random values. */
void CreateMatrixWithRandomElements(Matrix& mat)
{
	for (int i = 0; i < mat.size(); ++i)
	{
		for (int j = 0; j < mat[i].size(); ++j)
			mat[i][j] = rand() % 100;
	}
}

/* Compares two matrices. */
bool CompareMatrices(const Matrix& m1, const Matrix& m2)
{
	for (size_t i = 0; i < m1.size(); ++i)
	{
		for (size_t j = 0; j < m1[i].size(); ++j)
		{
			if (m1[i][j] != m2[i][j])
				return false;
		}
	}

	return true;
}

/* Loads elements from file to matrix. */
void LoadMatrixFromFile(Matrix& matrix, const std::string& path)
{
	ifstream inFile(path);

	for (size_t i = 0; i < matrix.size(); ++i)
	{
		for (size_t j = 0; j < matrix[i].size(); ++j)
			inFile >> matrix[i][j];
	}

	inFile.close();
}