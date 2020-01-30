/*
Author:  Cameron Allotey
Date last modified: 10/5/2019
Organization: ECE 4122 Class

Description:

C++ class for given matrix operations
    
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;
//header file
class Simple_Matrix
{
	unsigned int m_m,m_n;
	vector<vector<long long>> m_data;
public:
	//getters and setters for member fields
	unsigned int get_m() const{return this->m_m;}
	unsigned int get_n() const{return this->m_n;}
	vector<vector<long long>> get_data() const{return this->m_data;}
	void set_data(vector<vector<long long>> data){m_data = data;}
	//file constructor
	Simple_Matrix(string fn);
	//destructor
	~Simple_Matrix() {}
	// Stream insertion operator(output)
    friend ostream& operator<<(ostream& os, const Simple_Matrix& matrix)
    {
    	vector<vector<long long>> m_data = matrix.m_data;
    	for (int i = 0; i < matrix.m_m; ++i)
    	{
    		for (int j = 0; j < matrix.m_n; ++j)
    		{
    			os << scientific << setw(4) << setprecision(3) << m_data[i][j];
    		}
    		os << endl;
    	}
        return os;
    }    
};