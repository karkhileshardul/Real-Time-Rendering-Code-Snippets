#include<GL/freeglut.h>
#include<stdlib.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

bool gbFullscreen = false;

static int shoulder = 0;
static int elbow = 0;

int main(int argc, char **argv) {

	void display(void);
	void initialize(void);
	void uninitialize(void);
	void resize(int, int);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutCreateWindow("39-SolarSystem using freeglut");

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	return 0;
}

void initialize(void){
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glShadeModel(GL_FLAT);

}
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(-1.0f,0.0f,0.0f);
	glRotatef((GLfloat)shoulder,0.0f,0.0f,1.0f);
	glTranslatef(1.0f,0.0f,0.0f);
	glPushMatrix();
	glScalef(2.0f,0.4f,1.0f);
	glutWireCube(1.0);
	glPopMatrix();

	glTranslatef(1.0f,0.0f,0.0f);
	glRotatef((GLfloat)elbow,0.0f,0.0f,1.0f);
	glTranslatef(1.0f,0.0f,0.0f);
	glPushMatrix();
	glScalef(2.0f,0.4f,1.0f);
	glutWireCube(1.0);
	glPopMatrix();

	glPopMatrix();
	glutSwapBuffers();

}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		glutLeaveMainLoop();
		break;
	case 'F':
	case 'f':
		if (gbFullscreen == false) {
		
			glutFullScreen();
			gbFullscreen = true;
			
		}
		else {
			glutLeaveFullScreen();
			gbFullscreen = false;
		}
		break;
	case 's':
		shoulder=(shoulder+5)%360;
		glutPostRedisplay();
		break;
	case 'S':
		shoulder=(shoulder-5)%360;
		glutPostRedisplay();
		break;
	case 'e':
		elbow=(elbow+5)%360;
		glutPostRedisplay();
		break;
	case 'E':
		elbow=(elbow-5)%360;
		glutPostRedisplay();		
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		break;
	default:
		break;
	}
}

void resize(int width, int height) {
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-5.0f);
}

void uninitialize(void) {

}
