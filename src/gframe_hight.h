/* 
   Copyright (C) 2011-2013,
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

#ifndef GFRAME_HIGHT_H
#define GFRAME_HIGHT_H

#include "cscene2dn.h"
#include "main.h"
#include <QMainWindow>

namespace Ui {
class gframe_hight;
}

class gframe_hight : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit gframe_hight(QWidget *parent = 0);
    ~gframe_hight();
    void update();
    void clear();

private:
    Ui::gframe_hight *ui;
    CAction iter;
protected:

};

#endif // GFRAME_HIGHT_H

