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

#include "ModelTrackS4.h"
#include "ModelInfo.h"
#include <cmath>

ModelTrackS4::ModelTrackS4()
    : m_verticlesBackup(NULL)
    , m_oldLength(-1.0f)
    , m_oldLoopRadius(-1.0f)
{}

ModelTrackS4::~ModelTrackS4()
{
    delete [] m_verticlesBackup;
}

void ModelTrackS4::resize(float length, float rloop)
{
    if(std::fabs((m_oldLength - length) / length) > 0.001f || std::fabs((m_oldLoopRadius - rloop) / rloop) > 0.001f)
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
        m_oldLoopRadius = rloop;
        m_oldLength = length;
        //изменение размеров
        const float epsilon = 0.5f;
        const float tmp1 = rloop / ModelInfo::loopRadius();
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = m_verticlesBackup[i].m_location[0] * tmp1;
            m_pVertices[i].m_location[2] = (m_verticlesBackup[i].m_location[2] + ModelInfo::loopWidth()) * tmp1;
            if(std::fabs(m_verticlesBackup[i].m_location[1]) > epsilon)
                m_pVertices[i].m_location[1] = m_verticlesBackup[i].m_location[1] * length;
            else
                m_pVertices[i].m_location[1] = m_verticlesBackup[i].m_location[1];
        }
        //поворот осей координат
        for(int i = 0; i < m_numVertices; i++)
        {
            const float old_location_x = m_pVertices[i].m_location[0], old_location_y = m_pVertices[i].m_location[1];
            m_pVertices[i].m_location[0] = - old_location_y;
            m_pVertices[i].m_location[1] = old_location_x;
        }
        //сдвиг осей координат
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] += length;
            m_pVertices[i].m_location[1] -= rloop;
        }
        // просчет нормалей
        generateNormales(0.75f);
    }
}

