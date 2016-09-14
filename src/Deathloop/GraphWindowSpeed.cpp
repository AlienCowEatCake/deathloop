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

#include "GraphWindowSpeed.h"

#include <QEvent>
#include "PhysicalController.h"

GraphWindowSpeed::GraphWindowSpeed(QWidget *parent) :
    GraphWindowAbstract(false, parent), m_physicalController(NULL)
{
    updateTitle();
}

void GraphWindowSpeed::setPhysicalController(const PhysicalController * physicalController)
{
    m_physicalController = physicalController;
}

void GraphWindowSpeed::update()
{
    if(!m_physicalController)
        return;
    clear();

    QVector<float> & arrX = this->arrX();
    QVector<float> & arrY = this->arrY();
    Action action(m_physicalController->action());
    action.Reset();

    // сколько точек пропускать при рисовании графика
    const int skipPoints = 2;

    unsigned long long localTime = 0;
    double maxY = 0.0;
    int skipCounter = 0;
    while(action.get_State() == Action::StateNormal)
    {
        action.Refresh(localTime);
        if(action.get_v() > maxY)
            maxY = action.get_v();
        if(!skipCounter)
        {
            arrY.push_back(static_cast<float>(action.get_v()));
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

void GraphWindowSpeed::changeEvent(QEvent * event)
{
    GraphWindowAbstract::changeEvent(event);
    if(event->type() == QEvent::LanguageChange)
        updateTitle();
}

void GraphWindowSpeed::updateTitle()
{
    setLabels(tr("Ball Speed"), tr("t, s"), tr("v, m/s"));
}

