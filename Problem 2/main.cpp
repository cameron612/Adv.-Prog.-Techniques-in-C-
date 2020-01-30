/*
Author:  Cameron Allotey
Date last modified: 9/17/2019
Organization: ECE 4122 Class

Description:

C++ entry point of program to get max product
    
*/
#include "Simple_Matrix.h"

using namespace std;

//Function Prototype
long long getMatrixMaxProd(const Simple_Matrix& mat);

/*Function Header
Program Entry Point*/
int main(int argc, char **argv) 
{
	if (argc != 2)
	{
		std::cout<<"Invalid Input(s)! Please only input the file name containing the matrix"<<std::endl;
		return 0;
	}

	const Simple_Matrix mat(argv[1]);
	long long result = getMatrixMaxProd(mat);
	//write result to text file
	ofstream myfile;
	myfile.open("output2.txt");
	myfile << scientific << setprecision(16) << result << std::endl;
	myfile.close();
	return 0;
}

/*Function Header
Helper fucntion to get product*/
long long getMatrixMaxProd(const Simple_Matrix& mat) {
	//all will be positive
	long long max = 0;

	unsigned int i = 3;
	unsigned int j = 3;
	//matrix won't be smaller than 4x4
	//70600674
	vector<vector<long long>> data = mat.get_data();
	while (i < mat.get_m())
	{
		j = 3;
		while (j < mat.get_n())
		{
			//horizontal
			max = std::max(max, data[i][j - 3] * data[i][j - 2] * data[i][j - 1] * data[i][j]);
			//vertical
			max = std::max(max, data[i - 3][j] * data[i - 2][j] * data[i - 1][j] * data[i][j]);
			//diagonal
			max = std::max(max, data[i -3][j - 3] * data[i - 2][j - 2] * data[i - 1][j - 1] * data[i][j]);
			//anti diagonal
			max = std::max(max, data[i][j - 3] * data[i - 1][j - 2] * data[i - 2][j - 1] * data[i - 3][j]);
			++j;
		}
		++i;
	}
	return max;
}