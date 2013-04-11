// Copyright (c) 2012 Dougrist Productions
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Author: Benjamin Crist
//         Josh Douglas
// File: main.cc


#ifdef DEBUG

//#pragma comment (lib, "sfml-main-d.lib")
#pragma comment (lib, "sfml-system-d.lib")
#pragma comment (lib, "sfml-window-d.lib")
#pragma comment(lib, "opengl32.lib")

#else

//#pragma comment (lib, "sfml-main.lib")
#pragma comment (lib, "sfml-system.lib")
#pragma comment (lib, "sfml-window.lib")
#pragma comment(lib, "opengl32.lib")

#endif
#define PI 3.14159
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>

void testCamera();
void display();
void keyboard();

float dist = sqrt(1/3.0);
const int FPS = 27;
int bound = 100;
glm::vec3 eye = glm::vec3 (1, 0, 0);
glm::vec3 center = glm::vec3 (0, 0, 0);
glm::vec3 up = glm::vec3 (0, 1, 0);
float fovY = 60;
float aspectX = 1;
float zNear = 0.1;
float zFar = 5000;

void rotateCameraY(double rSpeed)
{
	glm::vec3 v = center - eye;

	center[0] = eye[0] + v[0] * cos(rSpeed) + v[2] * sin(rSpeed);
	center[2] = eye[2] + -v[0] * sin(rSpeed) + v[2] * cos(rSpeed);
}

//causes the camera to slide left or right without turning
void strafeCameraXZ(double mSpeed)
{
	glm::vec3 v = center - eye;

	glm::vec3 oV(-v.z, 0, v.x);

	eye[0] = eye[0] + oV[0] * mSpeed;
	eye[2] = eye[2] + oV[2] * mSpeed;

	center[0] = center[0] + oV[0] * mSpeed;
	center[2] = center[2] + oV[2] * mSpeed;
}

//moves the camera in a straight line in forward or reverse
void moveCameraXZ(double mSpeed)
{
	glm::vec3 v = center - eye;

	eye[0] = eye[0] + v[0] * mSpeed;
	eye[2] = eye[2] + v[2] * mSpeed;

	center[0] = center[0] + v[0] * mSpeed;
	center[2] = center[2] + v[2] * mSpeed;
}

void testCamera()
{
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glm::mat4 perspective = glm::perspective(fovY, aspectX, zNear, zFar);
	glLoadMatrixf(glm::value_ptr(perspective));
	//glLoadIdentity();
	//glOrtho(-bound, bound,-bound, bound,-bound, bound);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glMatrixMode(GL_MODELVIEW);
	std::cerr << eye.x << ',' << eye.z << '\t';
	std::cerr << center.x << ',' << center.z << std::endl;
	glm::mat4 isoViewMatrix = glm::lookAt(eye, center, up);
	glLoadMatrixf(glm::value_ptr(isoViewMatrix));

	float points[8][3] =
	{
		{-1, -1, 1}, {1, -1, 1}, {1, -1, -1}, {-1, -1, -1},
		{-1, 1, 1}, {1, 1, 1}, {1, 1, -1}, {-1, 1, -1}
	};

	float size = 10;
	glColor3f(1,0,1);
	glPushMatrix();
	glScalef(size,(size/8),size);

		glColor3f(1,1,1);
		//front
		glBegin(GL_QUADS);
			glVertex3fv(points[2]);
			glVertex3fv(points[3]);
			glVertex3fv(points[7]);
			glVertex3fv(points[6]);
		glEnd();


		glColor3f(1,1,0);
		//back
		glBegin(GL_QUADS);
			glVertex3fv(points[0]);
			glVertex3fv(points[1]);
			glVertex3fv(points[5]);
			glVertex3fv(points[4]);
		glEnd();
	
		glColor3f(1,0,1);
		//right
		glBegin(GL_QUADS);
			glVertex3fv(points[1]);
			glVertex3fv(points[2]);
			glVertex3fv(points[6]);
			glVertex3fv(points[5]);
		glEnd();

		glColor3f(0.5,1,0.5);
		//left
		glBegin(GL_QUADS);
			glVertex3fv(points[0]);
			glVertex3fv(points[3]);
			glVertex3fv(points[7]);
			glVertex3fv(points[4]);
		glEnd();

		glColor3f(0,1,1);
		//bottom
		glBegin(GL_QUADS);
			glVertex3fv(points[0]);
			glVertex3fv(points[1]);
			glVertex3fv(points[2]);
			glVertex3fv(points[3]);
		glEnd();

		glColor3f(0,1,0);
		//top
		glBegin(GL_QUADS);
			glVertex3fv(points[4]);
			glVertex3fv(points[5]);
			glVertex3fv(points[6]);
			glVertex3fv(points[7]);
		glEnd();

	glPopMatrix();
	glDisable(GL_DEPTH_TEST);

}

void keyboard()
{
	const double M_SPEED = 1.25;
	const double R_SPEED = 0.05;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		moveCameraXZ(M_SPEED);
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		 moveCameraXZ(-M_SPEED);
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		rotateCameraY(-R_SPEED);
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		rotateCameraY(R_SPEED);
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		 strafeCameraXZ(-M_SPEED);
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		strafeCameraXZ(M_SPEED);
	}

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		exit(0);
	}

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	testCamera();
}


int main()
{
  // create the window
    sf::Window window(sf::VideoMode(600, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);

	glClearColor(0,0,0,0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glm::mat4 perspective(float fovy, float aspect, float zNear,float zFar);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    // load resources, initialize the OpenGL states, ...

    // run the main loop
    bool running = true;
    while (running)
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw...
		display();
		keyboard();
        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    // release resources...

    return 0;
}