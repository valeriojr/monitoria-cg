#include <GL/gl.h>
#include <GL/freeglut.h>


float shininess = 0.0f;


void display();


void mouse(int button, int state, int x, int y) {
	if(button == 3 || button == 4){
		shininess += 1.0f;
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	}
}


int main(int argc, char** argv) {
	// Inicializacao
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("11 - Carregar .obj");

	// Registrando callbacks
	glutDisplayFunc(display);
	glutIdleFunc(glutPostRedisplay);
	glutMouseFunc(mouse);

	// Configurando OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	// glEnable(GL_COLOR_MATERIAL);

	// glColor3f(0.2f, 0.6f, 0.1f);

	float ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
	// ambiente 0, 0, 0, 1
	// posicao  0, 0, 1, 1
	// difusa 1, 1, 1, 1
	// especular 0, 0, 0, 0
	float light_ambient[] = {0.1f, 0.1f, 0.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	float light_position[] = {1.0f, 0.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	float light_diffuse[] = {1.0f, 0.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

	float light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);	


	float material_diffuse[] = {0.2f, 0.6f, 0.1f, 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	
	float material_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);	


	// Projeção perspectiva
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0f, 640.0f / 480.0f, 10e-3, 10e3);

	glutMainLoop();
	
	return 0;
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// View matrix
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Model matrix
	float t = 1.0f * glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	glRotatef(10.0f * t, 0.0f, 1.0f, 0.0f);

	glutSolidTeapot(1.0f);

	glutSwapBuffers();
}
