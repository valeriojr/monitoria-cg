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

// Variaveis para controlar a camera
float fov_y;
float cam_x, cam_y, cam_z;
float center_x, center_y, center_z;

// Variaveis para posicionar a parede
float parede_x, parede_y, parede_z; // posicao da parede
float parede_rotacao; // rotacao da parede
float parede_largura, parede_altura, parede_espessura; // dimensoes da parede

/*
Aqui faremos as operacoes de renderização
*/
void display()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Seleciona a matriz ModelView e reseta todas as transformacoes   
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
   
    // Define a matriz View
    gluLookAt(cam_x, cam_y, cam_z, center_x, center_y, center_z, 0.0f, 1.0f, 0.0f);
   
    // Define a matriz Model
    /*
    * Lembre-se, a ordem das transformacoes eh o inverso de
    * como aparecem no codigo, por isso aplicamos a escala, 
    * depois as rotacoes e finalmente a translacao
    */
	glTranslatef(parede_x, parede_y, parede_z);
	glRotatef(parede_rotacao, 0.0f, 1.0f, 0.0f); // rotacao em torno do eixo y
    // como o cubo tem lado 1 podemos escalar ele para as dimensoes desejadas
	glScalef(parede_largura, parede_altura, parede_espessura); 
	
	/*
	Desenha o cubo de lado 1 com todas as transformacoes acima
	aplicadas.
	A origem do cubo eh no centro, entao lembre de colocar
	altura/2 na coordenada y da posicao para ficar
	perfeitamente no chao
	*/
	glutSolidCube(1.0f);
   
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
	
	fov_y = 75.0f; // 75 graus
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov_y, 1.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.001f, 1000.0f);
    
	// Inicializando camera e parede
	
	cam_z = 5.0f; // 5m atrás da parede
	cam_y = 1.7f; // altura de uma pessoa 
	parede_altura = 3.0f; // 3m
	parede_espessura = 0.3f; // 30cm
	parede_largura = 4.0f; // 4m
	parede_rotacao = 45.0f; 
	parede_x = 0.0f;
	parede_y = 1.5f; // metade da altura
	parede_z = 0.0f;
	/*
	* center representa o ponto que estamos olhando,
    * nesse caso será o centro da parede
	*/
	center_x = parede_x;
	center_y = parede_y;
	center_z = parede_z;
	
    // Inicia o loop de eventos da GLUT
    glutMainLoop();
    
    return 0;
}