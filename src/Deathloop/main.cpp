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

#if !defined (USE_SWRAST)
#include <QGLFormat>
#endif
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
#if !defined (USE_SWRAST)
    QGLFormat fmt;
    fmt.setSampleBuffers(true);
    fmt.setSamples(16); // 2, 4, 8, 16
    QGLFormat::setDefaultFormat(fmt);
#endif

    QApplication app(argc, argv);
    app.setOrganizationDomain("fami.codefreak.ru");
    app.setOrganizationName("fami_net_team");
    app.setApplicationName("deathloop");
    app.setApplicationVersion("1.0");
#if !defined (Q_OS_MAC)
    app.setWindowIcon(QIcon(":/mres/ball.ico"));
#endif
#if defined (USE_FORCE_GL)
    app.setAttribute(Qt::AA_UseDesktopOpenGL);
#endif
    MainWindow w;  // главное окно
    w.show();
    return app.exec();
}
