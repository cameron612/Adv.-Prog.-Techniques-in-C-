/*
Author:  Cameron Allotey
Date last modified: 11/1/2019
Organization: ECE 4122 Class

Description:

Battlestar Buzzy: Use MPI to navigate 7 ships to dock on buzzy
    
*/
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <math.h>
#include "mpi.h"

using namespace std;

//variable definitions
#define MASTER 0

//function definitions
void getShipInfo(double * sendArr, int rank, double * recArr);
void updateJacket(int t, int rank);
void updateBuzzy();
double getNewForce();
double updateStatus(int rank, double * recArr);

//struct definitions
//struct to hold ship information(buzzy and jackets)
struct ship 
{
	double x, y, z, v, i, j, k, f;
	unsigned int status;
} buzzy, jacket;

/*Function Header
Program Entry Point
Initializes MPI and conducts main program activity loop*/
int main(int argc, char** argv)
{
	int numtasks, rank, rc;

	rc = MPI_Init(&argc, &argv);

	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	srand(rank);
	//get inital values
	unsigned int totTime;
	double maxF;
	double arr[58], sendArr[7], *recArr;
	double jX, jY, jZ, jVo, jI, jJ, jK;
	//status = 0 destoryed,  1 active, 2 docked
	unsigned int status = 1;
	//read input file
	if (rank == MASTER)
	{
		vector<double> inVec;
		//load in.dat
		ifstream infile;
		infile.open("in.dat");
		//1. time in seconds for docking
		infile >> totTime;
		inVec.push_back((double)totTime);
		//2. Fmax
		infile >> maxF;
		inVec.push_back(maxF);
		//3. x y z loc, initial speed, initial normalized direction
		double bX, bY, bZ, bVo, bI, bJ, bK;
		infile >> bX >> bY >> bZ >> bVo >> bI >> bJ >> bK;
		inVec.insert( inVec.end() , {bX, bY, bZ, bVo, bI, bJ, bK});
		//4. 7 lines same as 3 for each buzzy
		for (int i = 1; i < 8; i++)
		{
			infile >> jX >> jY >> jZ >> jVo >> jI >> jJ >> jK;
			inVec.insert( inVec.end() , {jX, jY, jZ, jVo, jI, jJ, jK});
		}
		infile.close();
		copy(inVec.begin(), inVec.end(), arr);
	}
	//broadcast input info to all jackets
	MPI_Bcast(&arr, 58, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//every process needs to make note of the total time, max force, and buzzys starting info
	totTime = (unsigned int) arr[0];
	maxF = arr[1];
	//populate buzzy starting info
	buzzy = {
		arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], 0, status
	};
	//populate own starting info
	jacket = {
		arr[7*rank + 2], arr[7*rank + 3], arr[7*rank + 4], arr[7*rank + 5], arr[7*rank + 6], arr[7*rank + 7], arr[7*rank + 8], 0, status
	};

	//loop through number of timesteps
	recArr = new double[56];
	for(int i = 0; i < totTime; ++i)
	{
		updateBuzzy();
		updateJacket(i, rank);
		getShipInfo(sendArr, rank, recArr);
		MPI_Allgather(&sendArr, 7, MPI_DOUBLE, recArr, 7, MPI_DOUBLE, MPI_COMM_WORLD);

		//update things from buzzy broadcast
		jacket.x = recArr[7*rank];
		jacket.y = recArr[7*rank + 1];
		jacket.z = recArr[7*rank + 2];
		jacket.status = recArr[7*rank + 6];
		//if all docked or destroyed -> break
		if (recArr[7*1 + 6] != 1 && recArr[7*2 + 6] != 1 && recArr[7*3 + 6] != 1
			&& recArr[7*4 + 6] != 1 && recArr[7*5 + 6] != 1 && recArr[7*6 + 6] != 1)
		{
			break;
		}
		//update each jacket from recArr
		if (rank == MASTER)
		{
			//print status of simulation
			for (int j = 1; j < 8; j++)
			{
				printf("%d,%d,%.6e,%.6e,%.6e,%.6e,%.6e,%.6e\n",
					j, (int) recArr[7*j + 6],
					recArr[7*j], recArr[7*j + 1], recArr[7*j + 2], recArr[7*j + 3], recArr[7*j + 4], recArr[7*j + 5]);
			}
		}
	}
	
	MPI_Finalize();

}
/*Function Header
Helper Function which takes in the send array by reference
Modifies the array to contain the necessary updated information*/
void getShipInfo(double * sendArr, int rank, double * recArr)
{
	sendArr[0] = jacket.x;
	sendArr[1] = jacket.y;
	sendArr[2] = jacket.z;
	sendArr[3] = jacket.i * jacket.f;
	sendArr[4] = jacket.j * jacket.f;
	sendArr[5] = jacket.k * jacket.f;
	//call update status here
	sendArr[6] = updateStatus(rank, recArr);
}

