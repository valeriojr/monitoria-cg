#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAX_VERTICES 100000

typedef struct Vec2 {
	float x, y;
} Vec2;

typedef struct Vec3 {
	float x, y, z;
} Vec3;


// Mesh data
int VERTEX_COUNT;
Vec3 VERTICES[MAX_VERTICES];
Vec3 NORMALS[MAX_VERTICES];
Vec2 TEX_COORDS[MAX_VERTICES];


void display();


void idle() {
	glutPostRedisplay();
}


int load_obj(const char* path);


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("11 - Carregar .obj");
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0f, 640.0f / 480.0f, 10e-3, 10e3);

	if(!load_obj("../capsule.obj")) {
		perror("Erro ao abrir o arquivo");
		return -1;
	}
	
	unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load("../imagens/piso.jpg", &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        perror("Texture failed to load");
        stbi_image_free(data);
    }

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
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	
	int i;
	glBegin(GL_TRIANGLES);
	for(i = 0;i < VERTEX_COUNT;i++){
		glNormal3f(NORMALS[i].x, NORMALS[i].y, NORMALS[i].z);
		glTexCoord2f(TEX_COORDS[i].x, TEX_COORDS[i].y);
		glVertex3f(VERTICES[i].x, VERTICES[i].y, VERTICES[i].z);
	}
	glEnd();

	glutSwapBuffers();
}


int load_obj(const char* path) {
	FILE* fp = fopen(path, "r");

	if(!fp)
		return 0;

	char buffer[512] = "";
	int vertex_count = 0;
	int normal_count = 0;
	int tex_coord_count = 0;
	Vec3 vertices[MAX_VERTICES];
	Vec3 normals[MAX_VERTICES];
	Vec2 tex_coords[MAX_VERTICES];

	while(fgets(buffer, 512, fp)){
		if(buffer[0] == '#') // Comment
			continue;
		
		char* token = strtok(buffer, " ");
	
		if(strcmp(token, "v") == 0){
			// Parse vertex
			vertices[vertex_count].x = atof(strtok(NULL, " "));
			vertices[vertex_count].y = atof(strtok(NULL, " "));
			vertices[vertex_count].z = atof(strtok(NULL, " "));
			vertex_count++;
		}
		else if(strcmp(token, "vn") == 0) {
			// Parse normal vector
			normals[normal_count].x = atof(strtok(NULL, " "));
			normals[normal_count].y = atof(strtok(NULL, " "));
			normals[normal_count].z = atof(strtok(NULL, " "));
			normal_count++;
		}
		else if(strcmp(token, "vt") == 0) {
			// Parse texture coordinate
			tex_coords[tex_coord_count].x = atof(strtok(NULL, " "));
			tex_coords[tex_coord_count].y = -atof(strtok(NULL, " "));
			tex_coord_count++;
		}
		else if(strcmp(token, "f") == 0) {
			// Parse face
			int i;
			for(i = 0;i < 3;i++){
				VERTICES[VERTEX_COUNT] = vertices[atoi(strtok(NULL, "/")) - 1];
				TEX_COORDS[VERTEX_COUNT] = tex_coords[atoi(strtok(NULL, "/")) - 1];
				NORMALS[VERTEX_COUNT] = normals[atoi(strtok(NULL, " ")) - 1];
				VERTEX_COUNT++;
			}
		}
	}

	fclose(fp);

	return 1;
}
