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

#include "ModelSphere.h"
#include "ModelInfo.h"
#include <cmath>

ModelSphere::ModelSphere()
    :  m_verticlesBackup(NULL), m_oldRadius(-1.0f)
{}

ModelSphere::~ModelSphere()
{
    delete [] m_verticlesBackup;
}

/// @brief изменение размера модели
void ModelSphere::resize(float radius)
{
    if(std::fabs((m_oldRadius - radius) / radius) > 0.001f)
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
        m_oldRadius = radius;
        //задание радиуса
        float tmp1 = radius / ModelInfo::sphereRadius();
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = m_verticlesBackup[i].m_location[0] * tmp1;
            m_pVertices[i].m_location[1] = m_verticlesBackup[i].m_location[1] * tmp1;
            m_pVertices[i].m_location[2] = m_verticlesBackup[i].m_location[2] * tmp1;
        }
        // просчет нормалей
        generateNormales(0.75f);
    }
}

