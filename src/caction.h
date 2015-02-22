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

#ifndef CACTION_H
#define CACTION_H
#include <cmath>

#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif

/*класс моделирующий движение шарика*/
class CAction
{
public:
    int flag,ff;
    double g,Beta,RSph,L,RLoop,x,y,z,Vx,Vy,y0,h,yi,xi,x1i,y1i,s,ti,w,/*fita,eps,epsilonR,a,*/v,t1,central,xprev,t,x0,zprev,ktime;

    /*Конструктор*/
    CAction(double Ngravity,double NBetaAngle,double NsphereR,double NFirstLength,double NLoopR);
    /*explicit CAction();*/
    CAction() {}
    ~CAction();	//Деструктор
    /*Движение*/
    void Refresh(double Ntime);

    // CHECKME: угол поворота шарика относительно своей оси
    double rotate_angle, t_old;
};

#endif

