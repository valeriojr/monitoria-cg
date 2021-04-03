#include <GL/gl.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_NAME "Example 5 - Arm"

// Colors
#define BLACK  0.0f, 0.0f, 0.0f
#define WHITE  1.0f, 1.0f, 1.0f
#define RED    1.0f, 0.0f, 0.0f
#define GREEN  0.0f, 1.0f, 0.0f
#define BLUE   0.0f, 0.0f, 1.0f
#define YELLOW 1.0f, 1.0f, 0.0f

// Camera
#define FOV_Y 70.0f

// Joint
#define JOINT_SECTORS 10
#define JOINT_STACKS 10


enum {
	ARM,
	FOREARM,
	HAND,
	THUMB_1,
	THUMB_2,
	THUMB_3,
	INDEX_1,
	INDEX_2,
	INDEX_3,
	MIDDLE_1,
	MIDDLE_2,
	MIDDLE_3,
	RING_1,
	RING_2,
	RING_3,
	PINKY_1,
	PINKY_2,
	PINKY_3,
	JOINT_COUNT
};


void display_grid(int size) {
	int i;

	glColor3f(WHITE);
	for(i = -size / 2;i < 2 * size;i++){
		glBegin(GL_LINES);
			glVertex3f(1.0f * i, 0.0f, -10.0f);
			glVertex3f(1.0f * i, 0.0f, 10.0f);
		glEnd();
	}
	for(i = -size / 2;i < 2 * size;i++) {
		glBegin(GL_LINES);
			glVertex3f(-10.0f, 0.0f, 1.0f * i);
			glVertex3f(10.0f, 0.0f, 1.0f * i);
		glEnd();
	}
}


/*
 * Exemplo 5 - Arm
 * Criado por Valerio Nogueira em 02/04/2021
 *
 * Este exemplo mostra como fazer um braço utilizando 'parenting'.
 * As partes do braço podem ser rotacionadas com <adicionar
 * controles>.
 */


/*
 * Vetor tridimensional. Os campos podem ser interpretados de 
 * diferentes maneiras, como posicao (x, y e z), cor (r, g e b) 
 * ou vetor de angulos de Euler. Isto e, os campos x, r e pitch 
 * se referem à mesma posição na memória.
 */
typedef struct Vec3 {
	union{
		struct	{
			float x, y, z;
		};
		struct {
			float r, g, b;
		};
		struct {
			float pitch, yaw, roll;
		};
	};
} Vec3;

/*
 * Posicao, orientacao e escala da junta
 */
typedef struct Transform {
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
} Transform;

/* Descreve uma junta.
 *
 * Joint
 *     | 
 *     v          Arm
 *    __ _______    |
 *   (##)_______| <_|
 *    ¨¨
 */
typedef struct Joint {
	Transform transform;
	float radius;
	Vec3 color;
	Vec3 arm_size;
	Vec3 arm_color;
	int child_count;
	struct Joint* children;
} Joint;


enum {X, Y, Z};
Joint* joints[JOINT_COUNT];

struct {
	Transform camera;
	Joint root;
	Joint* current;
	int axis;
} AppState;


void set_transform(Transform* t) {
	glTranslatef(t->position.x, t->position.y, t->position.z);

	glRotatef(t->rotation.z, 0.0f, 0.0f, 1.0f);
	glRotatef(t->rotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(t->rotation.x, 1.0f, 0.0f, 0.0f);

	glScalef(t->scale.x, t->scale.y, t->scale.z);
}


Joint create_joint(float radius, Vec3 arm_size, int child_count) {
	Joint j;
	
	j.radius = radius;
	j.color = (Vec3) {RED};

	j.arm_size = arm_size;
	j.arm_color = (Vec3) {BLUE};
	
	j.child_count = child_count;
	j.children = (Joint*) malloc(child_count * sizeof(Joint));

	j.transform.position = (Vec3) {0.0f, 0.0f, 0.0f};
	j.transform.rotation = (Vec3) {0.0f, 0.0f, 0.0f};
	j.transform.scale = (Vec3) {1.0f, 1.0f, 1.0f};

	return j;
}


void display_joint(Joint* j){
	set_transform(&j->transform);

	if(j == AppState.current){
		glColor3f(YELLOW);
	}
	else {
		glColor3f(j->color.r, j->color.g, j->color.b);
	}
	glutSolidSphere(j->radius, JOINT_SECTORS, JOINT_STACKS);	
	
	glPushMatrix();
		if(j == AppState.current){
			glColor3f(YELLOW);
		}
		else {
			glColor3f(j->arm_color.r, j->arm_color.g, j->arm_color.b);
		}
		glTranslatef(0.0f, 0.0f, -j->arm_size.z / 2.0f);
		glScalef(j->arm_size.x, j->arm_size.y, j->arm_size.z);
		glutSolidCube(1.0f);
	glPopMatrix();

	int i;
		
	for(i = 0;i < j->child_count;i++){
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, -j->arm_size.z);
			display_joint(&(j->children[i]));
		glPopMatrix();		
	}
}


