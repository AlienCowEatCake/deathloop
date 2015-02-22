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

#if defined(HAVE_QT5)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "main.h"

#if defined(_WIN32) || defined(WIN32)

#include <QFont>
#include <QFontDatabase>

#if defined(HAVE_OLD_QT)
#include <windows.h>
#endif

// Фикс шрифтов (замена стандартных на Tahoma, если стандартные не поддерживают кириллицу)
template<typename T> void fix_fonts(T * widget)
{
    static QFontDatabase qfd;
#if defined(HAVE_OLD_QT)
    static DWORD dwVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));
    if(!qfd.families(QFontDatabase::Cyrillic).contains(widget->font().family(), Qt::CaseInsensitive) || dwVersion <= 4)
#else
    if(!qfd.families(QFontDatabase::Cyrillic).contains(widget->font().family(), Qt::CaseInsensitive))
#endif
    {
        QFont font_tahoma = widget->font();
        font_tahoma.setFamily("Tahoma");
        widget->setFont(font_tahoma);
    }
}

#endif

bool ball_animated = false;

//функция сброса параметров класса maction
void caction_reset()
{
    maction.h = 0.0;
    maction.yi = 0.0;
    maction.xi = 0.0;
    maction.x1i = 0.0;
    maction.y1i = 0.0;
    maction.s = 0.0;
    maction.ti = 0.0;
    //maction.fita = 0.0;
    //maction.epsilonR = 0.0;
    maction.t1 = 0.0;
    maction.central = -maction.Beta;
    maction.zprev = 0.0;

    maction.x0 = sin(maction.Beta) * maction.RSph * sqrt(2.0);
    maction.x = maction.x0;
    maction.flag = 0;
    maction.z = - def_dz * maction.RLoop / def_rloop;
    maction.t = 0;
    maction.xprev = 0;
    maction.y0 = maction.L * sin(maction.Beta) + maction.RSph * sqrt(2.0) * cos(maction.Beta) + (maction.RLoop - maction.RLoop * cos(maction.Beta));
    maction.y = maction.y0;
    maction.v = 0;
    maction.ff = 0;
    maction.Vx = 0;
    maction.Vy = 0;
    //maction.eps = 0;
    //maction.a = 0;
    maction.w = 0;

    // CHECKME: угол поворота шарика относительно своей оси
    maction.rotate_angle = 0.0;
    maction.t_old = 0.0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#if defined(VERSION_NUMBER)
    setWindowTitle(trUtf8("Мертвая петля") + QString(" ") + VERSION_NUMBER);
#else
    setWindowTitle(trUtf8("Мертвая петля"));
#endif
    setCentralWidget(ui->widget);
    //ui->statusbar->hide();
    qtmr = new QTimer(this);
    //соединение таймера с виджетами
    connect(qtmr, SIGNAL(timeout()), ui->widget ,SLOT(actiontime()));
    connect(qtmr, SIGNAL(timeout()), ui->widget ,SLOT(updateGL()));
    connect(qtmr, SIGNAL(timeout()), SLOT(mbreak()));
    buttonflag = false;
    io = false;
    caction_reset();
    graph1 = new gframe_speed;
    graph1->setHidden(true);
    graph2 = new gframe_angular;
    graph2->setHidden(true);
    graph3 = new gframe_hight;
    graph3->setHidden(true);
    hlp = new help;
    hlp->setHidden(true);
    auth = new author;
    auth->setHidden(true);
    licensef = new license;
    licensef->setHidden(true);
    ui->pushButton_3->setText(trUtf8("Сброс"));
    ui->widget->updateGL();

    // Заполнение параметров по умолчанию
    // угол наклона 60 градусов
    ui->horizontalSlider_5->setValue(60);
    this->on_horizontalSlider_5_valueChanged(60);
    // длина 6 м
    ui->horizontalSlider->setValue(6);
    this->on_horizontalSlider_valueChanged(6);
    // радиус петли 2 м
    ui->horizontalSlider_2->setValue(20);
    this->on_horizontalSlider_2_valueChanged(20);
    // радиус шара 0.3 м
    ui->horizontalSlider_3->setValue(3);
    this->on_horizontalSlider_3_valueChanged(3);
    // скорость эксперимента 100%
    ui->horizontalSlider_4->setValue(100);
    this->on_horizontalSlider_4_valueChanged(100);

    // Анимация вращения шарика
    ball_animated = true;
    ui->action_ball_animation->setChecked(ball_animated);

    // О Qt
    connect(ui->action_about_qt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));

    // Индкатор времени
    ui->lcdNumber->setDecMode();
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    connect(qtmr,SIGNAL(timeout()),this,SLOT(disptime()));

    // Фикс шрифтов
