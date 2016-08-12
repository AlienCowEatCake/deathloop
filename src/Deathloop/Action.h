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

#if !defined (ACTION_H_INCLUDED)
#define ACTION_H_INCLUDED

/// @brief Класс моделирующий движение шарика
class Action
{
public:
    /// @brief Состояния системы
    enum BallState
    {
        StateNormal   = 0,  ///< Шар нормально двигается по петле
        StateGoBack   = 1,  ///< Шарик не смог подняться, начал катиться по петле обратно
        StateFallDown = 2,  ///< Шарик не может преодолеть петлю, начинается падение
        StateFinished = 3,  ///< Шарик прошёл мёртвую петлю
        StateError    = 4   ///< Ошибка при моделировании: провалился сквозь петлю
    };

    Action(double Ngravity,double NBetaAngle,double NsphereR,double NFirstLength,double NLoopR);

    /// @brief Метод, вычисляющий положение шарика, его скорость, угловую скорость, ускорение и т. д.
    void Refresh(double Ntime);
    /// @brief Сброс параметров
    void Reset();

    /// @brief Установить угол наклона входного участка петли
    void set_BetaAngle(double value)    { Beta = value; }
    /// @brief Установить радиус шарика
    void set_SphereR(double value)      { RSph = value; }
    /// @brief Установить длину входного участка петли
    void set_FirstLength(double value)  { L = value; }
    /// @brief Установить радиус петли
    void set_LoopR(double value)        { RLoop = value; }

    /// @brief Получить текущее состояние системы
    BallState get_State() const         { return ff; }

    /// @brief Получить угол наклона входного участка петли
    double get_BetaAngle() const        { return Beta; }
    /// @brief Получить радиус шарика
    double get_SphereR() const          { return RSph; }
    /// @brief Получить длину входного участка петли
    double get_FirstLength() const      { return L; }
    /// @brief Получить радиус петли
    double get_LoopR() const            { return RLoop; }
    /// @brief Получить угол поворота шарика относительно своей оси
    double get_RotateAngle() const      {return rotate_angle; }

    /// @brief Получить текущую координату шарика по оси x
    double get_x() const    { return x; }
    /// @brief Получить текущую координату шарика по оси y
    double get_y() const    { return y; }
    /// @brief Получить текущую координату шарика по оси z
    double get_z() const    { return z; }
    /// @brief Получить угловую скорость шара
    double get_w() const    { return w; }
    /// @brief Получить скорость шара
    double get_v() const    { return v; }

private:
    BallState ff;
    int flag;
    double g,Beta,RSph,L,RLoop,x,y,z,Vx,Vy,y0,h,yi,xi,x1i,y1i,s,ti,w,/*fita,eps,epsilonR,a,*/v,t1,central,xprev,t,x0,zprev;

    // CHECKME: угол поворота шарика относительно своей оси
    double rotate_angle, t_old;
};

#endif // ACTION_H_INCLUDED

