#include <math.h>
#include <ctype.h>

#include <GL/gl.h>
#include <GL/freeglut.h>


// Colors

#define BLACK 0.0f, 0.0f, 0.0f
#define RED 1.0f, 0.0f, 0.0f
#define GREEN 0.0f, 1.0f, 0.0f
#define BLUE 0.0f, 0.0f, 1.0f
#define WHITE 1.0f, 1.0f, 1.0f
#define YELLOW 1.0f, 1.0f, 0.0f
#define GROUND_COLOR 0.83f, 0.40f, 0.16f


// Math

#define ORIGIN 0.0f, 0.0f, 0.0f
#define RIGHT 1.0f, 0.0f, 0.0f
#define UP 0.0f, 1.0f, 0.0f
#define FORWARD 0.0f, 0.0f, -1.0f


// Game constants

#define MAX_BULLETS 10
#define BULLET_SPEED 100.0f


// Data types

struct Vec2 {
	float x, y;
};

typedef struct Vec2 Vec2;

struct Vec3 {
	float x, y, z;
};

typedef struct Vec3 Vec3;

struct Transform {
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
};

typedef struct Transform Transform;


// Constants

const float DEG2RAD = M_PI / 180.0f;
const float RAD2DEG = 180.0f / M_PI;


// Global variables

Vec2 WINDOW_SIZE = {640, 480};
Vec2 WINDOW_CENTER = {320, 240};
float FOVY = 75.0f;
float ZNEAR = 10e-3;
float ZFAR = 10e3;
Transform CAM;
int KEYBOARD[128] = {0};
Vec2 MOTION;
Transform PLAYER;
float T;
float DT;
float PLAYER_SPEED = 10.0f;
Transform BULLETS[MAX_BULLETS];
int ACTIVE_BULLETS = 0;
float GUN_RECOIL;


void init_gl();


// Callbacks

void display();

void idle();

void motion(int x, int y);

void mouse(int button, int state, int x, int y);

void keyboard(unsigned char key, int x, int y);

void keyboard_up(unsigned char key, int x, int y);

void reshape(int width, int height);


// Drawing utils

void draw_axis(int x, int y, int z);

void draw_grid(int n);


// Math utils

Vec3 forward(Transform* t);

Vec3 right(Transform* t);

Vec3 up(Transform* t);


// Characters

void draw_snow_white() {
	// Body
	glRotatef(-90.0f, RIGHT);
	glColor3f(YELLOW);
	glutSolidCone(0.5f, 1.0f, 10, 10);
	glTranslatef(0.0f, 0.0f, 0.5f);
	glColor3f(BLUE);
	glutSolidSphere(0.3f, 10, 10);

	// Gun
	glPushMatrix();
		glColor3f(BLACK);
		
		// Animate recoil
		glTranslatef(0.0f, GUN_RECOIL, 0.0f);	

		glTranslatef(0.3f, 0.0f, 0.0f);
		glPushMatrix();
			glScalef(0.05f, 0.5f, 0.1f);
			glutSolidCube(1.0f);
		glPopMatrix();
	
		glTranslatef(0.0f, -0.3f, 0.0f);
		glPushMatrix();
			glScalef(0.05f, 0.5f, 0.05f);
			glutSolidCube(1.0f);
		glPopMatrix();
	glPopMatrix();
	
	// Head
	glTranslatef(0.0f, 0.0f, 0.5f);
	glColor3f(1.0f, 1.0f, 0.75f);
	glutSolidSphere(0.25f, 10, 10);

	// Hair
	glTranslatef(0.0f, 0.1f, 0.0f);
	glColor3f(BLACK);
	glutSolidSphere(0.3f, 10, 10);

	glColor3f(RED);
	glPushMatrix();
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glutSolidTorus(0.15f, 0.2f, 10, 10);
	glPopMatrix();

	glColor3f(BLACK);
	glPushMatrix();
		glTranslatef(0.1f, 0.1f, -0.25f);
		glutSolidSphere(0.15f, 10, 10);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-0.1f, 0.1f, -0.25f);
		glutSolidSphere(0.15f, 10, 10);
	glPopMatrix();
}

void draw_bullet() {
	glColor3f(BLACK);
	glutSolidSphere(0.15f, 5, 5);
}

