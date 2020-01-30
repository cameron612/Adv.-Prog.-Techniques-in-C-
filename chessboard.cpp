/*
Author:  Cameron Allotey
Date last modified: 11/18/2019
Organization: ECE 4122 Class

Description:

Draw a chess board incorporating basic OpenGL features

*/
#include <iostream>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <stdlib.h> // standard definitions

#define ESC 27

//function definitions
void changeSize(int w, int h);
void drawPawn();
void drawRook();
void drawKnight(int color);
void drawBishop();
void drawQueen();
void drawKing();
void update(void);
void renderScene(void);
void processNormalKeys(unsigned char key, int xx, int yy);

//----------------------------------------------------------------------
// Global variables
//
// The coordinate system is set up so that the (x,y)-coordinate plane
// is the ground, and the z-axis is directed upwards. The y-axis points
// to the north and the x-axis points to the east.
//
// The values (x,y,z) are the current camera position. The values (lx, ly)
// point in the direction the camera is looking. 
//----------------------------------------------------------------------

// Camera position
float x = 4.0, y = -5.0, z = 10.0; // initially here
float deltaMove = 0.0; // initially camera doesn't move

// Camera direction
float lx = 4.0, ly = 4.0; // camera points initially along y-axis

//light controls
int l0 = 1;
int l1 = 1;

//enhancement setting
int e = 0;

//rotation setting
int r = 0;
float angle = 0;

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
	gluPerspective(45.0, ratio, 0.1, 100.0); // perspective transformation
	glMatrixMode(GL_MODELVIEW); // return to modelview mode
	glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

/*Function Header
Draws a pawn centered in the desired square*/
void drawPawn()
{
	if (e == 0)
	{
		// Draw sphere
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.375);
		glutSolidSphere(0.375, 20, 20);
		glPopMatrix();
	}
	else
	{
		//sphere on top of cone
		glPushMatrix();
		glutSolidCone(0.375, 0.5, 20, 20);
		glTranslatef(0.0, 0.0, 0.5);
		glutSolidSphere(0.1, 20, 20);
		glPopMatrix();
	}
}

/*Function Header
Draws a rook centered in the desired square*/
void drawRook()
{
	if (e == 0)
	{
		// Draw body (a cube l=0.75)
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.3751);
		glutSolidCube(0.75);
		glPopMatrix();
	}
	else
	{
		//half sphere over cone
		glPushMatrix();
		glutSolidCone(0.375, 0.8, 20, 20);
		glTranslatef(0.0, 0.0, 0.5);
		glutSolidTorus(0.1, 0.3, 20, 20);
		glPopMatrix();
	}
}

/*Function Header
Draws a knight centered in the desired square*/
void drawKnight(int color)
{
	if (e == 0)
	{
		// Draw body (a teapot)
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.25);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glutSolidTeapot(0.25);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		//draw base
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.3, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.25, 20, 20);
		glTranslatef(0.0, 0.0, 0.3);
		//draw neck
		glPushMatrix();
		glScalef(1.0, 1.0, 1.5);
		glutSolidSphere(0.25, 20, 20);
		glPopMatrix();
		//draw head
		glPushMatrix();
		if (color == 0)
		{
			glTranslatef(0.0, 0.1, 0.35);
			glRotatef(-100.0, 1, 0, 0);
		}
		else
		{
			glTranslatef(0.0, -0.1, 0.35);
			glRotatef(100.0, 1, 0, 0);
		}
		glutSolidCone(0.2, 0.4, 20, 20);
		glPopMatrix();
		//draw mane
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.35);
		for (int i = 0; i < 9; ++i)
		{
			if (color == 0)
			{
				glRotatef(10.0, 1, 0, 0);
			}
			else
			{
				glRotatef(-10.0, 1, 0, 0);
			}
			glPushMatrix();
			glScalef(0.5, 0.5, 3.0);
			glutSolidCube(0.2);
			glPopMatrix();
		}
		glPopMatrix();
		glPopMatrix();

	}
}

/*Function Header
Draws a bishop centered in the desired square*/
void drawBishop()
{
	if (e == 0)
	{
		// Draw body (a cone)
		glPushMatrix();
		glutSolidCone(0.375, 1, 20, 20);
		glPopMatrix();
	}
	else
	{
		//add small 2nd cone and small sphere 
		glPushMatrix();
		glutSolidCone(0.25, 1.0, 20, 20);
		glTranslatef(0.0, 0.0, 0.85);
		glutSolidCone(0.12, 0.15, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidSphere(0.05, 20, 20);
		glPopMatrix();
	}
}

/*Function Header
Draws a queen centered in the desired square*/
void drawQueen()
{
	if (e == 0)
	{
		// Draw body (a tetrahedron)
		glPushMatrix();
		glTranslatef(0.0, -0.2, 0.485);
		glScalef(0.5, 0.5, 0.5);
		glRotatef(35.5, 0.0, 1.0, 0.0);
		glRotatef(33.0, 1.0, 0.0, 0.0);
		glutSolidTetrahedron();
		glPopMatrix();
	}
	else
	{
		//staked torous to create curve
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.3, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.25, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.2, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.15, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.2, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.25, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.25, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		//cone on top for queen
		glutSolidCone(0.25, 0.2, 20, 20);
		glPopMatrix();
	}
}

/*Function Header
Draws a king centered in the desired square*/
void drawKing()
{
	if (e == 0)
	{
		// Draw body (a octahedron)
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.4);
		glScalef(0.4, 0.4, 0.4);
		glutSolidOctahedron();
		glPopMatrix();
	}
	else
	{
		//staked torous to create curve
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.3, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.25, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.2, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.15, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.2, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.25, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glutSolidTorus(0.1, 0.25, 20, 20);
		glTranslatef(0.0, 0.0, 0.1);
		glScalef(0.15, 0.15, 0.15);
		//Dodecahedron on top for king
		glutSolidDodecahedron();
		glPopMatrix();
	}
}

