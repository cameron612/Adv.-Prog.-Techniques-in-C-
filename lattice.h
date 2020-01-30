/*
Author:  Cameron Allotey
Date last modified: 9/19/2019
Organization: ECE 4122 Class

Description:

C++ class to handle command line arguments and get lattice dimensions
    
*/

#include <string.h>
#include <iostream>

using namespace std;
//Function Prototype
unsigned long long countPaths(int m, int n);
class lattice {
	int m_height;
	int m_width;
public:
	int get_height() {return m_height;}
	int get_width() {return m_width;}
	lattice(char **args) {
		string flag1 = args[1];
		string flag2 = args[2];
		if (args[1][1] == 'h') {
			m_height = atoi(args[2]);
			m_width = atoi(args[4]);
		} else {
			m_height = atoi(args[4]);
			m_width = atoi(args[2]);
		}
	}
};