void destroy_joint(Joint* j) {
	int i;
	for(i = 0;i < j->child_count;i++){
		destroy_joint(&(j->children[i]));
	}

	free(j->children);
}


void reshape(int width, int height){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float aspect = (float) width / (float) height;
	gluPerspective(FOV_Y, aspect, 0.001f, 1000.0f);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(AppState.camera.position.x, AppState.camera.position.y, AppState.camera.position.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	display_grid(10);

	display_joint(&AppState.root);

	glutSwapBuffers();
	glutPostRedisplay();
}


void keyboard(char key, int x, int y) {
	if(key == 'x' || key == 'X') {
		AppState.axis = X;
		printf("Changing rotation axis to X-axis\n");
	}
	if(key == 'y' || key == 'Y') {
		AppState.axis = Y;
		printf("Changing rotation axis to Y-axis\n");
	}
	if(key == 'z' || key == 'Z') {
		AppState.axis = Z;
		printf("Changing rotation axis to Z-axis\n");
	}
}


void mouse(int button, int state, int x, int y){
	printf("button: %d, state: %d, x: %d, y: %d\n", button, state, x, y);
		
	if(button == 3 || button == 4)
	{
		if(state == GLUT_UP){
			return;
		}
		int dir = button == 3 ? 1 : -1; 
		float* rotation = (float*) &(AppState.current->transform.rotation);
		rotation[AppState.axis] += dir * 10.0f;
	}
}


void menu(int item){
	AppState.current = joints[item];	
}


Joint create_finger(){
	Joint j1, j2, j3;
		
	Vec3 size1 = (Vec3) {0.05f, 0.05f, 0.1f};
	Vec3 size2 = (Vec3) {0.04f, 0.04f, 0.1f};
	Vec3 size3 = (Vec3) {0.04f, 0.03f, 0.1f};
	
	j1 = create_joint(0.05f, size1, 1);
	j2 = create_joint(0.04f, size2, 1);
	j3 = create_joint(0.03f, size3, 0);

	j2.children[0] = j3;
	j1.children[0] = j2;

	return j1;
}


Joint create_hand() {
	Vec3 hand_size = (Vec3) {0.35f, 0.1f, 0.3f};
	Joint hand = create_joint(0.1f, hand_size, 5);

	Joint thumb = create_finger();
	thumb.transform.position = (Vec3) {0.15f, 0.0f, 0.25f};
	thumb.transform.rotation.y = -75.0f;
	hand.children[0] = thumb;
	
	int i;
	for(i = 1;i < 5;i++){
		Joint finger = create_finger();
		finger.transform.position.x = -0.25f + 0.1f * i;
		hand.children[i] = finger;
	}

	

	return hand;
}


Joint create_arm() {
	Joint forearm = create_joint(0.1f, (Vec3) {0.1f, 0.1f, 0.5f}, 1);
	Joint arm = create_joint(0.1f, (Vec3) {0.1f, 0.1f, 0.5f}, 1);

	forearm.children[0] = create_hand();
	forearm.transform.rotation.y = -75.0f;
	arm.children[0] = forearm;
	arm.transform.rotation.y = -45.0f;

	return arm;
}


void create_menu(Joint* arm) {
	joints[ARM] = arm;
	Joint* forearm = arm->children;
	joints[FOREARM] = forearm;
	Joint* hand = forearm->children;
	joints[HAND] = hand;
	Joint* thumb1 = &hand->children[0];
	joints[THUMB_1] = thumb1;
	Joint* thumb2 = thumb1->children;
	joints[THUMB_2] = thumb2;
	Joint* thumb3 = thumb2->children;
	joints[THUMB_3] = thumb3;
	Joint* index1 = &hand->children[1];
	joints[INDEX_1] = index1;
	Joint* index2 = index1->children;
	joints[INDEX_2] = index2;
	Joint* index3 = index2->children;
	joints[INDEX_3] = index3;
	Joint* middle1 = &hand->children[2];
	joints[MIDDLE_1] = middle1;
	Joint* middle2 = middle1->children;
	joints[MIDDLE_2] = middle2;
	Joint* middle3 = middle2->children;
	joints[MIDDLE_3] = middle3;
	Joint* ring1 = &hand->children[3];
	joints[RING_1] = ring1;
	Joint* ring2 = ring1->children;
	joints[RING_2] = ring2;
	Joint* ring3 = ring2->children;
	joints[RING_3] = ring3;
	Joint* pinky1 = &hand->children[4];
	joints[PINKY_1] = pinky1;
	Joint* pinky2 = pinky1->children;
	joints[PINKY_2] = pinky2;
	Joint* pinky3 = pinky2->children;
	joints[PINKY_3] = pinky3;

	int arm_menu = glutCreateMenu(menu);
	int forearm_menu = glutCreateMenu(menu);
	int hand_menu = glutCreateMenu(menu);
	int thumb_menu = glutCreateMenu(menu);
	int index_menu = glutCreateMenu(menu);
	int middle_menu = glutCreateMenu(menu);
	int ring_menu = glutCreateMenu(menu);
	int pinky_menu = glutCreateMenu(menu);

	glutSetMenu(arm_menu);
	glutAddMenuEntry("Arm", ARM);
	glutAddSubMenu("Forearm", forearm_menu);

	glutSetMenu(forearm_menu);
	glutAddMenuEntry("Forearm", FOREARM);
	glutAddSubMenu("Hand", hand_menu);

	glutSetMenu(hand_menu);
	glutAddMenuEntry("Hand", HAND);
	glutAddSubMenu("Thumb", thumb_menu);
	glutAddSubMenu("Index", index_menu);
	glutAddSubMenu("Middle", middle_menu);
	glutAddSubMenu("Ring", ring_menu);
	glutAddSubMenu("Pinky", pinky_menu);

	glutSetMenu(thumb_menu);
	glutAddMenuEntry("Thumb 1", THUMB_1);
	glutAddMenuEntry("Thumb 2", THUMB_2);
	glutAddMenuEntry("Thumb 3", THUMB_3);

	glutSetMenu(index_menu);
	glutAddMenuEntry("Index 1", INDEX_1);
	glutAddMenuEntry("Index 2", INDEX_2);
	glutAddMenuEntry("Index 3", INDEX_3);

	glutSetMenu(middle_menu);
	glutAddMenuEntry("Middle 1", MIDDLE_1);
	glutAddMenuEntry("Middle 2", MIDDLE_2);
	glutAddMenuEntry("Middle 3", MIDDLE_3);

	glutSetMenu(ring_menu);
	glutAddMenuEntry("Ring 1", RING_1);
	glutAddMenuEntry("Ring 2", RING_2);
	glutAddMenuEntry("Ring 3", RING_3);

	glutSetMenu(pinky_menu);
	glutAddMenuEntry("Pinky 1", PINKY_1);
	glutAddMenuEntry("Pinky 2", PINKY_2);
	glutAddMenuEntry("Pinky 3", PINKY_3);

	glutSetMenu(arm_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_NAME);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glEnable(GL_DEPTH_TEST);
	
	reshape(WINDOW_WIDTH, WINDOW_HEIGHT);
	AppState.camera.position = (Vec3) {1.0f, 1.0f, 1.0f};
	
	AppState.root = create_arm();
	AppState.current = &AppState.root;

	create_menu(&AppState.root);
	joints[ARM] = &AppState.root;
	
	glutMainLoop();

	destroy_joint(&AppState.root);

	return 0;
}
