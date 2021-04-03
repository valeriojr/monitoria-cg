#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


int wire = 1;


void glutSolidCylinder(float radius, float height, int sectors) {
	int i;
	float theta = 2.0f * M_PI / sectors;
	float top = height / 2.0f;
	float bottom = -top;
	
	glBegin(GL_TRIANGLES);
	
	for(i = 0;i < sectors;i++) {
		float c0 = cos(i * theta);
		float s0 = sin(i * theta);
		float c1 = cos((i + 1) * theta);
		float s1 = sin((i + 1) * theta);
		
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f,        top,    0.0f);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(radius * c0, top,    radius * s0);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(radius * c1, top,    radius * s1);
	
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(0.0f,        bottom, 0.0f);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(radius * c1, bottom, radius * s1);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(radius * c0, bottom, radius * s0);
	
		glNormal3f(c0, 0.0f, s0);
		glVertex3f(radius * c0, bottom, radius * s0);
		glNormal3f(c1, 0.0f, s1);
		glVertex3f(radius * c1, top,    radius * s1);
		glNormal3f(c0, 0.0f, s0);
		glVertex3f(radius * c0, top,    radius * s0);
		
		glNormal3f(c1, 0.0f, s1);
		glVertex3f(radius * c1, top,    radius * s1);
		glNormal3f(c0, 0.0f, s0);
		glVertex3f(radius * c0, bottom, radius * s0);
		glNormal3f(c1, 0.0f, s1);
		glVertex3f(radius * c1, bottom, radius * s1);
	}

	glEnd();
}


void glutWireCylinder(float radius, float height, int sectors) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glutSolidCylinder(radius, height, sectors);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// View matrix
	gluLookAt(5.0f, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Model matrix
	float t = (float) glutGet(GLUT_ELAPSED_TIME) / 10e3;
	glRotatef(360.0f * t, 0.0f, 1.0f, 0.0f);

	if(!wire)
		glutSolidCylinder(1.0f, 2.0f, 5);
	else
		glutWireCylinder(1.0f, 2.0f, 5);

	glutSwapBuffers();
	glutPostRedisplay();
}


void keyboard(char key, int x, int y) {
	if(key == 'w' || key == 'W')
		wire = !wire;
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Cylinder");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(75.0f, (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 10e-3, 10e3);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}
