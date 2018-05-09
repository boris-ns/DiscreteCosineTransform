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

int main()
{

	steady_clock::time_point start = steady_clock::now();
	CalculateDCTransform();
	steady_clock::time_point finish = steady_clock::now();

	duration<double, milli> elapsedTime = finish - start;


	cout << "Calculating time: " << elapsedTime.count() << " ms" << endl;

	char x;
	cin >> x;
	return 0;
}