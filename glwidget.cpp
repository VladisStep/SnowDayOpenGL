#include "glwidget.h"





GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent){
}

void GLWidget::initializeGL(){

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1,1, -1,1, 2,100); //ортоганальная проекция
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Map_Init();
    glEnable(GL_DEPTH_TEST); // включаем буффер глубины

}


void GLWidget::MoveCamera(){

    if (speed != 0){
        xPoint += sin(ugol) * speed;
        yPoint += cos(ugol) * speed;
    }
    glRotatef(-xAlpha, 1,0,0);
    glRotatef(-zAlpha, 0,0,1);
    glTranslatef(-xPoint, -yPoint, zPoint);
}



void GLWidget::Map_Init(){

    // освещение
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);


    // генерация точек и цвета земли
    for (int i = 0; i < mapW; i++) // проходимся повсем координатам кары
        for (int j = 0; j < mapH; j++){

            float dc = (rand() % 20) * 0.01; //  задаем массив цветов
            mapCol[i][j].r = 0.7 + dc;
            mapCol[i][j].g = 0.7 + dc;
            mapCol[i][j].b = 0.7 + dc;

            map[i][j].x = i; // задаем массив вершин
            map[i][j].y = j;
            map[i][j].z = (rand() % 10) * 0.05;
        }


    // создаем массив индексов, по которым будет рисоваться карта
    for (int i = 0; i < mapW-1; i++){
        int pos = i * mapH;
        for (int j = 0; j < mapH-1; j++){
            mapInd[i][j][0] = pos;
            mapInd[i][j][1] = pos + 1;
            mapInd[i][j][2] = pos + 1 + mapH;

            mapInd[i][j][3] = pos + 1 + mapH;
            mapInd[i][j][4] = pos + mapH;
            mapInd[i][j][5] = pos;

            pos++;
        }
    }

    // генерация снега
    for (int i = 0; i < mapW; i++) // проходимся повсем координатам кары
        for (int j = 0; j < mapH; j++){

            float dc = (rand() % 20) * 0.01; //  задаем массив цветов
            fCol[i][j].r = 1;
            fCol[i][j].g = 1;
            fCol[i][j].b = 1;

            f[i][j].x = i; // задаем массив вершин
            f[i][j].y = j;
            f[i][j].z = rand() % 50;
        }
        // создаем массив индексов, по которым будет рисоваться снежинки
        for (int i = 0; i < mapW-1; i++){
            int pos = i * mapH;
            for (int j = 0; j < mapH-1; j++){
                fInd[i][j][0] = pos;
                pos++;
            }
        }



    for (int i = 0; i < mapW-1; i++){ // заполнение нормалей земли
        for (int j = 0; j < mapH -1; j++){
            CalcNormals(map[i][j], map[i+1][j], map[i][j+1], &mapNormal[i][j]);
        }
    }
}


void GLWidget::Snow(){
    for (int i = 0; i < mapW; i++) // проходимся повсем координатам кары
        for (int j = 0; j < mapH; j++){

            f[i][j].x += sin(f[i][j].y + f[i][j].z); // задаем массив вершин

            f[i][j].y += sin(f[i][j].x + f[i][j].z);

            f[i][j].x = f[i][j].x > 100 ? 0: f[i][j].x;
            f[i][j].y = f[i][j].y > 100 ? 0: f[i][j].y;

            f[i][j].x = f[i][j].x < 0 ? 100: f[i][j].x;
            f[i][j].y = f[i][j].y < 0 ? 100: f[i][j].y;

            f[i][j].z = f[i][j].z - 0.5 < 0 ? 50 : f[i][j].z - 1;
        }
}

void GLWidget::Map_Show(){

    // изменение овещения
    if (alpha > 180) alpha -= 360;
    glPushMatrix();
        glRotatef(alpha, 0,1,0);
        GLfloat position[] = {0, 0, 1, 0};
        glLightfv(GL_LIGHT0, GL_POSITION, position);
    glPopMatrix();


    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

        // отрисовка земли
        glVertexPointer(3, GL_FLOAT, 0, map);
        glColorPointer(3, GL_FLOAT, 0, mapCol);
        glNormalPointer(GL_FLOAT, 0, mapNormal);
        glDrawElements(GL_TRIANGLES, mapIndCnt, GL_UNSIGNED_INT, mapInd);

        // отрисовка снега
        glPointSize(2);
        glVertexPointer(3, GL_FLOAT, 0, f);
        glColorPointer(3, GL_FLOAT, 0, fCol);
        glDrawElements(GL_POINTS, fIndCnt, GL_UNSIGNED_INT, fInd);

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);



}

void GLWidget::Change_Sun(){ // изменения положения модельки солнца


    float sun[] = {-1,-1,0, 1,-1,0, 1,1,0, -1,1,0};

    glPushMatrix();
        glRotatef(-xAlpha, 1,0,0);
        glRotatef(-zAlpha, 0,0,1);
        glRotatef(alpha, 0,1,0);
        glTranslatef(0,0,20);
        glDisable(GL_DEPTH_TEST);

            glDisable(GL_TEXTURE_2D);
            glColor3f(1,1,1);
            glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, sun);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glDisableClientState(GL_VERTEX_ARRAY);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_DEPTH_TEST);

      glPopMatrix();
}



void GLWidget::paintGL(){


    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        Change_Sun();
        MoveCamera();
        Map_Show();
    glPopMatrix();
}

void GLWidget::resizeGL(int w, int h){

}

#define sqr(a) (a)*(a)
void GLWidget::CalcNormals(TCell a, TCell b, TCell c, TCell *n){ // вычисление нормалей для земли
    float wrki;
    TCell v1, v2;

    v1.x = a.x - b.x;
    v1.y = a.y - b.y;
    v1.z = a.z - b.z;
    v2.x = b.x - c.x;
    v2.y = b.y - c.y;
    v2.z = b.z - c.z;

    n->x = (v1.y * v2.z - v1.z * v2.y);
    n->y = (v1.z * v2.x - v1.x * v2.z);
    n->z = (v1.x * v2.y - v1.y * v2.x);
    wrki = sqrt(sqr(n->x) + sqr(n->y) + sqr(n->z));
    n->x /= wrki;
    n->y /= wrki;
    n->z /= wrki;
}






