#include <iostream>
#include <cmath>
#ifdef WIN32
#include <windows.h>
#include <gl/GL.h>
#include "GLUT.h"
#else
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#endif

using namespace std;

double getTime();      // �J�n������̌o�ߎ��Ԃ�b�P�ʂŎ擾����֐�
double mousePosX = 0;  // �}�E�X�J�[�\���ʒuX
double mousePosY = 0;  // �}�E�X�J�[�\���ʒuY
unsigned char key = 0; // �L�[�{�[�h����̓��͒l
void predisplay();     // �`��J�n���ɕK���ĂԊ֐�
void postdisplay();    // �`��I�����ɕK���ĂԊ֐�

////////////////////////////////////////////////////////////////////////////////

void display()
{
    predisplay(); // �`��J�n���ɕK���ĂԊ֐��B�����Ă͂Ȃ�Ȃ�
    /***** �ȍ~��ҏW���� *****/

    // �L�[�{�[�h���͂ɉ���������
    if (key == 'q' || key == 'Q')
    {
        exit(0);
    }

    double time = getTime(); // �o�ߎ��Ԃ�b�P�ʂŎ擾

    /////
    ///// 2D�p�[�g�J�n
    glDisable(GL_LIGHTING);  // �����Ă͂����Ȃ�

    /////
    ///// 3D�p�[�g�J�n
    glEnable(GL_LIGHTING);    // �����Ă͂����Ȃ�

	glPushMatrix();
	double xdist1 = mousePosX - (-50.0);
	double ydist1 = mousePosY;
	double dist1 = sqrt(xdist1 * xdist1 + ydist1 * ydist1);
	if (dist1 <= 20)
	{
		float sphere1Color[4] = { 0.3, 1.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sphere1Color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sphere1Color);
	}
	else
	{
		float sphere1Color[4] = { 0.1, 0.2, 0.0, 1.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sphere1Color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sphere1Color);
	}
	glTranslated(-50, 0, 0);
	glutSolidSphere(20.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, 0);
	float sphere2Color[4] = { 0.1, 0.2, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sphere2Color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sphere2Color);
	glutSolidSphere(20.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslated(50, 0, 0);
	float sphere3Color[4] = { 0.1, 0.2, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sphere3Color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sphere3Color);
	glutSolidSphere(20.0, 20, 20);
	glPopMatrix();

    /***** �����܂ŕҏW���� *****/
    postdisplay(); // �`��I�����ɕK���ĂԊ֐��B�����Ă͂Ȃ�Ȃ�
}

////////////////////////////////////////////////////////////////////////////////

#pragma region not have to edit
void predisplay()
{
    // �w�i�F�̐ݒ�
    glClearColor(0.0, 0.0, 0.0, 1.0); // �w�i�F [Red, Green, Blue, Alpha] Alpha��1.0�Œ�
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 100.0,
              0, 0, 0,
              0, 1.0, 0.0);
}

void postdisplay()
{
    glutSwapBuffers();
}

void keyboard(unsigned char k, int x, int y)
{
    key = k;
}

static double startTime;
static double prevTime;
double getTime()
{
#ifdef WIN32
    LARGE_INTEGER freq, t;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart / freq.QuadPart - startTime;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1.0e9 - startTime;
#endif
}

void idle()
{
    double time = getTime();
    if (time - prevTime > 1.0 / 60.0)
    {
        display();
        prevTime = time;
    }
}

int screenWidth = 1;
int screenHeight = 1;
void resize(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100.0, 100.0, -100.0, 100.0, 0.1, 200.0);
}

void mouseMotion(int x, int y)
{
    mousePosX =  (x - screenWidth  / 2) * 200.0 / screenWidth;
    mousePosY = -(y - screenHeight / 2) * 200.0 / screenHeight;
    glutIdleFunc(idle);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800); // �E�B���h�E�T�C�Y [��, ����]
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("graphics"); // �E�B���h�E��
    
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMotion);
    glutReshapeFunc(resize);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHT0);
    float gray[4]  = { 0.5, 0.5, 0.5, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, gray);
    float black[4] = { 0.1, 0.1, 0.1, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);

    // ���C�g�̈ʒu [x, y, z, 1]
    float lightPos[4] = { 0, 80.0, 100.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    // ���C�g�̐F [Red, Green, Blue, 1]
    float lightColor[4] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

#ifdef WIN32
    LARGE_INTEGER freq, t;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    startTime = (double)t.QuadPart / freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    startTime = ts.tv_sec + ts.tv_nsec / 1.0e9;
#endif
    prevTime = getTime();
    glutMainLoop();
    return 0;
}
#pragma endregion
