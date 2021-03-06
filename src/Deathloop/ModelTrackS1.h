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

#if !defined (MODELTRACKS1_H_INCLUDED)
#define MODELTRACKS1_H_INCLUDED

#include "models/MilkshapeModel.h"

/// @brief Класс наклонный участок
class ModelTrackS1: public MilkshapeModel
{
public:
    ModelTrackS1();
    ~ModelTrackS1();

    /// @brief изменение размера модели
    void resize(float length, float beta, float loopRadius, float sphereRadius);

private:
    // параметры для реализации изменения размера модели
    Vertex * m_verticlesBackup;
    float m_oldLength, m_oldBeta, m_oldLoopRadius;
};

#endif // MODELTRACKS1_H_INCLUDED

