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

#if !defined (MODELINFO_H_INCLUDED)
#define MODELINFO_H_INCLUDED

/// @brief Класс информации о характеристиках 3D моделей
class ModelInfo
{
public:

    /// @brief получить размер 3D модели петли
    static float loopRadius();
    /// @brief получить ширину 3D модели петли
    static float loopWidth();
    /// @brief получить радиус 3D модели шара
    static float sphereRadius();

private:

    /// @brief размер 3D модели петли
    static const float m_loopRadius;
    /// @brief ширина 3D модели петли
    static const float m_loopWidth;
    /// @brief радиус 3D модели шара
    static const float m_sphereRadius;
};

#endif // MODELINFO_H_INCLUDED

