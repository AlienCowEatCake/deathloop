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

#include "caction.h"
#include "main.h"

/*Шаг решения уравнения*/
double h = 0.0001;

/*Решение дифференциального уравнения*/
double diff(double x0, double y0, double y1, double x2, double *v, double g, double R, double r)
{
    int s = 1;
    if(x0 > x2) s*= -1;
    double xi = x0;
    double yi = y0;
    double y1i = y1;
    while(fabs(xi - x2)>h)
    {
        y1i = y1i - h * (5.0/9.0 * g * sin(yi) / (R - r * sqrt(2.0)));
        yi = yi + h * y1i;
        xi = xi + (double)s * h;
    };
    *v = y1i;
    return yi;
}

/*Конструктор (инициализация начальных значений*/
CAction::CAction(double Ngravity,double NBetaAngle,double NsphereR,double NFirstLength,double NLoopR)
{
    g = Ngravity;
    Beta = NBetaAngle;
    RSph = NsphereR;
    L = NFirstLength;
    RLoop = NLoopR;
    Vx = 0.0;
    Vy = 0.0;
    ff = 0;
    h = 0.0001;
    yi = 0.0;
    xi = 0.0;
    x1i = 0.0;
    y1i = 0.0;
    s = 1;
    ti = 0.0;
    w = 0.0;
    //fita = 0.0;
    //eps = 0.0;
    //epsilonR = 0.0;
    //a = 0.0;
    v = 0.0;
    t1 = 0.0;
    central = - Beta;
    xprev = 0.0;
    t = 0.0;
    z = - def_dz * RLoop / def_rloop;
    zprev = z;
    y0 = L * sin(Beta) + RSph * sqrt(2.0) * cos(Beta) + (RLoop - RLoop * cos(Beta));
    y = y0;
    x0 = sin(Beta) * RSph * sqrt(2.0);
    x = x0;
    flag = 0;
    z = - def_dz * RLoop / def_rloop;
    ktime = 1;

    // CHECKME: угол поворота шарика относительно своей оси
    rotate_angle = 0.0;
    t_old = 0.0;
}

/*Деструктор*/
CAction::~CAction() {}

/*Метод, вычисляющий положение шарика, его скорость, угловую скорость, ускорение и т. д.*/
void CAction::Refresh(double Ntime)
{
    double t = Ntime/1000.0;
    if((x < L*cos(Beta) + 3*RLoop) && (!(ff == 2)))
    {
        /*Движение по наклонному участку*/
        if(flag == 0)
        {
            /*Вычисление текущих координат*/
            x = x0 + g * t * t * sin(Beta) * cos(Beta) / 3.6;
            y = y0 - g * t * t * sin(Beta) * sin(Beta) / 3.6;
            /*Вычисление скоростей и углового ускорения*/
            Vx = g * t * sin(2 * (Beta)) / 3.6;
            Vy = - g * t * sin(Beta) * sin(Beta) / 1.8;
            //eps = g * sin(Beta) * RSph / 0.9 / sqrt(2.0);
            //fita = - eps * t * t / 2.0;
            v = sqrt(Vx * Vx + Vy * Vy);
            w = v / (RLoop - RSph * sqrt(2.0)); // w = sqrt(Vx * Vx + Vy * Vy) / (RLoop - RSph * sqrt(2.0));
            central = - Beta;
        }
        /*Движение в петле*/
        if((flag == 1) || (flag == 2) || (flag == 3))
        {
            /*Вычисление координат*/
            x = (RLoop - RSph * sqrt(2.0)) * sin(central) + (L * cos(Beta) + RLoop * sin(Beta));
            y = - (RLoop - RSph * sqrt(2.0)) * cos(central) + RLoop;
            /*Вычисление ускорений, в т. ч. и углового относительно центра петли*/
            //a = - 5.0 * g * sin(central) / 9.0;
            //eps = a * sqrt(2.0) /RSph;
            //epsilonR = a / (RLoop - RSph * sqrt(2.0));
            /*Вычисление угла (решение в полярных координатах*/
            central = diff(t, central, w, t + 0.01*ktime, &w, g, RLoop, RSph);
            /*Вычисление скорости*/
            v = w * (RLoop - RSph*sqrt(2.0));
            /*Движение со смещением по оси z*/
            if((flag == 2) || (flag == 3))
            {
                z = - def_dz * RLoop / def_rloop + central / M_PI * def_dz * RLoop / def_rloop;
                zprev = z;
            }
            if(w < 0)   ff = 1;/*Шарик не смог подняться, начал катиться по петле обратно*/
            x0 = x;
            t1 = t;
        }
        /*Движение по прямому участку после прохождения петли*/
        if(flag == 4)
        {
            x = x0 + v*(t - t1);
        }
        /*Условия, регламентирующие движение в зависимости от положения шарика в пределах модели*/
        if((x < (L*cos(Beta) + RSph*sqrt(2.0)*sin(Beta))) && (flag == 0)) flag = 0;
        if((x >= L*cos(Beta) + RSph*sqrt(2.0)*sin(Beta)) && ((flag == 0) || (flag == 1)) && (x < L*cos(Beta) + RLoop*sin(Beta))) flag = 1;
        if((x >= L*cos(Beta) + RLoop*sin(Beta)) && ((flag == 1) || (flag == 2)))	flag = 2;
        if((x >= L*cos(Beta) + RSph*sqrt(2.0)*sin(Beta)) && ((flag == 2) || (flag == 3)) && (x < L*cos(Beta) + RLoop*sin(Beta))) flag = 3;
        if((x >= L*cos(Beta) + RLoop*sin(Beta)) && ((flag == 3) || (flag == 4)))	flag = 4;

        if((v * v / (RLoop - RSph * sqrt(2.0)) < g * cos(central + M_PI)) && (cos(central) < 0))
            ff = 2; /*Шарик не может преодолеть петлю, начинается падение*/
        xprev = x;
        if(cos(central)>0)
            Vx = v * sin(central);
        else
            Vx = -v * sin(central);
    }
    else
    {
        if(x > L*cos(Beta) + 3*RLoop)
            ff = 3;
    }/*Шарик прошёл мёртвую петлю*/

    // CHECKME: угол поворота шарика относительно своей оси
    if(!ff)
    {
        double w_ball = v / RSph * sqrt(2.0);
        rotate_angle += w_ball * (t - t_old);
        if(rotate_angle > 2.0 * M_PI) rotate_angle -= 2.0 * M_PI;
        t_old = t;
    }

    // Ошибка при моделировании: провалился сквозь петлю
    if(y < -2.0 * RSph) ff=4;
}