void draw_ground() {
	glPushMatrix();
		glScalef(20.0f, 0, 20.0f);
		glBegin(GL_QUADS);
			glColor3f(GROUND_COLOR);
			glNormal3f(0.0f, 0.0f, 0.0f);
	
			glVertex3f(-0.5f, 0.0f, 0.5f);
			glVertex3f(0.5f, 0.0f, 0.5f);
			glVertex3f(0.5f, 0.0f, -0.5f);
			glVertex3f(-0.5f, 0.0f, -0.5f);
		glEnd();
	glPopMatrix();
}

void shoot() {
	static int current_bullet = 0;
	ACTIVE_BULLETS++;
	

	if(ACTIVE_BULLETS > MAX_BULLETS) {
		ACTIVE_BULLETS = MAX_BULLETS;
	}

	if(current_bullet > MAX_BULLETS - 1){
		current_bullet = 0;
	}
	
	Vec3 spawn_point = PLAYER.position;
	Vec3 player_right = right(&PLAYER);
	Vec3 player_fwd = forward(&PLAYER);
	spawn_point.y += 0.5f;
	spawn_point.x += 0.25f * player_right.x;
	spawn_point.z += 0.25f * player_right.z;
	spawn_point.x -= 0.3f * player_fwd.x;
	spawn_point.z -= 0.3f * player_fwd.z;

	BULLETS[current_bullet].position = spawn_point;
	BULLETS[current_bullet].rotation = PLAYER.rotation;
	current_bullet++;
	GUN_RECOIL = 0.3f;
}


// Game logic

void player_update() {
	float move_fwd = KEYBOARD['w'] - KEYBOARD['s'];
	float move_right = KEYBOARD['a'] - KEYBOARD['d'];
	Vec3 cam_fwd = (Vec3) {PLAYER.position.x - CAM.position.x, 0.0f, PLAYER.position.z - CAM.position.z};
	Vec3 cam_right = right(&PLAYER);

	PLAYER.position.x += 0.5f * PLAYER_SPEED * move_fwd * cam_fwd.x * DT;
	PLAYER.position.z += 0.5f * PLAYER_SPEED * move_fwd * cam_fwd.z * DT;

	PLAYER.position.x += 2.0f * PLAYER_SPEED * move_right * cam_right.x * DT;
	PLAYER.position.z += 2.0f * PLAYER_SPEED * move_right * cam_right.z * DT;

	GUN_RECOIL -= 10.0f * DT;
	if(GUN_RECOIL < 0.0f)
		GUN_RECOIL = 0.0f;
}

void cam_update() {
	Vec3 player_fwd = forward(&PLAYER);
	CAM.position.x = PLAYER.position.x + 3.0f * player_fwd.x;
	CAM.position.z = PLAYER.position.z + 3.0f * player_fwd.z;
}

void bullet_update(int index) {
	Vec3 bullet_fwd = forward(&BULLETS[index]);
	BULLETS[index].position.x -= BULLET_SPEED * bullet_fwd.x * DT;
	BULLETS[index].position.z -= BULLET_SPEED * bullet_fwd.z * DT;
}


// Main

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(WINDOW_SIZE.x, WINDOW_SIZE.y);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Branca de Neve: Global Offensive 3D");
	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(WINDOW_CENTER.x, WINDOW_CENTER.y);	

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard_up);
	glutReshapeFunc(reshape);

	init_gl();

	CAM.position = (Vec3) {0.0f, 3.0f, -3.0f};
	
	glutMainLoop();

	return 0;
}


void init_gl() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}


// Callbacks

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// View matrix
	Vec3 u = up(&CAM);
	Vec3 eye = CAM.position;
	Vec3 center = PLAYER.position;

	gluLookAt(eye.x, eye.y - 0.5f, eye.z, center.x, center.y + 1.0f, center.z, u.x, u.y, u.z); 

	#ifdef DEBUG
	draw_grid(10);
	draw_axis(1, 1, 1);
	#endif
	
	draw_ground();

	glPushMatrix();
		glTranslatef(PLAYER.position.x, PLAYER.position.y, PLAYER.position.z);
		glRotatef(PLAYER.rotation.y, 0.0f, 1.0f, 0.0f);
		draw_snow_white();
	glPopMatrix();

	int i;
	for(i = 0;i < ACTIVE_BULLETS;i++){
		glPushMatrix();
			glTranslatef(BULLETS[i].position.x, BULLETS[i].position.y, BULLETS[i].position.z);
			draw_bullet();
		glPopMatrix();
	}

	glutSwapBuffers();
}

