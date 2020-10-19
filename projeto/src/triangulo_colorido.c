#include <GL/gl.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

typedef struct Vertex
{
	float x, y, z;
	float r, g, b;
} Vertex;

Vertex vertices[] = {
	{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f}, // vermelho
	{ 0.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f}, // verde
	{ 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f}, // azul
};

/*
Aqui faremos as operacoes de renderização
*/
void display()
{
	int i;
	Vertex v;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
	
	for(i = 0;i < sizeof(vertices);i++)
	{
		v = vertices[i];
		glColor3f(v.r, v.g, v.b);
		glVertex3f(v.x, v.y, v.z);
	}
	glEnd();
	
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
    // Inicializacao
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL");
    
    // Registra o callback de renderização (por enquanto não faz nada)
    glutDisplayFunc(display);
    
    // Inicia o loop de eventos da GLUT
    glutMainLoop();
    
    return 0;
}