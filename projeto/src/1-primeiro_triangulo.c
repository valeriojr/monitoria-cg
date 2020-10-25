#include <GL/gl.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f
};

/*
Aqui faremos as operacoes de renderização
*/
void display()
{
	int i;
	
	glBegin(GL_TRIANGLES);
	for(i = 0;i < sizeof(vertices) / sizeof(float);i += 3)
	{
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
	}
	glEnd();
	
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
    /* 
    Primeiro iniicalizamos a glut, passando o endereço
    para o argc (quantidade de argumentos passados pela
    linha de comando) e o argv (a linha de comando em si)
    */
    glutInit(&argc, argv);
   
    /*
    Propriedades da janela. Cada constante representa um
    elemento de um \textit{bitfield}
    */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   
   
    // Define o tamanho da janela
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Cria a janela e o contexto OpenGL, com o nome "OpenGL"    
    glutCreateWindow("OpenGL");
    
    // Registra o callback de renderização (por enquanto não faz nada)
    glutDisplayFunc(display);
    
    /*
    Inicia o loop de eventos da janela. O fluxo do programa
    é passado para a GLUT. Qualquer código escrito após essa
    chamada só será executado quando a janela for fechada
    */
    glutMainLoop();
    
    return 0;
}