void idle() {
	static int last_t;

	int t = glutGet(GLUT_ELAPSED_TIME);
	float new_t = (float) t / 10e3;
	DT = new_t - T;
	T = new_t;

	player_update();
	int i;
	for(i = 0;i < ACTIVE_BULLETS;i++) {
		bullet_update(i);
	}
	cam_update();

	glutPostRedisplay();
	last_t = t;
}

void motion(int x, int y) { 
	static int wrap = 0;
	Vec2 delta;
	
    if (!wrap) {
        delta.x = x - WINDOW_CENTER.x;
        delta.y = y - WINDOW_CENTER.y;

		// CAM.rotation.x += delta.y > 0 ? 1.0f : (delta.y < 0 ? -1.0f : 0.0f);
		PLAYER.rotation.y -= 90.0f * delta.x * DT;

        wrap = 1;
        glutWarpPointer(WINDOW_CENTER.x, WINDOW_CENTER.y);
    } else {
        wrap = 0;
    }
}

void mouse(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		shoot();
	}
}

void keyboard(unsigned char key, int x, int y){
	if(key == 27)
		glutLeaveMainLoop();

	KEYBOARD[tolower(key)] = 1;
}

void keyboard_up(unsigned char key, int x, int y){
	KEYBOARD[tolower(key)] = 0;
}

void reshape(int width, int height) {
	float aspect = (float) width / (float) height;
	WINDOW_SIZE.x = width;
	WINDOW_SIZE.y = height;
	WINDOW_CENTER.x = width / 2;
	WINDOW_CENTER.y = height / 2;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, aspect, ZNEAR, ZFAR);
}


// Drawing utils

void draw_axis(int x, int y, int z) {
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	if(x) {
		glColor3f(RED);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(ZFAR, 0.0f, 0.0f);
	}
	if(y) {
		glColor3f(GREEN);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, ZFAR, 0.0f);
	}
	if(z) {
		glColor3f(BLUE);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, ZFAR);
	}
	glEnd();
	glLineWidth(2.0f);
}

void draw_grid(int n) {
	int i;

	glBegin(GL_LINES);
	glColor3f(WHITE);
	for(i = -n;i <= n;i++){
		float d = (float) i;
		// Parallel to x-axis 
		glVertex3f(-n, 0.0f, d);
		glVertex3f(i != 0 ? n : 0.0f, 0.0f, d);
		// Parallel to z-axis
		glVertex3f(d, 0.0f, -n);
		glVertex3f(d, 0.0f, i != 0 ? n : 0.0f);
	}
	glEnd();
}


// Math utils

Vec3 forward(Transform* t) {
	Vec3 v;
	float a = t->rotation.x * DEG2RAD;
	float b = t->rotation.y * DEG2RAD;
	float c = t->rotation.z * DEG2RAD;

	v.x = -(cos(c) * sin(b) + sin(c) * sin(a) * cos(b));
	v.y = -(sin(c) * sin(b) - cos(c) * sin(a) * cos(b));
	v.z = -(cos(a) * cos(b));

	return v;
}

Vec3 up(Transform* t) {
	Vec3 v;
	float a = t->rotation.x * DEG2RAD;
	float b = t->rotation.y * DEG2RAD;
	float c = t->rotation.z * DEG2RAD;

	v.x = -sin(c) * cos(a);
	v.y = cos(c) * cos(a);
	v.z = sin(a);
	
	return v;
}

Vec3 right(Transform* t) {
	Vec3 v;
	float a = t->rotation.x * DEG2RAD;
	float b = t->rotation.y * DEG2RAD;
	float c = t->rotation.z * DEG2RAD;

	v.x = cos(c) * cos(b) - sin(c) * sin(a) * sin(b);
	v.y = sin(c) * cos(b) + cos(c) * sin(a) * sin(b);
	v.z = -cos(a) * sin(b);

	return v;
}
