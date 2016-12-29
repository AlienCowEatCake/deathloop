/*
   Copyright (C) 2011-2016,
        Andrei V. Kurochkin     <kurochkin.andrei.v@yandex.ru>
        Mikhail E. Aleksandrov  <alexandroff.m@gmail.com>
        Peter S. Zhigalov       <peter.zhigalov@gmail.com>

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

#include "Scene3D.h"

#include <cmath>
#include "PhysicalController.h"

#if !defined (M_PI)
#define M_PI 3.14159265358979323846
#endif

Scene3D::Scene3D(QWidget* parent)
    : Scene3DAbstract(parent)
    , m_physicalController(NULL)
    , m_ballAnimated(true)
{
    const GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 0.0f};     // фоновый свет
    const GLfloat light_diffuse[] = {0.35f, 0.35f, 0.35f, 0.0f};  // значение диффузного света
    const GLfloat light_position[] = {0.5f, 0.0f, 0.5f, 0.0f};    // позиция источника света
    setDefaultScene(SceneDefault(SceneParameters(-90.0f, 0.0f, 0.0f, 0.0f, 0.2f), 0.005f, 3.0f));
    setLightParameters(LightParameters(GL_LIGHT0, light_ambient, light_diffuse, light_position, 5.0f));
}

/// @brief setPhysicalController - установить контроллер физического процесса
void Scene3D::setPhysicalController(const PhysicalController * physicalController)
{
    m_physicalController = physicalController;
}

/// @brief initializeGL - инициализировать OpenGL-сцену
void Scene3D::initializeGL()
{
    this->qglClearColor(Qt::black);
    GLImpl::glEnable(GL_DEPTH_TEST);
    GLImpl::glShadeModel(GL_FLAT);
    GLImpl::glEnable(GL_CULL_FACE);

    GLImpl::glEnableClientState(GL_VERTEX_ARRAY);
    GLImpl::glEnableClientState(GL_COLOR_ARRAY);

    m_track1.loadModelData(QLatin1String(":/data/m_track_s1.ms3d"));
    m_track2.loadModelData(QLatin1String(":/data/m_track_s2.ms3d"));
    m_track3.loadModelData(QLatin1String(":/data/m_track_s3.ms3d"));
    m_track4.loadModelData(QLatin1String(":/data/m_track_s1.ms3d"));
    m_sphere.loadModelData(QLatin1String(":/data/m_sphere.ms3d"));

    GLImpl::glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient(GL_LIGHT0));    // Установка Фонового Света
    GLImpl::glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse(GL_LIGHT0));    // Установка Диффузного Света
    GLImpl::glLightfv(GL_LIGHT0, GL_POSITION, lightPosition(GL_LIGHT0));  // Позиция света

    GLImpl::glEnable(GL_LIGHTING);  // Разрешить освещение
    GLImpl::glEnable (GL_LIGHT0);

    GLImpl::glShadeModel(GL_SMOOTH);

    GLImpl::glEnable(GL_MULTISAMPLE);
}

/// @brief paintGL - отрисовать OpenGL-сцену
void Scene3D::paintGL()
{
    if(!m_physicalController)
        return;

    GLImpl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLImpl::glMatrixMode(GL_MODELVIEW);
    GLImpl::glLoadIdentity();

    GLImpl::glScalef(nSca(), nSca(), nSca());
    GLImpl::glTranslatef(0.0f, zTra(), 0.0f);
    GLImpl::glRotatef(xRot(), 1.0f, 0.0f, 0.0f);
    GLImpl::glRotatef(yRot(), 0.0f, 1.0f, 0.0f);
    GLImpl::glRotatef(zRot(), 0.0f, 0.0f, 1.0f);

    GLImpl::glRotatef(90.0f, 1.0f, 0.0f,0.0f);

    const Action & action = m_physicalController->action();
    const float rad2deg = 180.0f / static_cast<float>(M_PI);
    const float loopRadius = static_cast<float>(action.get_LoopR());
    const float sphereRadius = static_cast<float>(action.get_SphereR());
    const float beta = static_cast<float>(action.get_BetaAngle());
    const float length = static_cast<float>(action.get_FirstLength());
    const float sinBeta = std::sin(beta);
    const float cosBeta = std::cos(beta);

    m_track1.resize(length, beta, loopRadius, sphereRadius);
    m_track1.draw();
    m_track2.resize(loopRadius, beta);
    m_track2.draw();
    m_track3.resize(loopRadius);
    m_track3.draw();
    m_track4.resize(- loopRadius * sinBeta + 3.0f * loopRadius + 3.0f * sphereRadius, loopRadius);
    m_track4.draw();
    GLImpl::glTranslatef(- loopRadius * sinBeta - length * cosBeta, - loopRadius, 0.0f);

    GLImpl::glTranslatef(static_cast<float>(action.get_x()), static_cast<float>(action.get_y()), static_cast<float>(action.get_z()));
    m_sphere.resize(sphereRadius);

    // CHECKME: угол поворота шарика относительно своей оси
    if(m_ballAnimated)
        GLImpl::glRotatef(- static_cast<float>(action.get_RotateAngle()) * rad2deg, 0.0f, 0.0f, 1.0f);

    m_sphere.draw();
}

/// @brief ballAnimated - узнать, включена ли анимация шарика при отрисовке сцены
bool Scene3D::ballAnimated() const
{
    return m_ballAnimated;
}

/// @brief setBallAnimated - включить или выключить анимацию шарика при отрисовке сцены
void Scene3D::setBallAnimated(bool ballAnimated)
{
    m_ballAnimated = ballAnimated;
}
