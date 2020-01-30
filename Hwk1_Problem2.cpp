/*
Author: Cameron Allotey
Class: ECE 4122
Date: 8/22/19

Description:
HW1 Problem 2 - Numerical Integration
*/

#include <iostream>
#include <iomanip>
#include <string> 
#include <sstream>

//Function Declarations
void integrate(double upperBound, unsigned long numDiv);
double func(double x);
unsigned long validInput();

//Global Variables
double result = 0;

/*
Function Header
entry point into the application
Takes user input and calls integrate function
*/
int main()
{
	//Get upper bound as command line input
	double upperBound = 0;
	std::cout << "Please enter a value for the upper limit (beta): ";
	std::cin >> upperBound;
	if (std::cin.fail()) 
	{
		std::cout << "Invalid Input!" << std::endl;
		return 0;
	}
	//Get number of divisions as command line input
	std::cout << "Please enter the number of subdivisions to use: ";
	//take in as String to check for negative
	unsigned long numDiv = validInput();
	if (numDiv == -1) 
	{
		return 0;
	}
	integrate(upperBound, numDiv);
	std::cout << std::fixed << std::showpoint;
    std::cout << std::setprecision(16);
	std::cout << "The integral evaluates to: " << result << std::endl;
	return 0;
}

/*
Function Header
implements integration using the general midpoint rule formula
updates the global result variable based on the given function
*/
void integrate(double upperBound, unsigned long numDiv) 
{
	//check inputs (if B=0 return 0, numDiv must be integer > 0)
	if (upperBound == 0)
	{
		result = 0;
		return;
	}
	double dx = upperBound/numDiv;

	for (int i = 1; i <= numDiv; ++i)
	{
		//calculate function input using midpoint rule
		double m = ((2*i - 1) * upperBound)/(2*numDiv);
		result += func(m);
	}

	result *= dx;
}

/*
Function Header
implementation of given function
returns a double representing the result of calculation
*/
double func(double x) 
{
	return 4.0/(1.0 + x*x);
}

/*
Function Header
additional fucntion to check for valid input
returns the input if valid otherwise error
*/
unsigned long validInput()
{
	unsigned long result;
	std::string strIn;
	std::cin >> strIn;
	if (std::cin.fail())
	{
		std::cout << "Input Error! Input too large or null!" << std::endl;
		return -1;
	}
	std::string::const_iterator i = strIn.begin(); 
	while (i != strIn.end()) 
	{
		if (!std::isdigit(*i)) {
			std::cout << "Input invalid! Must be a positive integer" << std::endl;
			return -1;
		}
		++i;
	}
	std::stringstream(strIn) >> result;
	return result;
}