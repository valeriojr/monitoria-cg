#include <GL/gl.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

/*
Aqui faremos as operacoes de renderizacao
*/
void display()
{
}

int main(int argc, char** argv)
{
    // Inicializacao
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("OpenGL");
    
    // Registra o callback de renderizacao (por enquanto nao faz nada)
    glutDisplayFunc(display);
    
    // Inicia o loop de eventos da GLUT
    glutMainLoop();
    
    return 0;
}