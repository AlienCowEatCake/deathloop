/* 
   Copyright (C) 2011-2014,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

   This file is part of the `deathloop' program.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(HAVE_QT5)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <cmath>
#include "cscene3d.h"
#include "main.h"

#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif

double tot = 0.0;
int vfps = 10;
const float rad2deg = 180.0f / (float)M_PI;
int id_timer;
CAction maction(9.8, 1, 0.1, 20.0, 2.0);

GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 0.0f};     //фоновый свет
GLfloat light_diffuse[] = {0.35f, 0.35f, 0.35f, 0.0f};  //значение диффузного света
GLfloat light_position[] = {0.5f, 0.0f, 0.5f, 0.0f};    //позиция источника света

Cscene3D::Cscene3D(QWidget* parent) : BaseWidget(parent)
{
    defaultScene();
}

void Cscene3D::actiontime()
{
    tot += 10.0 * maction.ktime;
    maction.Refresh(tot);
}


void Cscene3D::timerEvent(QTimerEvent *) // событие таймера
{
    updateGL(); // обновление изображения
}

void Cscene3D::initializeGL()
{
    qglClearColor(Qt::black);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    glEnable(GL_CULL_FACE);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    track1.loadModelData(":/data/m_track_s1.ms3d");
    track2.loadModelData(":/data/m_track_s2.ms3d");
    track3.loadModelData(":/data/m_track_s3.ms3d");
    track4.loadModelData(":/data/m_track_s1.ms3d");
    sphere.loadModelData(":/data/m_sphere.ms3d");

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);    // Установка Фонового Света
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);    // Установка Диффузного Света
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);   // Позиция света

    glEnable(GL_LIGHTING);                              // Разрешить освещение
    glEnable (GL_LIGHT1);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_MULTISAMPLE);
}

void Cscene3D::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();    

    GLfloat ratio=(GLfloat)nHeight/(GLfloat)nWidth;
    if (nWidth>=nHeight)
        glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -10.0, 30.0);
    else
        glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -10.0, 30.0);

    glViewport(0, 0,(GLint)nWidth, (GLint)nHeight);
}

/*основная функция рисования*/
void Cscene3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(nSca, nSca, nSca);
    glTranslatef(0.0f, zTra, 0.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot, 0.0f, 0.0f, 1.0f);

    glRotatef(90.0f, 1.0f, 0.0f,0.0f);

    track1.resize(maction.L, maction.Beta, maction.RLoop);
    track1.draw();
    track2.resize(maction.RLoop, maction.Beta);
    track2.draw();
    track3.resize(maction.RLoop);
    track3.draw();    
    track4.resize(- maction.RLoop * sin(maction.Beta) - maction.L * cos(maction.Beta)+ maction.L*cos(maction.Beta) + 3*maction.RLoop + 3*maction.RSph, maction.RLoop);
    track4.draw();    
    glTranslatef(-maction.RLoop * sin(maction.Beta) - maction.L * cos(maction.Beta),
                 -maction.RLoop,
                 0.0f);

    glTranslatef(maction.x, maction.y, maction.z);
    sphere.resize(maction.RSph);

    // CHECKME: угол поворота шарика относительно своей оси
    extern bool ball_animated;
    if(ball_animated)
        glRotatef(- maction.rotate_angle * rad2deg, 0.0f, 0.0f, 1.0f);

    sphere.draw();
}

void Cscene3D::mousePressEvent(QMouseEvent* pe)
{
    ptrMousePosition = pe->pos();
}

void Cscene3D::mouseReleaseEvent(QMouseEvent*)
{
    updateGL();
}

void Cscene3D::mouseMoveEvent(QMouseEvent* pe)
{    
    xRot += 180/1.5*(GLfloat)(pe->y()-ptrMousePosition.y())/height();
    zRot += 180/1.5*(GLfloat)(pe->x()-ptrMousePosition.x())/width();
    ptrMousePosition = pe->pos();
    updateGL();
}

void Cscene3D::wheelEvent(QWheelEvent* pe)
{
    if ((pe->delta())>0) scale_plus();
    else if ((pe->delta())<0) scale_minus();

    updateGL();
}

void Cscene3D::keyPressEvent(QKeyEvent* pe)
{
    switch (pe->key())
    {
    case Qt::Key_Plus:
        scale_plus();
        break;

    case Qt::Key_Equal:
        scale_plus();
        break;

    case Qt::Key_Minus:
        scale_minus();
        break;

    case Qt::Key_Up:
        rotate_up();
        break;

    case Qt::Key_Down:
        rotate_down();
        break;

    case Qt::Key_Left:
        rotate_left();
        break;

    case Qt::Key_Right:
        rotate_right();
        break;

    case Qt::Key_Z:
        translate_down();
        break;

    case Qt::Key_X:
        translate_up();
        break;

    case Qt::Key_Space:
        defaultScene();
        break;

    case Qt::Key_Escape:
        this->close();
        break;
    }

    updateGL();
}

void Cscene3D::scale_plus()
{
    if(nSca < 3.0f)
    {
        nSca = nSca * 1.1f;
        update_light();
    }
}

void Cscene3D::scale_minus()
{
    if(nSca > 0.005f)
    {
        nSca = nSca / 1.1f;
        update_light();
    }
}

void Cscene3D::rotate_up()
{
    xRot += 1.0f;
}

void Cscene3D::rotate_down()
{
    xRot -= 1.0f;
}

void Cscene3D::rotate_left()
{
    zRot += 1.0f;
}

void Cscene3D::rotate_right()
{
    zRot -= 1.0f;
}

void Cscene3D::translate_down()
{
    zTra -= 0.05f;
}

void Cscene3D::translate_up()
{
    zTra += 0.05f;
}

void Cscene3D::defaultScene()
{
    xRot = -90.0f;
    yRot = 0.0f;
    zRot = 0.0f;
    zTra = 0.0f;
    nSca = 0.2f;
    update_light();
}

// Обновление освещения при изменении масштаба
void Cscene3D::update_light()
{
    GLfloat light_diffuse_new[] =
    {
        light_diffuse[0] * 5.0f * nSca,
        light_diffuse[1] * 5.0f * nSca,
        light_diffuse[2] * 5.0f * nSca,
        light_diffuse[3]
    };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse_new);
}