/*Function Header
Helper Function to update jacket struct containing each jacket and buzzy's own information
*/
void updateJacket(int t, int rank)
{
	if (rank != 0)
	{
		double tempForce = getNewForce();
		jacket.f = tempForce;
	}
	double accel = jacket.f / 10000.0; //a = f/m
	jacket.x += jacket.i * (jacket.v + accel * t);
	jacket.y += jacket.j * (jacket.v + accel * t);
	jacket.z += jacket.k * (jacket.v + accel * t);
	jacket.v += accel;
	//do not change i,j,k vectors to keep direction unchanged
}

/*Function Header
Helper Function to update struct containing buzzy's information to track for collisions or docking
*/
void updateBuzzy()
{
	buzzy.x += buzzy.i * buzzy.v;
	buzzy.y += buzzy.j * buzzy.v;
	buzzy.z += buzzy.k * buzzy.v;
}

/*Function Header
Helper Function to calculate the necessary force for each time interval
Since jackets will be drifitng, force will be 0 always
*/
double getNewForce()
{
	return 0.0;
}

/*Function Header
Helper Function to detetct collisions and docking
*/
double updateStatus(int rank, double * recArr)
{
	if (rank == 0)
	{
		return 1.0;
	}
	bool check1,check2,check3;
	check1 = jacket.status == 1;
	//check for docking
	double distance;

	//calculate necessary values
	distance = sqrt(pow(buzzy.x - jacket.x, 2) + pow(buzzy.y - jacket.y, 2) + pow(buzzy.z - jacket.z, 2));
	check2 = distance < 50.0;
	double dot = buzzy.x * jacket.x + buzzy.y * jacket.y + buzzy.z * jacket.z;
	double mag1, mag2;
	mag1 = sqrt(pow(buzzy.x, 2) + pow(buzzy.y, 2) + pow(buzzy.z, 2));
	mag2 = sqrt(pow(jacket.x, 2) + pow(jacket.y, 2) + pow(jacket.z, 2));
	double cosAngle = cos(dot/(mag1*mag2));
	//confirm docking
	if (check1 && check2 && (cosAngle > 0.8))
	{
		jacket.status = 2;
		jacket.x = buzzy.x;
		jacket.y = buzzy.y;
		jacket.z = buzzy.z;
		jacket.v = buzzy.v;
		jacket.i = buzzy.i;
		jacket.j = buzzy.j;
		jacket.k = buzzy.k;
		return 2.0;
	}
	//check for collisions
	check3 = false;
	double dist;
	for (int i = 1; i < 8; i++)
	{
		if (i != rank && jacket.status == 1 && recArr[7*i + 6] == 1.0)
		{
			dist = sqrt(pow(recArr[7*i] - jacket.x, 2) + pow(recArr[7*i + 1] - jacket.y, 2) + pow(recArr[7*i + 2] - jacket.z, 2));
			if (dist < 250.0)
			{
				check3 = true;
				recArr[7*i + 6] = 0;
			}
		}
	}
	if (check1 && (check3 || check2))
	{
		jacket.status = 0;
		jacket.v = 0;
		return 0;
	}
	//otherwise still active
	return 1.0;
}