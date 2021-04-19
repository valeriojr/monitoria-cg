#include <math.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "perlin_noise.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


void display();

void idle();

void glutSolidFunction(float domain_start, float domain_end, int samples, float (*f)(float, float));

void setup_lighting();

float sea(float x, float z);


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("10 - Agua");

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	setup_lighting();

	glMatrixMode(GL_PROJECTION);
	gluPerspective(75.0f, (float) WINDOW_WIDTH / WINDOW_HEIGHT, 10e-3, 10e3);

	glutMainLoop();
	
	return 0;
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(10.0f, 5.0f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	glColor3f(0.0f, 0.2f, 8.0f);
	glutSolidFunction(-50.0f, 50.0f, 50, sea);

	glutSwapBuffers();
}


void idle() {
	glutPostRedisplay();
}


void glutSolidFunction(float domain_start, float domain_end, int samples, float (*f)(float, float)) {
	int i, j;
	float step = (domain_end - domain_start) / samples;

	glBegin(GL_TRIANGLES);
	for(i = 0;i < samples;i++){
		for(j = 0;j < samples;j++) {
			float i0 = domain_start + i * step;
			float j0 = domain_start + j * step;
			float i1 = i0 + step;
			float j1 = j0 + step;

			float fi0j0 = f(i0, j0);
			float fi1j0 = f(i1, j0);
			float fi0j1 = f(i0, j1);
			float fi1j1 = f(i1, j1);
	
			/*
			0 (i0, fi0j0, j0)      1 (i0, fi0j1, j1)
			 o--------------------o
			 |                _-¨ |
			 |             _-¨    |
			 |          _-¨       |
			 |       _-¨          |
			 |    _-¨             |
			 | _-¨                |
			 o--------------------o
			2 (i1, fi1j0, j0)      3 (i1, fi1j1, j1)
			*/
			float v[][3] = {
				{i0, fi0j0, j0}, // 0
				{i1, fi1j0, j0}, // 2
				{i0, fi0j1, j1}, // 1
				{i0, fi0j1, j1}, // 1
				{i1, fi1j0, j0}, // 2
				{i1, fi1j1, j1}  // 3
			};

			// Calculate face normal using cross product
			float a1, a2, a3; // v[0] - v[1]
			float b1, b2, b3; // v[2] - v[1]
			float nx, ny, nz; // a x b
			a1 = v[0][0] - v[2][0];
	 		a2 = v[0][1] - v[2][1];
			a3 = v[0][2] - v[2][2];
			b1 = v[1][0] - v[2][0];
			b2 = v[1][1] - v[2][1];
			b3 = v[1][2] - v[2][2];
			nx = a2 * b3 - a3 * b2;
			ny = a3 * b1 - a1 * b3;
			nz = a1 * b2 - a2 * b1;
			glNormal3f(nx, ny, nz);
			
			int idx;
			for(idx = 0;idx < 3;idx++){
				glVertex3f(v[idx][0], v[idx][1], v[idx][2]);
			}
			
			a1 = v[1][0] - v[2][0];
	 		a2 = v[1][1] - v[2][1];
			a3 = v[1][2] - v[2][2];
			b1 = v[5][0] - v[2][0];
			b2 = v[5][1] - v[2][1];
			b3 = v[5][2] - v[2][2];
			nx = a2 * b3 - a3 * b2;
			ny = a3 * b1 - a1 * b3;
			nz = a1 * b2 - a2 * b1;
			glNormal3f(nx, ny, nz);
			
			for(idx = 3; idx < 6;idx++){
				glVertex3f(v[idx][0], v[idx][1], v[idx][2]);
			}
		}
	}
	glEnd();
}


void setup_lighting()
{  
	float mat_specular[] = {1.0f, 1.0f, 1.0f};
	float mat_shininess[] = {50.0f};
	float light_ambient[] = {0.2f, 0.2f, 0.2f};
	float light_position[] = {0.0f, -1.0f, 0.0f, 0.0f};
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


float sea(float x, float z) {
	float t = 100.0f * (float) glutGet(GLUT_ELAPSED_TIME) / 10e3;
	float noise = perlin2d(x, z, 0.0f, 4);
	return 0.5f * (sin(x + noise + t) + cos(z + noise + t));
}
