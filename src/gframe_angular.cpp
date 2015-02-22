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

#include "gframe_angular.h"
#include "ui_gframe_angular.h"
#include "main.h"

gframe_angular::gframe_angular(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gframe_angular)
{
    ui->setupUi(this);
    setCentralWidget(ui->widget);   
    ui->widget->axisX = trUtf8("t, c");
    ui->widget->axisY = trUtf8("w, рад/с");
    update();
}

gframe_angular::~gframe_angular()
{    
    delete ui;   
}

void gframe_angular::update()
{
    clear();
    iter = maction;
    unsigned long local_tot = 0;
    double max_y = 0.0;
    int skip_flag = 0;
    // WARNING!!!
    iter.ktime = 1.0;
    // WARNING!!!
    while(iter.ff == 0)
    {
        iter.Refresh(local_tot * iter.ktime);
        if(iter.w > max_y)
            max_y = iter.w;
        if(!skip_flag)
        {
            ui->widget->masY.push_back((float)iter.w);
            ui->widget->masX.push_back((float)(local_tot*iter.ktime/1000.0));
            skip_flag = - scene2d_skip_points;
        }
        else
        {
            skip_flag++;
        }
        local_tot += 10;
    }

    ui->widget->resize(0.0f, ui->widget->masX[ui->widget->masX.size() - 1], 0.0, max_y);

    for(size_t i = 0; i < ui->widget->masX.size(); i++)
    {
        ui->widget->masX[i] *= ui->widget->scale_x;
        ui->widget->masY[i] *= ui->widget->scale_y;
    }

    repaint();
}

void gframe_angular::clear()
{
    ui->widget->masX.clear();
    ui->widget->masY.clear();
}

