/*
Author:  Cameron Allotey
Date last modified: 9/10/2019
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
class ECE_Matrix
{
	unsigned int m_m,m_n;
	vector<vector<double>> m_data;
public:
	//getters and setters for member fields
	unsigned int get_m() const{return this->m_m;}
	unsigned int get_n() const{return this->m_n;}
	vector<vector<double>> get_data() const{return this->m_data;}
	void set_data(vector<vector<double>> data){m_data = data;}

	//no arg constructor
	ECE_Matrix():m_m{0}, m_n{0}{}
	//mxm constructor
	ECE_Matrix(unsigned int m, double value);
	//mxn constructor
	ECE_Matrix(unsigned int m, unsigned int n, double value);
	//file constructor
	ECE_Matrix(string fn);
	//destructor
	~ECE_Matrix() {}
	//copy constructor
	ECE_Matrix(const ECE_Matrix& rhs);
	ECE_Matrix& operator=(const double rhs);
	ECE_Matrix& operator=(const ECE_Matrix& rhs);
	ECE_Matrix operator+(const ECE_Matrix& rhs);
	ECE_Matrix operator-(const ECE_Matrix& rhs) const;
	ECE_Matrix transpose() const;

	ECE_Matrix operator*(const double &rhs) const;
	ECE_Matrix operator/(const double &rhs) const;
	ECE_Matrix operator+(double const &rhs) const;
	ECE_Matrix operator-(double const &rhs) const;

	ECE_Matrix& operator+=(const ECE_Matrix& rhs);
	ECE_Matrix& operator-=(const ECE_Matrix& rhs);

	friend ECE_Matrix operator * ( double const &lhs, ECE_Matrix const &rhs);
	friend ECE_Matrix operator / ( double const &lhs, ECE_Matrix const &rhs);
	friend ECE_Matrix operator + ( double const &lhs, ECE_Matrix const &rhs);
	friend ECE_Matrix operator - ( double const &lhs, ECE_Matrix const &rhs);

	// Stream insertion operator(output)
    friend ostream& operator<<(ostream& os, const ECE_Matrix& matrix)
    {
    	vector<vector<double>> m_data = matrix.m_data;
    	for (int i = 0; i < matrix.m_m; i++)
    	{
    		for (int j = 0; j < matrix.m_n; j++)
    		{
    			os << scientific << setw(12) << setprecision(3) << m_data[i][j];
    		}
    		os << endl;
    	}
        return os;
    }    
};