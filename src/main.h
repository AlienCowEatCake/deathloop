/*
   Copyright (C) 2011-2015,
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

#ifndef MAIN_H
#define MAIN_H
/*главный заголовочный файл*/
#include "caction.h"

// размер 3D модели петли
static const float def_rloop = 4.810161f;
// ширина 3D модели петли
static const float def_dz = 0.5f;
// радиус 3D модели шара
static const float def_rsph = 0.6f;
extern double tot;

extern CAction maction;

// сколько точек пропускать при рисовании графика
static const int scene2d_skip_points = 2;

#endif // MAIN_H

