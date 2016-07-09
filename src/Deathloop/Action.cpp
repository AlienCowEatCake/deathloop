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

#include "Action.h"
#include "ModelInfo.h"
#include <cmath>

#if !defined (M_PI)
#define M_PI 3.14159265358979323846
#endif

namespace {

/// @brief Шаг решения уравнения
double h = 0.0001;

/// @brief Решение дифференциального уравнения
double diff(double x0, double y0, double y1, double x2, double *v, double g, double R, double r)
{
    int s = 1;
    if(x0 > x2) s*= -1;
    double xi = x0;
    double yi = y0;
    double y1i = y1;
    while(std::fabs(xi - x2)>h)
    {
        y1i = y1i - h * (5.0/9.0 * g * std::sin(yi) / (R - r * std::sqrt(2.0)));
        yi = yi + h * y1i;
        xi = xi + s * h;
    };
    *v = y1i;
    return yi;
}

} // namespace

/// @brief Конструктор (инициализация начальных значений
Action::Action(double Ngravity,double NBetaAngle,double NsphereR,double NFirstLength,double NLoopR)
{
    g = Ngravity;
    Beta = NBetaAngle;
    RSph = NsphereR;
    L = NFirstLength;
    RLoop = NLoopR;
    Vx = 0.0;
    Vy = 0.0;
    ff = StateNormal;
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
    z = - ModelInfo::loopWidth() * RLoop / ModelInfo::loopRadius();
    zprev = z;
    y0 = L * std::sin(Beta) + RSph * std::sqrt(2.0) * std::cos(Beta) + (RLoop - RLoop * std::cos(Beta));
    y = y0;
    x0 = std::sin(Beta) * RSph * std::sqrt(2.0);
    x = x0;
    flag = 0;
    z = - ModelInfo::loopWidth() * RLoop / ModelInfo::loopRadius();

    // CHECKME: угол поворота шарика относительно своей оси
    rotate_angle = 0.0;
    t_old = 0.0;
}

