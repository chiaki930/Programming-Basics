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

// 25個のティーポットの位置と角度
double xpos[5][5];  // X座標
double ypos[5][5];  // y座標
double angle[5][5]; // 回転角度

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

    // 25個のティーポットをランダムに配置する
    for (int x = 0; x < 5; ++x)
    {
        for (int y = 0; y < 5; ++y)
        {
            xpos[x][y] = (double(rand()) / RAND_MAX * 2.0 - 1.0) * 100.0;
            ypos[x][y] = (double(rand()) / RAND_MAX * 2.0 - 1.0) * 100.0;
        }
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

    // 25個のティーポットを同じ角度で回転させる
    for (int x = 0; x < 5; ++x)
    {
        for (int y = 0; y < 5; ++y)
        {
            angle[x][y] = time * 60.0;
        }
    }
    
    // 描画
	for (int x = 0; x < 5; ++x)
	{
		for (int y = 0; y < 5; ++y)
		{
			glPushMatrix();
			//
			// 位置 [x, y, z]
			glTranslated(xpos[x][y], ypos[x][y], 0);
			// 回転 [角度(degree), 回転軸x, 回転軸y, 回転軸z]
			glRotated(angle[x][y], 0, 0, 1.0);
			// 色
			float teapot1Color[4] = { 1.0, 0.2, 0.2, 1.0 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teapot1Color);
			// 描画
			glutSolidTeapot(10.0);
			//
			glPopMatrix();
		}
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
    setup();
    glutMainLoop();
    return 0;
}
#pragma endregion