#if defined(_WIN32) || defined(WIN32)
    fix_fonts(this);
    fix_fonts(ui->menubar);
    fix_fonts(ui->groupBox);
    fix_fonts(ui->groupBox_2);
    fix_fonts(ui->groupBox_3);
    fix_fonts(ui->groupBox_4);
    fix_fonts(ui->label);
    fix_fonts(ui->label_5);
    fix_fonts(ui->label_6);
    fix_fonts(ui->label_8);
    fix_fonts(ui->label_9);
    fix_fonts(ui->label_10);
    fix_fonts(ui->label_11);
    fix_fonts(ui->menu_2);
    fix_fonts(ui->menu_3);
    fix_fonts(ui->menu_4);
    fix_fonts(ui->pushButton_2);
    fix_fonts(ui->pushButton_3);
    fix_fonts(ui->action);
    fix_fonts(ui->action_2);
    fix_fonts(ui->action_3);
    fix_fonts(ui->action_4);
    fix_fonts(ui->action_5);
    fix_fonts(ui->action_7);
    fix_fonts(ui->action_about_qt);
    fix_fonts(ui->action_ball_animation);
#endif

    // окно-заставка
    fr = new Frame;
    fr->show();
    fr->startTimer(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_3_triggered()
{
    if(graph1->isHidden())
        graph1->setHidden(false);
    else
        graph1->setHidden(true);
}

void MainWindow::on_action_2_triggered()
{
    if(auth->isHidden())
        auth->setHidden(false);
    else
        auth->setHidden(true);
}

void MainWindow::on_action_triggered()
{
    if(hlp->isHidden())
        hlp->setHidden(false);
    else
        hlp->setHidden(true);
}

void MainWindow::on_action_7_triggered()
{
    if(licensef->isHidden())
        licensef->setHidden(false);
    else
        licensef->setHidden(true);
}

void MainWindow::on_pushButton_2_clicked()
{
    io = false;
    if(!buttonflag)
    {
        ui->horizontalSlider->setEnabled(false);
        ui->horizontalSlider_2->setEnabled(false);
        ui->horizontalSlider_3->setEnabled(false);
        ui->horizontalSlider_5->setEnabled(false);
        ui->pushButton_2->setText(trUtf8("Стоп"));
        buttonflag = true;
        qtmr->start(10);
    }
    else
    {
        ui->horizontalSlider->setEnabled(true);
        ui->horizontalSlider_2->setEnabled(true);
        ui->horizontalSlider_3->setEnabled(true);
        ui->horizontalSlider_5->setEnabled(true);
        buttonflag = false;
        ui->pushButton_2->setText(trUtf8("Старт"));
        qtmr->stop();
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->lcdNumber->display(0);

    qtmr->stop();
    tot = 0;
    io = false;

    ui->pushButton_2->setEnabled(true);
    if(maction.ff == 3 || buttonflag == true)
        ui->pushButton_2->click();
    caction_reset();
    ui->widget->updateGL();
}


void MainWindow::on_horizontalSlider_4_valueChanged(int value)
{
    QString st;
    st = QString::number(value);
    ui->label_11->setText(st);
    maction.ktime = (double)value/100.0;
}


void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    QString st;
    double value1;
    value1 = (double)value/10.0;
    st = QString::number(value1);
    ui->label_9->setText(st);
    maction.RSph = value1;
    maction.y = maction.y0 = maction.L * sin(maction.Beta) + maction.RSph*sqrt(2.0) + (maction.RLoop - maction.RLoop * cos(maction.Beta));
    maction.z = -def_dz * maction.RLoop / def_rloop;
    maction.x = maction.x0 = sin(maction.Beta) * maction.RSph * sqrt(2.0);
    caction_reset();
    update_gframes();
    ui->widget->updateGL();
}



void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    QString st;
    double value1 = (double)value/10.0;
    st = QString::number(value1);
    ui->label_10->setText(st);
    maction.RLoop = value1;
    maction.y = maction.y0 = maction.L * sin(maction.Beta) + maction.RSph*sqrt(2.0) + (maction.RLoop - maction.RLoop * cos(maction.Beta));
    maction.z = -def_dz * maction.RLoop / def_rloop;
    maction.x = maction.x0 = sin(maction.Beta) * maction.RSph * sqrt(2.0);
    caction_reset();
    update_gframes();
    ui->widget->updateGL();
}



