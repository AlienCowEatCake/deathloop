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

#if !defined (GRAPHWINDOWANGULAR_H_INCLUDED)
#define GRAPHWINDOWANGULAR_H_INCLUDED

#include "widgets/GraphWindowAbstract/GraphWindowAbstract.h"

class PhysicalController;

class GraphWindowAngular : public GraphWindowAbstract
{
    Q_OBJECT
    
public:
    explicit GraphWindowAngular(QWidget *parent = 0);
    void update();

    void setPhysicalController(const PhysicalController * physicalController);

protected:
    void changeEvent(QEvent * event);
    void updateTitle();

private:
    const PhysicalController * m_physicalController;
};

#endif // GRAPHWINDOWANGULAR_H_INCLUDED

