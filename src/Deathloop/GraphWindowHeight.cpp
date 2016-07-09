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

#include "GraphWindowHeight.h"
#include "PhysicalController.h"

GraphWindowHeight::GraphWindowHeight(QWidget *parent) :
    GraphWindowAbstract(false, parent), m_physicalController(NULL)
{}

void GraphWindowHeight::setPhysicalController(const PhysicalController * physicalController)
{
    m_physicalController = physicalController;
}

void GraphWindowHeight::update()
{
    if(!m_physicalController)
        return;
    clear();

    QVector<float> & arrX = this->arrX();
    QVector<float> & arrY = this->arrY();
    Action action(m_physicalController->action());

    // сколько точек пропускать при рисовании графика
    const int skipPoints = 2;

    unsigned long long localTime = 0;
    double maxY = 0.0;
    int skipCounter = 0;
    while(action.get_State() == Action::StateNormal)
    {
        action.Refresh(localTime);
        if(action.get_y() > maxY)
            maxY = action.get_y();
        if(!skipCounter)
        {
            arrY.push_back(static_cast<float>(action.get_y()));
            arrX.push_back(localTime / 1000.0f);
            skipCounter = - skipPoints;
        }
        else
        {
            skipCounter++;
        }
        localTime += 10;
    }

    resizeGraph(0.0f, arrX[arrX.size() - 1], 0.0f, static_cast<float>(maxY));

    for(int i = 0; i < arrX.size(); i++)
    {
        arrX[i] *= scaleX();
        arrY[i] *= scaleY();
    }

    repaint();
}

