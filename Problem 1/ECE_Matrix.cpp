/*
Author:  Cameron Allotey
Date last modified: 9/10/2019
Organization: ECE 4122 Class

Description:

Implementation of C++ class for given matrix operations
    
*/
#include "ECE_Matrix.h"
///////////////////////////////
//constructor implementations//
///////////////////////////////


/*m,n, value constructor
ECE_Matrix Constructor
creates a m x n martix initialized to the given value*/
ECE_Matrix::ECE_Matrix(unsigned int m, unsigned int n, double value):m_m(m), m_n(n)
{
	m_data.resize(m);
	for (unsigned int i = 0; i < m; ++i)
	{
		m_data[i].resize(n, value);
	}
}

/*m, value constructor
ECE_Matrix Constructor
creates a m x m martix initialized to the given value*/
ECE_Matrix::ECE_Matrix(unsigned int m, double value):m_m(m), m_n(m)
{
	m_data.resize(m);
	for (unsigned int i = 0; i < m; ++i)
	{
		m_data[i].resize(m, value);
	}
}

/*string constructor
ECE_Matrix Constructor
Inputs a text file name
creates a m x n martix where m and n are in the first line of the text file
initialized to the given values in the text file*/
ECE_Matrix::ECE_Matrix(string fn)
{
	ifstream infile;
	infile.open(fn);
	infile >> m_m >> m_n;
	m_data.resize(m_m);
	for (unsigned int ii = 0; ii < m_m; ++ii)
	{
		if (!infile.eof())
		{
			m_data[ii] = vector<double>(m_n);
			for (unsigned int jj = 0; jj < m_n; ++jj)
			{
				if (!infile.eof())
				{
					infile >> m_data[ii][jj];
				}            
			}        
		}    
	}    
	infile.close();
}

/*copy constructor
ECE_Matrix Constructor
Set current ECE_Matrix insance values to rhs values*/
ECE_Matrix::ECE_Matrix(const ECE_Matrix& rhs)
{
	m_data = rhs.m_data;
	m_m = rhs.get_m();
	m_n = rhs.get_n();
}

////////////////////////////
//operator implementations//
////////////////////////////

/*equals operation given double
ECE_Matrix operator=
Set current ECE_Matrix data values to rhs*/
ECE_Matrix& ECE_Matrix::operator=(const double rhs)
{
	for (unsigned int i = 0; i < m_m; ++i) 
	{
    	for (unsigned int j = 0; j < m_n; ++j)
	    {
	    	m_data[i][j] = rhs;
	    }
	}
	return *this;
}

/*equals operation given ECE_Matrix
ECE_Matrix operator=
Set current ECE_Matrix data values to rhs values*/
ECE_Matrix& ECE_Matrix::operator=(const ECE_Matrix& rhs)
{
	if (&rhs == this)
	{
    	return *this;
	}
	unsigned int m = rhs.get_m();
	unsigned int n = rhs.get_n();
	m_data.resize(m);
	for (unsigned int i = 0; i < m; ++i)
	{
    	m_data[i].resize(n);
    	for (unsigned int j = 0; j < n; ++j)
	    {
	    	m_data[i][j] = rhs.m_data[i][j];
	    }
	}
	m_m = m;
	m_n = n;
	return *this;
}

/*addition operation given ECE_Matrix
ECE_Matrix operator+
Set current ECE_Matrix data values to value + rhs values
If dimensions mismatch, use smallest required size to hold all data*/
ECE_Matrix ECE_Matrix::operator+(const ECE_Matrix& rhs)
{
	ECE_Matrix newMat(max(rhs.m_m, m_m), max(rhs.m_n, m_n), 0);
	double left,right = 0.0;
	for (unsigned int i = 0; i < newMat.m_m; ++i)
	{
    	for (unsigned int j = 0; j < newMat.m_n; ++j)
	    {
	    	if (i <= m_m-1 && j <= m_n-1)
	    	{
	    		left = m_data[i][j];
	    	} else {
	    		left = 0.0;
	    	}
	    	if (i <= rhs.m_m-1 && j <= rhs.m_n-1)
	    	{
	    		right = rhs.m_data[i][j];
	    	} else {
	    		right = 0.0;
	    	}
	    	newMat.m_data[i][j] = left + right;
	    }
	}
	return newMat;
}
	
