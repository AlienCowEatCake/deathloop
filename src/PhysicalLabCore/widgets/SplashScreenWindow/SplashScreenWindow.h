/*
   Copyright (C) 2011-2016,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

   This file is part of the `PhysicalLabCore' library.

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

#if !defined (PHYSICALLABCORE_SPLASHSCREENWINDOW_H_INCLUDED)
#define PHYSICALLABCORE_SPLASHSCREENWINDOW_H_INCLUDED

#include <QWidget>
#include <QString>

namespace Ui
{
class SplashScreenWindow;
}

/// @brief Класс окна-заставки
class SplashScreenWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreenWindow(QWidget * parent = 0);
    ~SplashScreenWindow();

    /// @brief setPixmap - Загрузить изображение в окно
    void setPixmap(const QString & filename);
    /// @brief setTitle - Установить заголовок окна
    void setTitle(const QString & title);

protected:
    /// @brief keyPressEvent - Реакция на нажатие клавиши (закрытие)
    void keyPressEvent(QKeyEvent *);
    /// @brief mousePressEvent - Реакция на нажатие мыши (закрытие)
    void mousePressEvent(QMouseEvent *);

private:
    /// @brief moveToCenter - Перемещение окна в центр экрана
    void moveToCenter();

    Ui::SplashScreenWindow * m_ui;
};

#endif // PHYSICALLABCORE_SPLASHSCREENWINDOW_H_INCLUDED

