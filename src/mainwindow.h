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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/*главное окно*/
#include <QMainWindow>
#include "cscene3d.h"
#include "gframe_hight.h"
#include "gframe_speed.h"
#include "gframe_angular.h"
#include "help.h"
#include "author.h"
#include "license.h"
#include "frame.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QTimer         *qtmr;   // таймер
    Frame          *fr;     // окно-заставка
    gframe_speed   *graph1; // график скорости
    gframe_angular *graph2; // график угловой скорости
    gframe_hight   *graph3; // график высоты
    help           *hlp;    // справка
    author         *auth;   // об авторах
    license        *licensef; // лицензия
    bool buttonflag, io;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    /*здесь расположены события реагирующие на изменение данных в программе*/
    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_horizontalSlider_4_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_5_valueChanged(int value);

    void mbreak();

    void on_action_4_triggered();

    void on_action_5_triggered();

    void on_action_7_triggered();

    void on_action_ball_animation_triggered();

    void closeEvent(QCloseEvent *);

    void disptime();

private:
    Ui::MainWindow *ui;
    void update_gframes();
};

#endif // MAINWINDOW_H

