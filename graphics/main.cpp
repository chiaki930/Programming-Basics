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

                       // テクスチャマッピングのための追加
bool loadTexture(int textureId, const char* filename, int width, int height);
GLuint textureID;

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

    // 今回使う画像は1枚のみ
    glGenTextures(1, &textureID);
    // earth.raw という画像ファイルを読み込む（RAWファイルのみ使用可能）
    loadTexture(textureID, "earth.raw", 2048, 1025);
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

    // マウスカーソルについてくる四角形（画像貼付）
    glPushMatrix();
    //
    // 位置指定 [x, y, z]
    glTranslated(mousePosX, mousePosY, 0);
    // 画像を貼り付けるためのおまじない
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    // 四角形（画像で塗りつぶし）
    glBegin(GL_POLYGON);
    glTexCoord2f(0, 0);  glVertex3d(-40, -40, 0); // 画像の左下[0, 0]と四角形の左下[-40, -40]を対応付け
    glTexCoord2f(0, 1);  glVertex3d(-40,  40, 0); // 画像の左上[0, 1]と四角形の左上[-40,  40]を対応付け
    glTexCoord2f(1, 1);  glVertex3d( 40,  40, 0); // 画像の右上[1, 1]と四角形の右上[-40,  40]を対応付け
    glTexCoord2f(1, 0);  glVertex3d( 40, -40, 0); // 画像の右下[1, 0]と四角形の右下[ 40, -40]を対応付け
    glEnd();
    // 画像貼付おわりのおまじない
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    //
    glPopMatrix();

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

bool loadTexture(int textureId, const char* filename, int width, int height)
{
    // テクスチャマップをファイルから読み込み
    FILE* ftex = nullptr;
    fopen_s(&ftex, filename, "rb");
    if (ftex == nullptr) // ファイルロード失敗
    {
        return false;
    }
    GLubyte* textureImage = new GLubyte[width * height * 4];
    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            // R, G, B & アルファ成分をファイルから読み出し
            for (int i = 0; i < 4; ++i)
            {
                fread(&textureImage[((height - h - 1) * width + w) * 4 + i], sizeof(unsigned char), 1, ftex);
            }
        }
    }
    fclose(ftex);

    // テクスチャオブジェクトの作成
    glBindTexture(GL_TEXTURE_2D, textureId);
    // テクスチャの割り当て 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage);
    // テクスチャマップのデータ格納形式の指定
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // テクスチャの繰り返し方法の指定 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // テクスチャを拡大・縮小する方法の指定 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // 色の調整（環境の設定）
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, 0);
    delete[] textureImage;
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