/*subtraction operation given ECE_Matrix
ECE_Matrix operator-
Set current ECE_Matrix data values to value - rhs values
If dimensions mismatch, use smallest required size to hold all data*/
ECE_Matrix ECE_Matrix::operator-(const ECE_Matrix& rhs) const
{
	ECE_Matrix newMat(max(rhs.m_m, m_m), max(rhs.m_n, m_n), 0);
	double left,right = 0.0;
	for (unsigned int i = 0; i < newMat.m_m; ++i)
	{
    	for (unsigned int j = 0; j < newMat.m_n; ++j)
	    {
	    	if (i <= m_m-1 && j <= m_n-1)
	    	{
	    		left = m_data[i][j];
	    	} else {
	    		left = 0.0;
	    	}
	    	if (i <= rhs.m_m-1 && j <= rhs.m_n-1)
	    	{
	    		right = rhs.m_data[i][j];
	    	} else {
	    		right = 0.0;
	    	}
	    	newMat.m_data[i][j] = left - right;
	    }
	}
	return newMat;
}

/*Transpose operation on ECE_Matrix instance
ECE_Matrix transpose operation
return transpose of ECE_Matrix instance*/
ECE_Matrix ECE_Matrix::transpose() const
{
	ECE_Matrix newMat(m_n,m_m,0);
	for (unsigned int i = 0; i < m_n; ++i)
	{
    	for (unsigned int j = 0; j < m_m; ++j)
	    {
	    	newMat.m_data[i][j] = m_data[j][i];
	    }
	}
	return newMat;
}

/*multiplication operation given double value
ECE_Matrix operator*
return new ECE_Matrix with data equal to value * rhs*/
ECE_Matrix ECE_Matrix::operator*(double const &rhs) const
{
	ECE_Matrix newMat(m_m,m_n,0);
	for (unsigned int i = 0; i < m_m; ++i)
	{
    	for (unsigned int j = 0; j < m_n; ++j)
	    {
	    	newMat.m_data[i][j] = m_data[i][j] * rhs;
	    }
	}
	return newMat;
}

/*division operation given double value
ECE_Matrix operator/
return new ECE_Matrix with data equal to value / rhs*/
ECE_Matrix ECE_Matrix::operator/(double const &rhs) const
{
	ECE_Matrix newMat(m_m,m_n,0);
	if (rhs == 0) {
		return newMat;
	}
	for (unsigned int i = 0; i < m_m; ++i)
	{
    	for (unsigned int j = 0; j < m_n; ++j)
	    {
	    	newMat.m_data[i][j] = m_data[i][j] / rhs;
	    }
	}
	return newMat;
}

/*addition operation given double value
ECE_Matrix operator+
return new ECE_Matrix with data equal to value + rhs*/
ECE_Matrix ECE_Matrix::operator+(double const &rhs) const
{
	ECE_Matrix newMat(m_m,m_n,0);
	for (unsigned int i = 0; i < m_m; ++i)
	{
    	for (unsigned int j = 0; j < m_n; ++j)
	    {
	    	newMat.m_data[i][j] = m_data[i][j] + rhs;
	    }
	}
	return newMat;
}

/*subtraction operation given double value
ECE_Matrix operator-
return new ECE_Matrix with data equal to value - rhs*/
ECE_Matrix ECE_Matrix::operator-(double const &rhs) const
{
	ECE_Matrix newMat(m_m,m_n,0);
	if (rhs == 0) {
		return newMat;
	}
	for (unsigned int i = 0; i < m_m; ++i)
	{
    	for (unsigned int j = 0; j < m_n; ++j)
	    {
	    	newMat.m_data[i][j] = this->m_data[i][j] - rhs;
	    }
	}
	return newMat;
}

