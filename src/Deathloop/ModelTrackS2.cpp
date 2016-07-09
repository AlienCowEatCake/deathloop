/* 
   Copyright (C) 2011-2016,
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

#include "ModelTrackS2.h"
#include "ModelInfo.h"
#include <cmath>

#if !defined (M_PI)
#define M_PI 3.14159265358979323846
#endif
#if !defined (M_PI_2)
#define M_PI_2 1.57079632679489661923
#endif

namespace {

// функция сжатия сектора
void convert_point(float beta, float xOld, float yOld, float & xNew, float & yNew)
{
    float r = std::sqrt(xOld * xOld + yOld * yOld);
    float fi = std::asin(std::fabs(yOld) / r) + static_cast<float>(M_PI);
    fi = (2.0f * beta * fi / static_cast<float>(M_PI)) + (3.0f * static_cast<float>(M_PI_2)) - (3.0f * beta);
    xNew = r * std::cos(fi);
    yNew = r * std::sin(fi);
}

} // namespace

ModelTrackS2::ModelTrackS2()
    : m_verticlesBackup(NULL), m_oldLoopRadius(-1.0f), m_oldBeta(-1.0)
{}

ModelTrackS2::~ModelTrackS2()
{
    delete [] m_verticlesBackup;
}

/// @brief изменение размера модели
void ModelTrackS2::resize(float loopRadius, float beta)
{
    if(std::fabs((m_oldLoopRadius - loopRadius) / loopRadius) > 0.001f || std::fabs((m_oldBeta - beta) / beta) > 0.001f)
    {
        //копия вершин
        if(!m_verticlesBackup)
        {
            m_verticlesBackup = new Vertex [m_numVertices];
            for(int i = 0; i < m_numVertices; i++)
            {
                m_verticlesBackup[i].m_location[0] = m_pVertices[i].m_location[0];
                m_verticlesBackup[i].m_location[1] = m_pVertices[i].m_location[1];
                m_verticlesBackup[i].m_location[2] = m_pVertices[i].m_location[2];
            }
        }
        //сохранение параметров
        m_oldBeta = beta;
        m_oldLoopRadius = loopRadius;
        //задание радиуса
        float tmp1 = loopRadius / ModelInfo::loopRadius();
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = m_verticlesBackup[i].m_location[0] * tmp1;
            m_pVertices[i].m_location[1] = m_verticlesBackup[i].m_location[1] * tmp1;
            m_pVertices[i].m_location[2] = m_verticlesBackup[i].m_location[2] * tmp1;
        }
        //сжатие
        for(int i = 0; i < m_numVertices; i++)
        {
            convert_point(beta, m_pVertices[i].m_location[0], m_pVertices[i].m_location[1],
                          m_pVertices[i].m_location[0], m_pVertices[i].m_location[1]);
        }
        // просчет нормалей
        generateNormales(0.75f);
    }
}

