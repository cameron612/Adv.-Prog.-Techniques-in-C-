/*
Author: Cameron Allotey
Class: ECE 4122
Date: 8/22/19

Description:
HW1 Problem 1 - One More One
*/

#include <iostream>

//Function Declarations
int oneMore(int input);
int validInput();

//Global Variables
int count = 0;

/*
Function Header
entry point into the application
returns the number of iterations before termination of algorithm
*/
int main()
{
	int input = 0;
	std::cout << "Please enter the starting number n: ";
	input = validInput();
	if (input != -1) oneMore(input);
	return 0;
}

/*
Function Header
recursive call to complete the algorithm process
returns the next number in the pattern or prints to console when complete
*/
int oneMore(int input)
{
	if(input == 1)
	{
		std::cout << "The sequence had " << count << " instance(s) of 1 added." << std::endl;
		return 0;
	}
	else if (input % 7 == 0)
	{
		input /= 7;
	}
	else
	{
		++input;
		++count;
	}

	return oneMore(input);
}

/*
Function Header
additional fucntion to check for valid input
returns the input if valid otherwise error
*/
int validInput()
{
	int result;
	std::cin >> result;
	if (std::cin.fail() || result < 1)
	{
		std::cout << "Input Error! Incompatible input please enter a positive integer" << std::endl;
		return -1;
	}
	return result;
}