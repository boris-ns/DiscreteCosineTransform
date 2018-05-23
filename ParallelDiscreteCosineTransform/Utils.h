#pragma once

#include "Transform.h"

/* Resizes passed matrix to dimensions size*size. */
void ResizeMatrix(Matrix& matrix, size_t size);

/* Prints passed matrix to the console. */
void PrintMatrix(const Matrix& matrix);

/* Writes passed matrix to the file. */
void WriteMatrixToFile(const Matrix& matrix, const std::string& path);

/* Functions that fills up matrix with random values. */
void CreateMatrixWithRandomElements(Matrix& mat);

/* Compares two matrices. */
bool CompareMatrices(const Matrix& m1, const Matrix& m2);

/* Loads elements from file to matrix. */
void LoadMatrixFromFile(Matrix& matrix, const std::string& path);