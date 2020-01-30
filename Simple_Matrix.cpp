/*
Author:  Cameron Allotey
Date last modified: 10/5/2019
Organization: ECE 4122 Class

Description:

Implementation of C++ class for given matrix operations
    
*/
#include "Simple_Matrix.h"
///////////////////////////////
//constructor implementations//
///////////////////////////////

/*string constructor
Simple_Matrix Constructor
Inputs a text file name
creates a m x n martix where m and n are in the first line of the text file
initialized to the given values in the text file*/
Simple_Matrix::Simple_Matrix(string fn)
{
	ifstream infile;
	infile.open(fn);
	infile >> m_m >> m_n;
	m_data.resize(m_m);
	for (unsigned int ii = 0; ii < m_m; ++ii)
	{
		if (!infile.eof())
		{
			m_data[ii] = vector<long long>(m_n);
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