void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    QString st;
    st = QString::number(value);
    ui->label_8->setText(st);
    maction.L = value;
    maction.y = maction.y0 = maction.L * sin(maction.Beta) + maction.RSph*sqrt(2.0) + (maction.RLoop - maction.RLoop * cos(maction.Beta));
    maction.z = -def_dz * maction.RLoop / def_rloop;
    maction.x = maction.x0 = sin(maction.Beta) * maction.RSph * sqrt(2.0);
    caction_reset();
    update_gframes();
    ui->widget->updateGL();
}


void MainWindow::on_horizontalSlider_5_valueChanged(int value)
{
    QString st;
    st = QString::number(value);
    ui->label->setText(st);
    maction.Beta = 0.01745329251994329576923690768489 * (double)value;
    maction.y = maction.y0 = maction.L * sin(maction.Beta) + maction.RSph*sqrt(2.0) + (maction.RLoop - maction.RLoop * cos(maction.Beta));
    maction.z = -def_dz * maction.RLoop / def_rloop;
    maction.x = maction.x0 = sin(maction.Beta) * maction.RSph * sqrt(2.0);
    caction_reset();
    update_gframes();
    ui->widget->updateGL();
}

void MainWindow::mbreak()
{
    QMessageBox msgBox;
    if(maction.ff == 3)
    {
        ui->pushButton_2->setEnabled(false);
        qtmr->stop();
    }
    if((maction.ff == 1 || maction.ff == 2 || maction.ff == 4) && !io)
    {
        io = true;
        msgBox.setAttribute(Qt::WA_QuitOnClose);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setWindowTitle(trUtf8("Статус"));
        if(maction.ff == 1)
            msgBox.setText(trUtf8("Шар не смог совершить мертвую петлю\nПричина: начал катиться обратно"));
        if(maction.ff == 2)
            msgBox.setText(trUtf8("Шар не смог совершить мертвую петлю\nПричина: выпал из петли"));
        if(maction.ff == 4)
            msgBox.setText(trUtf8("Ошибка при моделировании\nПопробуйте ввести другие параметры"));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowIcon(QIcon(":/mres/ball.ico"));
        msgBox.exec();
    }
    if(maction.ff == 1 || maction.ff == 2 || maction.ff == 4)
    {
        ui->pushButton_2->click();
        ui->pushButton_3->click();
    }
}

void MainWindow::on_action_4_triggered()
{
    if(graph2->isHidden())
        graph2->setHidden(false);
    else
        graph2->setHidden(true);
}

void MainWindow::on_action_5_triggered()
{
    if(graph3->isHidden())
        graph3->setHidden(false);
    else
        graph3->setHidden(true);
}

void MainWindow::on_action_ball_animation_triggered()
{
    ball_animated = ui->action_ball_animation->isChecked();
}

//закрытие всех дочерних окон
void MainWindow::closeEvent(QCloseEvent *)
{
    delete graph1;
    delete graph2;
    delete graph3;
    delete hlp;
    delete auth;
    delete licensef;
    delete fr;
}

void MainWindow::update_gframes()
{
    if(graph1)
        graph1->update();
    if(graph2)
        graph2->update();
    if(graph3)
        graph3->update();
}

void MainWindow::disptime()
{
    QString str;
    str.setNum(tot/1000.0,10,2);
    ui->lcdNumber->display(str);
}

