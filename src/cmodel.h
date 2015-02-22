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

#ifndef CMODEL_H
#define CMODEL_H
#include <QtOpenGL>
#include <cmath>
#include "main.h"

#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif
#if !defined(M_PI_2)
#define M_PI_2 1.57079632679489661923
#endif

// класс для описания 3D моделей
class Cmodel
{
protected:
    // Сетка - группа треугольников с одним материалом
    struct Mesh
    {
        int m_materialIndex;
        int m_numTriangles;
        int *m_pTriangleIndices;
    };
    int m_numMeshes;
    Mesh *m_pMeshes;
    // данные о материалах
    struct Material
    {
        float m_ambient[4], m_diffuse[4], m_specular[4], m_emissive[4];
        float m_shininess;
        GLuint m_texture;
        char *m_pTextureFilename;
    };
    int m_numMaterials;
    Material *m_pMaterials;
    // данные о треугольниках
    struct Triangle
    {
        float m_vertexNormals[3][3];
        float m_s[3], m_t[3];
        int m_vertexIndices[3];
    };
    int m_numTriangles;
    Triangle *m_pTriangles;
    // данные о вершинах
    struct Vertex
    {
        char m_boneID;
        float m_location[3];
    };
    int m_numVertices;
    Vertex *m_pVertices;

public:
    // конструктор модели
    Cmodel();
    // деструктор модели
    virtual ~Cmodel();
    // загрузка модели
    virtual bool loadModelData(const char *filename) = 0;
    // вывод модели на экран
    void draw();
    // перезагрузка текстур при потере контекста
    void reloadTextures();
};

// класс, создержащий 3D модели в формате MilkShape 3D
class CMilkshapeModel: public Cmodel
{
public:
    // конструктор модели
    CMilkshapeModel();
    // деструктор модели
    virtual ~CMilkshapeModel();
    // загрузка модели
    virtual bool loadModelData(const char *filename);
protected:
    // просчет нормалей
    void normal_gen();
};

// класс наклонный участок
class m_track_s1: public CMilkshapeModel
{
protected:
    // параметры для реализации изменения размера модели
    Vertex *vert_backup;
    float o_length, o_beta, o_rloop;
public:
    // конструктор модели
    m_track_s1();
    // деструктор модели
    ~m_track_s1();
    // изменение размера модели
    void resize(float length, float beta, float rloop);
};

// класс входной сектор в петлю
class m_track_s2: public CMilkshapeModel
{
protected:
    // параметры для реализации изменения размера модели
    float o_radius, o_beta;
    Vertex *vert_backup;
    // функция сжатия сектора
    static inline void convert_point(float betta, float x_old, float y_old, float *x_new, float *y_new)
    {
        float r = sqrt(x_old * x_old + y_old * y_old);
        float fi = asin(fabs(y_old) / r) + M_PI;
        fi = (2.0 * betta * fi / M_PI) + (3.0 * M_PI_2) - (3.0 * betta);
        *x_new = r * cos(fi);
        *y_new = r * sin(fi);
    }
public:
    // конструктор модели
    m_track_s2();
    // деструктор модели
    ~m_track_s2();
    // изменение размера модели
    void resize(float radius, float beta);
};

// класс петля
class m_track_s3: public CMilkshapeModel
{
protected:
    // параметры для реализации изменения размера модели
    Vertex *vert_backup;
    float o_radius;
public:
    // конструктор модели
    m_track_s3();
    // деструктор модели
    ~m_track_s3();
    // изменение размера модели
    void resize(float radius);
};

// класс горизонтальный участок желоба
class m_track_s4: public CMilkshapeModel
{
protected:
    // параметры для реализации изменения размера модели
    float o_length, o_rloop;
    Vertex *vert_backup;
public:
    // конструктор модели
    m_track_s4();
    // деструктор модели
    ~m_track_s4();
    // изменение размера модели
    void resize(float length, float rloop);
};

// класс шарик
class m_sphere: public CMilkshapeModel
{
protected:
    // параметры для реализации изменения размера модели
    float o_radius;
    Vertex *vert_backup;
public:
    // конструктор модели
    m_sphere();
    // деструктор модели
    ~m_sphere();
    // изменение размера модели
    void resize(float radius);
};

#endif // CMODEL_H

