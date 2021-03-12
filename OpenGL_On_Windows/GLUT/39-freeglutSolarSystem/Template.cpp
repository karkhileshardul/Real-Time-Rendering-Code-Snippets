#include<GL/freeglut.h>
#include<stdlib.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
bool gbFullscreen = false;

static int year = 0;
static int day = 0;

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
	glutWireSphere(1.0,20,16);
	glRotatef((GLfloat)year,0.0f,1.0f,0.0f);
	glTranslatef(2.0f,0.0f,0.0f);
	glRotatef((GLfloat)day,0.0f,1.0f,0.0f);
	glutWireSphere(0.2,10,8);
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
	case 'd':
		day=(day+10)%360;
		glutPostRedisplay();
		break;
	case 'D':
		day=(day-10)%360;
		glutPostRedisplay();
		break;
	case 'y':
		year=(year+5)%360;
		glutPostRedisplay();
		break;
	case 'Y':
		year=(year-5)%360;
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
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void uninitialize(void) {

}
