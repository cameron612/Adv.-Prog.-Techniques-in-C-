/*
Author:  Cameron Allotey
Date last modified: 12/3/2019
Organization: ECE 4122 Class

Description:

Buzzy Bowl! Drones perform a halftime swarm using OpenMPI and OpenGL

*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "iomanip"
#include <cmath>
#include <math.h>
#include <cstdlib>
#include <GL/glut.h>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include "ECE_Bitmap.h"

#define ESC 27

//function definitions
void displayFootballField();
void drawUAVs(double x, double y, double z);
void processNormalKeys(unsigned char key, int xx, int yy);
void timerFunction(int id);
void redraw();
void CalcualteUAVsLocation(int rank);
void setForce(int rank, double distance);
void update(int rank);
void setUpForce(int rank);
void checkCollisions(int rank);
///////////////////
//GLOBAL VARIABLES
///////////////////
// Send location and velocity vector in each direction
const int numElements = 6; // x, y, z, vx, vy, vz

const int rcvSize = 16 * 6; // (Main task + 15 UAVs) * numElements

double* rcvbuffer = new double[rcvSize];

double sendBuffer[numElements];

//set openGL camera values
double eye_x = 0.0, eye_y = 55.0, eye_z = 47.0;
double center_x = 0.0, center_y = 0.0, center_z = 25.0;

//coordinates to update view, initalized according to field positions
double xcoords[15] = { -39.5,-19.7,0,19.7,39.5,-39.5,-19.7,0,19.7,39.5,-39.5,-19.7,0,19.7,39.5 };
double ycoords[15] = { -22.2,0,22.2,-22.2,0,22.2,-22.2,0,22.2,-22.2,0,22.2,-22.2,0,22.2 };
double zcoords[15] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

//determines when to start swarming
bool phase1 = true;

//center of sphere
double target[] = { 0,0,50.0 };
double reference1[] = { 1,0,0 };
double reference2[] = { 0,0,1 };
double radius = 10.0;

//physics constants
double circx, circy, circz, theta = 0, phi;

double gravDir[] = { 0,0,-1 };
double gravMag = 10.0;

//struct to hold uav information
struct uav
{
	double x, y, z, v, i, j, k, f, fi, fj, fk;
};

//array to hold information of all uavs on each process
std::vector<struct uav> uavs;

//openGL texture values
GLuint texture[2];

struct Image {

	unsigned long sizeX;

	unsigned long sizeY;

	char* data;

};

typedef struct Image Image;

BMP inBitmap;


//----------------------------------------------------------------------
// Reshape callback
//
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//
//----------------------------------------------------------------------
void changeSize(int w, int h)
{
	float ratio = ((float)w) / ((float)h); // window aspect ratio
	glMatrixMode(GL_PROJECTION); // projection matrix is active
	glLoadIdentity(); // reset the projection
	gluPerspective(90.0, ratio, 0.1, 1000.0); // perspective transformation
	glMatrixMode(GL_MODELVIEW); // return to modelview mode
	glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene()
{

	// Clear color and depth buffers
	glClearColor(1.0, 1.0, 1.0, 1.0); // background color to green??
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	//std::cout << eye_z << std::endl;
	gluLookAt(eye_x, eye_y, eye_z,
		center_x, center_y, center_z,
		0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	//apply texture to field
	glEnable(GL_TEXTURE_2D);
	displayFootballField();
	glDisable(GL_TEXTURE_2D);

	//draw each uav according to global coordinates
	for (int i = 0; i < 15; i++)
	{
		drawUAVs(xcoords[i], ycoords[i], zcoords[i]);
	}
	//draw sphere as reference for swarming
	glPushMatrix();
	glColor3ub(255, 255, 255); // set drawing color to white
	glTranslated(0.0, 0.0, 50.0);
	glutWireSphere(10.0, 10, 10);
	glPopMatrix();

	glutSwapBuffers(); // Make it all visible

	MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvbuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);
	redraw();
}

//----------------------------------------------------------------------
// Function to update coordinates in preparation for next scene rendered
// Update corrdinates after allgather
//----------------------------------------------------------------------
void redraw()
{
	for (int i = 0; i < 16; i++)
	{
		if (i != 0)
		{
			xcoords[i - 1] = rcvbuffer[6 * i];
			ycoords[i - 1] = rcvbuffer[6 * i + 1];
			zcoords[i - 1] = rcvbuffer[6 * i + 2];
		}
	}
}

//----------------------------------------------------------------------
// Function to draw polygon representing field
// Additionally applies textures
//----------------------------------------------------------------------
void displayFootballField()
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glColor3ub(0, 255, 0); // set drawing color to green
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-50.0, -25.0, 0.0);
	glTexCoord2f(1, 0);
	glVertex3f(-50.0, 25.0, 0.0);
	glTexCoord2f(0, 0);
	glVertex3f(50.0, 25.0, 0.0);
	glTexCoord2f(0, 1);
	glVertex3f(50.0, -25.0, 0.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glPopMatrix();
}

//----------------------------------------------------------------------
// Function to draw shapes representing drones
// Initials: CA -> C: Cone A: red
//----------------------------------------------------------------------
void drawUAVs(double x, double y, double z)
{
	glColor3ub(255.0, 0.0, 0.0); // set drawing color to red
	glPushMatrix();
	glTranslated(x, y, z);
	glutSolidCone(0.5, 1, 20, 20);
	glPopMatrix();
}
//----------------------------------------------------------------------
// mainOpenGL  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
void timer(int id)
{
	glutPostRedisplay();
	glutTimerFunc(100, timer, 0);
}
//----------------------------------------------------------------------
// mainOpenGL  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
void mainOpenGL(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);

	glutCreateWindow(argv[0]);

	inBitmap.read("ff.bmp");

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Create Textures

	glGenTextures(2, texture);

	// Setup first texture
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, inBitmap.bmp_info_header.width, inBitmap.bmp_info_header.height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, &inBitmap.data[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// Do the second texture
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	//additional openGL inits
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);

	glutIgnoreKeyRepeat(0); // ignore key repeat when holding key down
	glutKeyboardFunc(processNormalKeys); // process standard key clicks


	glutTimerFunc(100, timerFunction, 0);
	glutMainLoop();
}

//----------------------------------------------------------------------
// User-input callbacks
//
// processNormalKeys: ESC, q, and Q cause program to exit
// wasd and 1,2 are used to move camera for debugging
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q')
	{
		exit(0);
	}
	switch (key)
	{
	case 'w':
	case 'W':
		//move camera forward
		eye_y -= 1.0;
		glutPostRedisplay(); // redisplay everything
		break;
	case 'a':
	case 'A':
		//move camera left
		eye_x += 1.0;
		glutPostRedisplay(); // redisplay everything
		break;
	case 's':
	case 'S':
		//move camera backward
		eye_y += 1.0;
		glutPostRedisplay(); // redisplay everything
		break;
	case 'd':
	case 'D':
		//move camera right
		eye_x -= 1.0;
		glutPostRedisplay(); // redisplay everything
		break;
	case '1':
		//move camera up
		eye_z += 1.0;
		glutPostRedisplay(); // redisplay everything
		break;
	case '2':
		//move camera down
		eye_z -= 1.0;
		glutPostRedisplay(); // redisplay everything
		break;
	default:
		break;

	}
}

//----------------------------------------------------------------------
// Function to calculate changes in UAV location each loop
// Broken into inital then secondary phases
//----------------------------------------------------------------------
void CalcualteUAVsLocation(int rank)
{
	int idx = rank - 1;
	if (phase1) //fly to center
	{
		//calculate necessary values
		double distance = sqrt(pow(uavs[idx].x - target[0], 2) + pow(uavs[idx].y - target[1], 2) + pow(uavs[idx].z - target[2], 2));
		phase1 = distance > 10.0;
		if (uavs[idx].f == 0)
		{
			//intially set force towards sphere
			setForce(rank, distance);
			update(rank);
		}
		else
		{
			//"drift" to sphere by only counteracting gravity
			setUpForce(rank);
			update(rank);
		}
		//populate buffer for MPI
		sendBuffer[0] = uavs[idx].x;
		sendBuffer[1] = uavs[idx].y;
		sendBuffer[2] = uavs[idx].z;
		sendBuffer[3] = uavs[idx].i * uavs[idx].v;
		sendBuffer[4] = uavs[idx].j * uavs[idx].v;
		sendBuffer[5] = uavs[idx].k * uavs[idx].v;
	}
	else //rotate around center
	{
		//check for collisions once randomness is introduced
		checkCollisions(rank);
		update(rank);
		sendBuffer[0] = uavs[idx].x;
		sendBuffer[1] = uavs[idx].y;
		sendBuffer[2] = uavs[idx].z;
		sendBuffer[3] = uavs[idx].i * uavs[idx].v;
		sendBuffer[4] = uavs[idx].j * uavs[idx].v;
		sendBuffer[5] = uavs[idx].k * uavs[idx].v;
	}
}

//----------------------------------------------------------------------
// Function to update uav vector with changes to x,y,z, and v after calc
// Broken into phases
//----------------------------------------------------------------------
void update(int rank)
{
	if (phase1)
	{
		int idx = rank - 1;
		double m = 1.0; //mass
		//account for gravity
		double fx = uavs[idx].f * uavs[idx].fi + gravMag * gravDir[0];
		double fy = uavs[idx].f * uavs[idx].fj + gravMag * gravDir[1];
		double fz = uavs[idx].f * uavs[idx].fk + gravMag * gravDir[2];
		double fmag = sqrt(pow(fx, 2) + pow(fy, 2) + pow(fz, 2));
		double fuv[3] = { fx / fmag, fy / fmag, fz / fmag };
		//fdir should match vector set previously

		//f = a bc m = 1kg
		double t = 1.0;
		uavs[idx].v = uavs[idx].v + fmag * t;
		uavs[idx].x = uavs[idx].x + uavs[idx].i * (uavs[idx].v * t + 0.5 * fmag * pow(t, 2));
		uavs[idx].y = uavs[idx].y + uavs[idx].j * (uavs[idx].v * t + 0.5 * fmag * pow(t, 2));
		uavs[idx].z = uavs[idx].z + uavs[idx].k * (uavs[idx].v * t + 0.5 * fmag * pow(t, 2));
	}
	else
	{
		int idx = rank - 1;
		theta += 1.0; //change theta by 1 each iteration

		//virtual target points for circular motion
		circx = target[0] + radius * sin(theta) * cos(phi);
		circy = target[1] + radius * sin(theta) * sin(phi);
		circz = target[2] + radius * cos(theta);

		double m = 1.0; //mass
		double dx = circx - uavs[idx].x;
		double dy = circy - uavs[idx].y;
		double dz = circz - uavs[idx].z;
		double mag = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
		double dir[3] = { dx / mag, dy / mag, dz / mag };

		//hookes law
		double hdx = target[0] - uavs[idx].x;
		double hdy = target[1] - uavs[idx].y;
		double hdz = target[2] - uavs[idx].z;
		double hfmag = sqrt(pow(hdx, 2) + pow(hdy, 2) + pow(hdz, 2));
		double hf = 0.8 * (10 - hfmag);
		double hfuv[3] = { hdx / hfmag, hdy / hfmag, hdz / hfmag };

		//total force and calculation
		double fx = 100 * dir[0] + gravMag * gravDir[0] + hf * hfuv[0];
		double fy = 100 * dir[1] + gravMag * gravDir[1] + hf * hfuv[1];
		double fz = 10 + 100 * dir[2] + gravMag * gravDir[2] + hf * hfuv[2];
		double fmag = sqrt(pow(fx, 2) + pow(fy, 2) + pow(fz, 2));
		//force limit
		if (fmag > 20)
		{
			fmag = 15;
		}
		double fuv[3] = { fx / fmag, fy / fmag, fz / fmag };
		//f = a because m = 1kg
		double t = 0.1;
		uavs[idx].v = uavs[idx].v + fmag * t;
		//speed limit
		if (uavs[idx].v > 12)
		{
			uavs[idx].v = 5;
		}
		uavs[idx].x = uavs[idx].x + fuv[0] * (uavs[idx].v * t + 0.5 * fmag * pow(t, 2));
		uavs[idx].y = uavs[idx].y + fuv[1] * (uavs[idx].v * t + 0.5 * fmag * pow(t, 2));
		uavs[idx].z = uavs[idx].z + fuv[2] * (uavs[idx].v * t + 0.5 * fmag * pow(t, 2));
	}
}

//----------------------------------------------------------------------
// Function to check for UAV collisions and update velocity if so
//----------------------------------------------------------------------
void checkCollisions(int rank)
{
	double dist, vx, vy, vz;
	int idx = rank - 1;
	for (int i = 0; i < 15; i++)
	{
		dist = sqrt(pow(uavs[idx].x - rcvbuffer[6*(i+1)], 2) + pow(uavs[idx].y - rcvbuffer[6 * (i + 1)+1], 2) + pow(uavs[idx].z - rcvbuffer[6 * (i + 1)+2], 2));
		if (i != idx && dist < 1)
		{
			vx = rcvbuffer[6 * (i + 1) + 3];
			vy = rcvbuffer[6 * (i + 1) + 4];
			vz = rcvbuffer[6 * (i + 1) + 5];
			uavs[idx].v = sqrt(pow(vx, 2) + pow(vy, 2) + pow(vz, 2));
			uavs[idx].i = vx / uavs[idx].v;
			uavs[idx].j = vy / uavs[idx].v;
			uavs[idx].k = vz / uavs[idx].v;
		}
	}
}

//----------------------------------------------------------------------
// Function to set force towards sphere from ground
//----------------------------------------------------------------------
void setForce(int rank, double distance)
{
	int idx = rank - 1;
	//APPLIED force (capped at 20N)
	double m = 1.0; //mass
	double fx = (2.0 * m * (target[0] - uavs[idx].x)) / 900.0;
	double fy = (2.0 * m * (target[1] - uavs[idx].y)) / 900.0;
	double fz = 10.0 + (2.0 * m * (target[2] - uavs[idx].z)) / 900.0;
	double fmag = sqrt(pow(fx, 2) + pow(fy, 2) + pow(fz, 2));
	double fuv[3] = { fx / fmag, fy / fmag, fz / fmag };
	uavs[idx].f = fmag;
	uavs[idx].fi = fuv[0];
	uavs[idx].fj = fuv[1];
	uavs[idx].fk = fuv[2];
}

//----------------------------------------------------------------------
// Function to counteract force of gravity
//----------------------------------------------------------------------
void setUpForce(int rank)
{
	int idx = rank - 1;
	double m = 1.0; //mass
	double fx = 0;
	double fy = 0;
	double fz = 10.0;
	double fmag = sqrt(pow(fx, 2) + pow(fy, 2) + pow(fz, 2));
	double fuv[3] = { fx / fmag, fy / fmag, fz / fmag };
	uavs[idx].f = fmag;
	uavs[idx].fi = fuv[0];
	uavs[idx].fj = fuv[1];
	uavs[idx].fk = fuv[2];
}

//----------------------------------------------------------------------
// timerFunction  - called whenever the timer fires
//----------------------------------------------------------------------
void timerFunction(int id)
{
	glutPostRedisplay();
	glutTimerFunc(100, timerFunction, 0);
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Main entry point determines rank of the process and follows the 
// correct program path
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)

{
	int numTasks, rank;

	int rc = MPI_Init(&argc, &argv);

	if (rc != MPI_SUCCESS)
	{
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int gsize = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &gsize);

	srand(rank);
	if (rank == 0)
	{
		mainOpenGL(argc, argv);
	}
	else
	{
		//populate inital info
		for (int i = 0; i < 15; i++)
		{
			struct uav temp;
			temp.x = xcoords[rank - 1];
			temp.y = ycoords[rank - 1];
			temp.z = zcoords[rank - 1];
			temp.v = 0;
			temp.i = 0;
			temp.j = 0;
			temp.k = 0;
			temp.f = 0;
			temp.fi = 0;
			temp.fj = 0;
			temp.fk = 0;
			uavs.push_back(temp);
		}
		double vec[3] = { target[0] - uavs[rank - 1].x, target[1] - uavs[rank - 1].y, target[2] - uavs[rank - 1].z };
		double mag = sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2));
		double uv[3] = { vec[0] / mag, vec[1] / mag, vec[2] / mag };
		uavs[rank - 1].i = uv[0];
		uavs[rank - 1].j = uv[1];
		uavs[rank - 1].k = uv[2];
		// Sleep for 5 seconds
		std::this_thread::sleep_for(std::chrono::seconds(5));
		for (int ii = 0; ii < 475; ii++)
		{
			CalcualteUAVsLocation(rank);
			MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvbuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);
		}

		//rotation phase
		circx = uavs[rank - 1].x;
		circy = uavs[rank - 1].y;
		circz = uavs[rank - 1].z;
		phi = rand() % 360;
		for (int ii = 0; ii < 600; ii++)
		{
			CalcualteUAVsLocation(rank);
			MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvbuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
	return 0;
}