/// @brief Метод, вычисляющий положение шарика, его скорость, угловую скорость, ускорение и т. д.
void Action::Refresh(double Ntime)
{
    double t = Ntime/1000.0;
    // CHECKME раньше это было 0.01*ktime
    double dt = t - t_old;

    if((x < L*std::cos(Beta) + 3*RLoop) && (!(ff == StateFallDown)))
    {
        /*Движение по наклонному участку*/
        if(flag == 0)
        {
            /*Вычисление текущих координат*/
            x = x0 + g * t * t * std::sin(Beta) * std::cos(Beta) / 3.6;
            y = y0 - g * t * t * std::sin(Beta) * std::sin(Beta) / 3.6;
            /*Вычисление скоростей и углового ускорения*/
            Vx = g * t * std::sin(2 * (Beta)) / 3.6;
            Vy = - g * t * std::sin(Beta) * std::sin(Beta) / 1.8;
            //eps = g * std::sin(Beta) * RSph / 0.9 / std::sqrt(2.0);
            //fita = - eps * t * t / 2.0;
            v = std::sqrt(Vx * Vx + Vy * Vy);
            w = v / (RLoop - RSph * std::sqrt(2.0)); // w = std::sqrt(Vx * Vx + Vy * Vy) / (RLoop - RSph * std::sqrt(2.0));
            central = - Beta;
        }
        /*Движение в петле*/
        if((flag == 1) || (flag == 2) || (flag == 3))
        {
            /*Вычисление координат*/
            x = (RLoop - RSph * std::sqrt(2.0)) * std::sin(central) + (L * std::cos(Beta) + RLoop * std::sin(Beta));
            y = - (RLoop - RSph * std::sqrt(2.0)) * std::cos(central) + RLoop;
            /*Вычисление ускорений, в т. ч. и углового относительно центра петли*/
            //a = - 5.0 * g * std::sin(central) / 9.0;
            //eps = a * std::sqrt(2.0) /RSph;
            //epsilonR = a / (RLoop - RSph * std::sqrt(2.0));
            /*Вычисление угла (решение в полярных координатах*/
            central = diff(t, central, w, t + dt, &w, g, RLoop, RSph);
            /*Вычисление скорости*/
            v = w * (RLoop - RSph*std::sqrt(2.0));
            /*Движение со смещением по оси z*/
            if((flag == 2) || (flag == 3))
            {
                z = - ModelInfo::loopWidth() * RLoop / ModelInfo::loopRadius() + central / M_PI * ModelInfo::loopWidth() * RLoop / ModelInfo::loopRadius();
                zprev = z;
            }
            if(w < 0)   ff = StateGoBack;/*Шарик не смог подняться, начал катиться по петле обратно*/
            x0 = x;
            t1 = t;
        }
        /*Движение по прямому участку после прохождения петли*/
        if(flag == 4)
        {
            x = x0 + v*(t - t1);
        }
        /*Условия, регламентирующие движение в зависимости от положения шарика в пределах модели*/
        if((x < (L*std::cos(Beta) + RSph*std::sqrt(2.0)*std::sin(Beta))) && (flag == 0)) flag = 0;
        if((x >= L*std::cos(Beta) + RSph*std::sqrt(2.0)*std::sin(Beta)) && ((flag == 0) || (flag == 1)) && (x < L*std::cos(Beta) + RLoop*std::sin(Beta))) flag = 1;
        if((x >= L*std::cos(Beta) + RLoop*std::sin(Beta)) && ((flag == 1) || (flag == 2)))	flag = 2;
        if((x >= L*std::cos(Beta) + RSph*std::sqrt(2.0)*std::sin(Beta)) && ((flag == 2) || (flag == 3)) && (x < L*std::cos(Beta) + RLoop*std::sin(Beta))) flag = 3;
        if((x >= L*std::cos(Beta) + RLoop*std::sin(Beta)) && ((flag == 3) || (flag == 4)))	flag = 4;

        if((v * v / (RLoop - RSph * std::sqrt(2.0)) < g * std::cos(central + M_PI)) && (std::cos(central) < 0))
            ff = StateFallDown; /*Шарик не может преодолеть петлю, начинается падение*/
        xprev = x;
        if(std::cos(central)>0)
            Vx = v * std::sin(central);
        else
            Vx = -v * std::sin(central);
    }
    else
    {
        if(x > L*std::cos(Beta) + 3*RLoop)
            ff = StateFinished;
    }/*Шарик прошёл мёртвую петлю*/

    // CHECKME: угол поворота шарика относительно своей оси
    if(ff == StateNormal)
    {
        double w_ball = v / RSph * std::sqrt(2.0);
        rotate_angle += w_ball * (t - t_old);
        if(rotate_angle > 2.0 * M_PI) rotate_angle -= 2.0 * M_PI;
        t_old = t;
    }

    // Ошибка при моделировании: провалился сквозь петлю
    if(y < -2.0 * RSph) ff=StateError;
}

/// @brief Сброс параметров
void Action::Reset()
{
    h = 0.0;
    yi = 0.0;
    xi = 0.0;
    x1i = 0.0;
    y1i = 0.0;
    s = 0.0;
    ti = 0.0;
    //fita = 0.0;
    //epsilonR = 0.0;
    t1 = 0.0;
    central = -Beta;
    zprev = 0.0;

    x0 = std::sin(Beta) * RSph * std::sqrt(2.0);
    x = x0;
    flag = 0;
    z = - ModelInfo::loopWidth() * RLoop / ModelInfo::loopRadius();
    t = 0;
    xprev = 0;
    y0 = L * std::sin(Beta) + RSph * std::sqrt(2.0) * std::cos(Beta) + (RLoop - RLoop * std::cos(Beta));
    y = y0;
    v = 0;
    ff = StateNormal;
    Vx = 0;
    Vy = 0;
    //eps = 0;
    //a = 0;
    w = 0;

    // CHECKME: угол поворота шарика относительно своей оси
    rotate_angle = 0.0;
    t_old = 0.0;
}

