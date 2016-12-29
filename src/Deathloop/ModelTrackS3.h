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

#if !defined (MODELTRACKS3_H_INCLUDED)
#define MODELTRACKS3_H_INCLUDED

#include "models/MilkshapeModel.h"

/// @brief Класс петля
class ModelTrackS3: public MilkshapeModel
{
public:
    ModelTrackS3();
    ~ModelTrackS3();

    /// @brief изменение размера модели
    void resize(float loopRadius);

private:
    // параметры для реализации изменения размера модели
    Vertex * m_verticlesBackup;
    float m_oldLoopRadius;
};

#endif // MODELTRACKS3_H_INCLUDED

