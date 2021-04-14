#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <stdio.h>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT


public:

    // для камеры
    float xAlpha = 70.0;
    float zAlpha = 0.0;
    float zPoint = -5;
    float xPoint = 0;
    float yPoint = 0;
    float ugol = -zAlpha / 180 * M_PI;
    float speed = 0;


    // для поля
    typedef struct{
        float x,y,z;
    }TCell;
    typedef struct{
        float r,g,b;
    }TColor;
#define mapW 100
#define mapH 100
    TCell map[mapW][mapH];
    TColor mapCol[mapW][mapH];
    GLuint mapInd[mapW -1][mapH -1][6];
    TCell mapNormal[mapW][mapH];
    int mapIndCnt = sizeof(mapInd) / sizeof(GLuint);

    // для генерации снега
    TCell f[mapW][mapH];
    TColor fCol[mapW][mapH];
    GLuint fInd[mapW -1][mapH -1][1];
    int fIndCnt = sizeof(fInd) / sizeof(GLuint);


    float alpha = 0; // угол освещения

    GLWidget(QWidget *parent = nullptr);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void MoveCamera(); // перемещение камеры

    void Map_Init();
    void Map_Show();

    void Snow();

    void CalcNormals(TCell a, TCell b, TCell c, TCell *n);

    void Change_Sun();


};


#endif // GLWIDGET_H