//----------------------------------------------------------------------
// Update with each idle event
//
// This incrementally moves the camera and requests that the scene be
// redrawn.
//----------------------------------------------------------------------
void update(void)
{
	if (deltaMove) { // update camera position
		x += deltaMove * lx * 0.1;
		y += deltaMove * ly * 0.1;
	}
	glutPostRedisplay(); // redisplay everything
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene(void)
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	GLfloat light0_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light0_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light0_diffuse[] = { 0.0, 0.0, 0.0, 1.0 };

	GLfloat light1_position[] = { -5, -5, 8, 1};
	GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light1_specular[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light1_ambient[] = { 0.0, 0.0, 0.0, 1.0 };

	int i, j;

	// Clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//magenta bg color
	glClearColor(0.3, 0.1, 0.3, 1.0);
	// Reset transformations
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	
	

	// Set the camera centered at (x,y,1) and looking along directional
	// vector (lx, ly, 0), with the z-axis pointing up
	gluLookAt(
		x, y, z,
		lx, ly, 0.0,
		0.0, 0.0, 1.0);
	//update board angle if r key was pressed
	if (r == 1)
	{
		angle += 10;
		r = 0;
	}
	glPushMatrix();
	glTranslatef(4.0, 4.0, 0);
	glRotatef(angle, 0, 0, 1);
	glTranslatef(-4.0, -4.0, 0);

	//lighting settings and toggling for light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	if (l0 == 1)
	{
		glEnable(GL_LIGHT0);
	}
	else
	{
		glDisable(GL_LIGHT0);
	}
	//initialize light1
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	if (l1 == 1)
	{
		glEnable(GL_LIGHT1);
	}
	else
	{
		glDisable(GL_LIGHT1);
	}

	//material settings for lighting
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	// Draw ground - 8x8 checker pattern with no border
	for (float i = 0.0; i < 8.0; i++)
	{
		for (float j = 0.0; j < 8.0; j++)
		{
			if ((int) (i + j) % 2 == 0)
			{
				glColor3f(0.1, 0.1, 0.1); // set drawing color to black
			}
			else
			{
				glColor3ub(255, 255, 255); // set drawing color to white
			}
			glBegin(GL_POLYGON);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(j, i, 0.0);
			glVertex3f(j, i + 1.0, 0.0);
			glVertex3f(j + 1.0, i + 1.0, 0.0);
			glVertex3f(j + 1.0, i, 0.0);
			glEnd();
		}
	}
	
	// Draw pawns
	for (float i = 0; i < 2; i++)
	{
		for (float j = 0; j < 8; j++)
		{
			glPushMatrix();
			if (i == 1)
			{
				glTranslatef(j + 0.5, 1.5, 0);
				glColor3ub(140.0, 140.0, 135.0); // set drawing color to white
			}
			else
			{
				glTranslatef(j + 0.5, 6.5, 0);
				glColor3ub(150.0, 75.0, 0.0); // set drawing color to black
			}
			drawPawn();
			glPopMatrix();
		}
	}

	//Draw other pieces
	for (float i = 0; i < 2; i++)
	{
		for (float j = 0; j < 8; j++)
		{
			glPushMatrix();
			int color;
			if (i == 1)
			{
				glTranslatef(j + 0.5, 0.5, 0);
				glColor3ub(140.0, 140.0, 135.0); // set drawing color to white
				color = 0;
			}
			else
			{
				glTranslatef(j + 0.5, 7.5, 0);
				glColor3ub(150.0, 75.0, 0.0); // set drawing color to black
				color = 1;
			}
			switch ((int)j)
			{
			case 0:
				drawRook();
				break;
			case 1:
				drawKnight(color);
				break;
			case 2:
				drawBishop();
				break;
			case 3:
				drawQueen();
				break;
			case 4:
				drawKing();
				break;
			case 5:
				drawBishop();
				break;
			case 6:
				drawKnight(color);
				break;
			case 7:
				drawRook();
				break;
			default:
				break;
			}
			glPopMatrix();
		}
	}
	glPopMatrix();
	glutSwapBuffers(); // Make it all visible
	glFlush();
}

//----------------------------------------------------------------------
// User-input callbacks
//
// processNormalKeys: ESC, q, and Q cause program to exit
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q')
	{
		exit(0);
	}
	switch (key)
	{
	case 'd':
	case 'D':
		//move camera down
		z -= 0.25;
		break;
	case 'u':
	case 'U':
		//move camera down
		z += 0.25;
		break;
	case '0':
		//toggle l0
		l0 = (l0 == 1) ? 0 : 1;
		break;
	case '1':
		//toggle l1
		l1 = (l1 == 1) ? 0 : 1;
		break;
	case 'e':
	case 'E':
		//toggle enhanced graphics
		e = (e == 1) ? 0 : 1;
		break;
	case 'r':
	case 'R':
		//rotate board
		r = r == 0 ? 1 : 0;
		break;
	default:
		break;

	}
}

//----------------------------------------------------------------------
// Main program  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
int main(int argc, char** argv)
{
	// general initializations
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Chess!");

	// register callbacks
	glutReshapeFunc(changeSize); // window reshape callback
	glutDisplayFunc(renderScene); // (re)display callback
	glutIdleFunc(update); // incremental update
	glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
	glutKeyboardFunc(processNormalKeys); // process standard key clicks

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0; // this is just to keep the compiler happy
}
