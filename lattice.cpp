/*
Author:  Cameron Allotey
Date last modified: 10/4/2019
Organization: ECE 4122 Class

Description:

C++ entry point of program to get number of paths through a lattice
    
*/

#include <omp.h>
#include "lattice.h"

/*Function Header
Program Entry Point
Initializes lattice , handles errors, then calls function to compute*/
int main(int argc, char *argv[]) {
	string flag1, flag2;
	if (argc != 5) {
		cout<<"Input Error! Incorrect number of arguments!!"<<endl;
		return 0;
	}
	lattice myLattice(argv);
	unsigned long long res = countPaths(myLattice.get_height(), myLattice.get_width());
	cout<<"Number of Routes: "<<res<<endl;
	cout<<endl;
	return 0;

}

/*Function Header
computational function to find number of paths
Uses one worker thread to satisfy multithreading requirment
Uses unsigned long long for computation to avoid overflow
Uses combinatoric solution where number of paths = (m+n)!/(m!n!)*/
unsigned long long countPaths(int m, int n) {
	//set number of worker threads to 1
	const int nThreads = 1;
	omp_set_num_threads(nThreads);

	//computes a running factorial
	unsigned long long fact;
	#pragma omp parallel
	{
	    fact = 1; 
	    //prevents overflow by shortening loop to avoid 40!
	    for (unsigned long long i = n+1; i < (m + n + 1); ++i) { 
	        fact *= i; 
	        fact /= (i - n); 
	    } 
	}
	return fact;
}