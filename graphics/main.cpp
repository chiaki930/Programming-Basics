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

double getTime();      // 開始時からの経過時間を秒単位で取得する関数
double mousePosX = 0;  // マウスカーソル位置X
double mousePosY = 0;  // マウスカーソル位置Y
unsigned char key = 0; // キーボードからの入力値
void predisplay();     // 描画開始時に必ず呼ぶ関数
void postdisplay();    // 描画終了時に必ず呼ぶ関数

// 10個のティーポットの情報
double xpos[10];    // x座標
double ypos[10];    // Y座標
float color[10][4]; // 色

////////////////////////////////////////////////////////////////////////////////

void setup()
{
    float gray[4] = { 0.5, 0.5, 0.5, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, gray);
    float black[4] = { 0.1, 0.1, 0.1, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);

    // ライトの位置 [x, y, z, 1]
    float lightPos[4] = { 0, 80.0, 100.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    // ライトの色 [Red, Green, Blue, 1]
    float lightColor[4] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

    // 10個のティーポットの位置と色指定
    for (int i = 0; i < 10; ++i)
    {
        xpos[i] = i * 10.0;
        ypos[i] = i * 10.0;
        color[i][0] = 1.0; //R
        color[i][1] = 0.2; //G
        color[i][2] = 0.2; //B
        color[i][3] = 1.0; //A = 1
    }
}

void display()
{
    predisplay(); // 描画開始時に必ず呼ぶ関数。消してはならない
    /***** 以降を編集する *****/

    // キーボード入力に応じた処理
    if (key == 'q' || key == 'Q')
    {
        exit(0);
    }

    double time = getTime(); // 経過時間を秒単位で取得

    /////
    ///// 2Dパート開始
    glDisable(GL_LIGHTING);  // 消してはいけない

    /////
    ///// 3Dパート開始
    glEnable(GL_LIGHTING);    // 消してはいけない

    // 円周上を動かす
    for (int i = 0; i < 10; ++i)
    {
        xpos[i] = 50.0 * sin(2.0 * time + i * 0.5) + mousePosX;
        ypos[i] = 50.0 * cos(2.0 * time + i * 0.5) + mousePosY;
    }

    // キーボードに応じて色を変える
    if (key == 'r' || key == 'R')
    {
        for (int i = 0; i < 10; ++i)
        {
            color[i][0] = 1.0; // R
            color[i][1] = 0.0; // G
            color[i][2] = 0.0; // B
        }
    }
    else if (key == 'g' || key == 'G')
    {
        for (int i = 0; i < 10; ++i)
        {
            color[i][0] = 0.0; // R
            color[i][1] = 1.0; // G
            color[i][2] = 0.0; // B
        }
    }
    else if (key == 'b' || key == 'B')
    {
        for (int i = 0; i < 10; ++i)
        {
            color[i][0] = 0.0; // R
            color[i][1] = 0.0; // G
            color[i][2] = 1.0; // B
        }
    }

    // 10個のティーポットの描画
	for (int i = 0; i < 10; ++i)
	{
		glPushMatrix();
		//
		// 位置 [x, y, z]
		glTranslated(xpos[i], ypos[i], 0);
		// 回転 [角度(degree), 回転軸x, 回転軸y, 回転軸z]
		glRotated(time * 60.0, 0, 0, 1.0);
		// 色
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color[i]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color[i]);
        // 描画
		glutSolidTeapot(10.0);
		//
		glPopMatrix();
	}

    /***** ここまで編集する *****/
    postdisplay(); // 描画終了時に必ず呼ぶ関数。消してはならない
}

////////////////////////////////////////////////////////////////////////////////

#pragma region not have to edit
void predisplay()
{
    // 背景色の設定
    glClearColor(0.0, 0.0, 0.0, 1.0); // 背景色 [Red, Green, Blue, Alpha] Alphaは1.0固定
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
    glutInitWindowSize(800, 800); // ウィンドウサイズ [幅, 高さ]
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("graphics"); // ウィンドウ名
    
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMotion);
    glutReshapeFunc(resize);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHT0);

    setup();

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
