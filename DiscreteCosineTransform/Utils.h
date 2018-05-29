#pragma once

#include "Transform.h"

/* Resizes (creates NxN) matrices. */
void ResizeMatrix(Matrix& matrix, size_t size);

/* Prints matrix to the console. */
void PrintMatrix(const Matrix& matrix);	

/* Fills up matrix w/ random elements. */
void CreateMatrixWithRandomElements(Matrix& mat);

/* Writes matrix to file. */
void WriteMatrixToFile(const Matrix& matrix, const std::string& path);

/* Loads matrix from file. */
void LoadMatrixFromFile(Matrix& matrix, const std::string& path);

/* Compares two matrices. */
bool CompareMatrices(const Matrix& m1, const Matrix& m2);