/*addition operation given matrix
ECE_Matrix operator+=
return new ECE_Matrix with data equal to value + rhs value
Resize matrix appropriately to accomodate mismatching dimensions*/
ECE_Matrix& ECE_Matrix::operator+=(const ECE_Matrix& rhs)
{
	if (m_m != rhs.m_m) {
		m_m = max(rhs.m_m, m_m);
		this -> m_data.resize(m_m);
	}
	int flag = 0;
	if (m_n != rhs.m_n) {
		m_n = max(rhs.m_n, m_n);
		flag = 1;
	}
	for (unsigned int i = 0; i < m_m; ++i)
	{
		if (flag) {
			this -> m_data[i].resize(m_n);
		}
    	for (unsigned int j = 0; j < m_n; ++j)
	    {
	    	this->m_data[i][j] = m_data[i][j] + rhs.m_data[i][j];
	    }
	}
	return *this;
}

/*subtraction operation given matrix
ECE_Matrix operator-=
return new ECE_Matrix with data equal to value - rhs value
Resize matrix appropriately to accomodate mismatching dimensions*/
ECE_Matrix& ECE_Matrix::operator-=(const ECE_Matrix& rhs)
{
	if (m_m != rhs.m_m) {
		m_m = max(rhs.m_m, m_m);
		this -> m_data.resize(m_m);
	}
	int flag = 0;
	if (m_n != rhs.m_n) {
		m_n = max(rhs.m_n, m_n);
		flag = 1;
	}
	for (unsigned int i = 0; i < m_m; ++i)
	{
		if (flag) {
			this -> m_data[i].resize(m_n);
		}
    	for (unsigned int j = 0; j < m_n; ++j)
	    {
	    	this->m_data[i][j] = m_data[i][j] - rhs.m_data[i][j];
	    }
	}
	return *this;
}

/*addition operation given double value and matrix
friend function ECE_Matrix operator +
return new ECE_Matrix with data equal to value + rhs*/
ECE_Matrix operator + ( double const &lhs, ECE_Matrix const &rhs)
{
	ECE_Matrix newMat(rhs.get_m(),rhs.get_n(),0);
	vector<vector<double>> data = rhs.get_data();
	for (unsigned int i = 0; i < rhs.get_m(); ++i)
	{
    	for (unsigned int j = 0; j < rhs.get_n(); ++j)
	    {
	    	data[i][j] = data[i][j] + lhs;
	    }
	}
	newMat.set_data(data);
	return newMat;
}

/*subtraction operation given double value and matrix
friend function ECE_Matrix operator -
return new ECE_Matrix with data equal to value - rhs*/
ECE_Matrix operator - ( double const &lhs, ECE_Matrix const &rhs)
{
	ECE_Matrix newMat(rhs.get_m(),rhs.get_n(),0);
	vector<vector<double>> data = rhs.get_data();
	for (unsigned int i = 0; i < rhs.get_m(); ++i)
	{
    	for (unsigned int j = 0; j < rhs.get_n(); ++j)
	    {
	    	data[i][j] = data[i][j] - lhs;
	    }
	}
	newMat.set_data(data);
	return newMat;
} 

/*multiplication operation given double value and matrix
friend function ECE_Matrix operator *
return new ECE_Matrix with data equal to value * rhs*/
ECE_Matrix operator * ( double const &lhs, ECE_Matrix const &rhs)
{
	ECE_Matrix newMat(rhs.get_m(),rhs.get_n(),0);
	vector<vector<double>> data = rhs.get_data();
	for (unsigned int i = 0; i < rhs.get_m(); ++i)
	{
    	for (unsigned int j = 0; j < rhs.get_n(); ++j)
	    {
	    	data[i][j] = data[i][j] * lhs;
	    }
	}
	newMat.set_data(data);
	return newMat;
} 

/*division operation given double value and matrix
friend function ECE_Matrix operator /
return new ECE_Matrix with data equal to value / rhs*/
ECE_Matrix operator / ( double const &lhs, ECE_Matrix const &rhs)
{
	ECE_Matrix newMat(rhs.get_m(),rhs.get_n(),0);
	vector<vector<double>> data = rhs.get_data();
	for (unsigned int i = 0; i < rhs.get_m(); ++i)
	{
    	for (unsigned int j = 0; j < rhs.get_n(); ++j)
	    {
	    	data[i][j] = data[i][j] / lhs;
	    }
	}
	newMat.set_data(data);
	return newMat;
} 