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

#include "ModelTrackS1.h"
#include "ModelInfo.h"
#include <cmath>

#if !defined (M_PI_2)
#define M_PI_2 1.57079632679489661923
#endif

ModelTrackS1::ModelTrackS1()
    : m_verticlesBackup(NULL), m_oldLength(-1.0f), m_oldBeta(-1.0), m_oldLoopRadius(-1.0f)
{}

ModelTrackS1::~ModelTrackS1()
{
    delete [] m_verticlesBackup;
}

/// @brief изменение размера модели
void ModelTrackS1::resize(float length, float beta, float loopRadius, float sphereRadius)
{
    if(std::fabs((m_oldLength - length) / length) > 0.001f || std::fabs((m_oldBeta - beta) / beta) > 0.001f || std::fabs((m_oldLoopRadius - loopRadius) / loopRadius) > 0.001f)
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
        m_oldLength = length;
        m_oldLoopRadius = loopRadius;
        //изменение размеров
        const float epsilon = 0.5;
        float tmp1 = loopRadius / ModelInfo::loopRadius();
        float tmp2 = static_cast<float>(sphereRadius * std::sqrt(2.0) * std::sin(beta));
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = m_verticlesBackup[i].m_location[0] * tmp1;
            m_pVertices[i].m_location[2] = (m_verticlesBackup[i].m_location[2] - ModelInfo::loopWidth()) * tmp1;
            if(std::fabs(m_verticlesBackup[i].m_location[1]) > epsilon)
                m_pVertices[i].m_location[1] = m_verticlesBackup[i].m_location[1] * length + tmp2;
            else
                m_pVertices[i].m_location[1] = m_verticlesBackup[i].m_location[1];
        }
        //поворот осей координат
        tmp1 = std::cos(beta - static_cast<float>(M_PI_2));
        tmp2 = std::sin(beta - static_cast<float>(M_PI_2));
        for(int i = 0; i < m_numVertices; i++)
        {
            float old_location_x = m_pVertices[i].m_location[0], old_location_y = m_pVertices[i].m_location[1];
            m_pVertices[i].m_location[0] = old_location_x * tmp1 + old_location_y * tmp2;
            m_pVertices[i].m_location[1] = - old_location_x * tmp2 + old_location_y * tmp1;
        }
        //сдвиг осей координат
        tmp1 = loopRadius * std::sin(beta);
        tmp2 = loopRadius * std::cos(beta);
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] -= tmp1;
            m_pVertices[i].m_location[1] -= tmp2;
        }
        // просчет нормалей
        generateNormales(0.75f);
    }
}

