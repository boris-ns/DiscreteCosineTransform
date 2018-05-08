#include <iostream>
#include <vector>

using namespace std;

typedef vector<vector<int>> Matrix;


void PrintMatrix(const Matrix& matrix)
{
	for (int i = 0; i < matrix.size(); ++i)
	{
		for (int j = 0; j < matrix[i].size(); ++j)
		{
			cout << matrix[i][j] << " ";
		}

		cout << endl;
	}
}

void Phase1MatrixMultiplication(const Matrix& m1, const Matrix& m2, Matrix& result)
{
	size_t size = m1.size();

	result.resize(size);
	for (size_t i = 0; i < result.size(); ++i)
		result[i].resize(size);
	
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			result[i][j] = 0;

			for (int k = 0; k < size; ++k)
				result[i][j] += m1[i][k] * m2[k][j];
		}
	}
}

void Phase2MatrixMultiplication(const Matrix& m1, const Matrix& m2, Matrix& result)
{
	size_t size = m1.size();

	result.resize(size);
	for (size_t i = 0; i < result.size(); ++i)
		result[i].resize(size);

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			result[i][j] = 0;

			for (int k = 0; k < size; ++k)
			{
				result[i][j] += m1[i][k] * m2[j][k];
			}
		}
	}
}

void Phase3MatrixMultiplication(const Matrix& m1, const Matrix& m2, Matrix& result)
{
	size_t size = m1.size();

	result.resize(size);
	for (size_t i = 0; i < result.size(); ++i)
		result[i].resize(size);

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			result[i][j] = m1[i][j] * m2[i][j];
		}
	}
}

int main()
{
	Matrix matrixLn = { { 0, 1 },{ 2, 3 } };
	Matrix matrixC = { { 4, 5 },{ 6, 7 } };
	Matrix matrixAlpha = { { 8, 9 },{ 10, 11 } };
	Matrix matrixR;
	Matrix matrixRR;
	Matrix matrixRRR;

	cout << "Matrix Ln:" << endl;
	PrintMatrix(matrixLn);
	cout << "Matrix C:" << endl;
	PrintMatrix(matrixC);
	cout << "Matrix Alpha:" << endl;
	PrintMatrix(matrixAlpha);

	cout << "Matrix R:" << endl;	
	Phase1MatrixMultiplication(matrixLn, matrixC, matrixR);
	PrintMatrix(matrixR);

	cout << "Matrix RR:" << endl;
	Phase2MatrixMultiplication(matrixR, matrixC, matrixRR);
	PrintMatrix(matrixRR);

	cout << "Matrix RRR:" << endl;
	Phase3MatrixMultiplication(matrixAlpha, matrixRR, matrixRRR);
	PrintMatrix(matrixRRR);

	char x;
	cin >> x;
	return 0;
}