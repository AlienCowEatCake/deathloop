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
#include <QtPlugin>
#include <QIcon>
#include "utils/Workarounds.h"
#include "MainWindow.h"

#if defined (USE_STATIC_QJPEG)
Q_IMPORT_PLUGIN(qjpeg)
#endif
#if defined (USE_STATIC_QTIFF)
Q_IMPORT_PLUGIN(qtiff)
#endif
#if defined (USE_STATIC_QICO)
Q_IMPORT_PLUGIN(qico)
#endif

int main(int argc, char** argv)
{
    Workarounds::HighDPIFix();
#if !defined (USE_SWRAST)
    QGLFormat fmt;
    fmt.setSampleBuffers(true);
    QGLFormat::setDefaultFormat(fmt);
#endif

    QApplication app(argc, argv);
    app.setOrganizationDomain(QString::fromLatin1("fami.codefreak.ru"));
    app.setOrganizationName(QString::fromLatin1("fami_net_team"));
    app.setApplicationName(QString::fromLatin1("deathloop"));
    app.setApplicationVersion(QString::fromLatin1("1.0"));
#if !defined (Q_OS_MAC)
    app.setWindowIcon(QIcon(QString::fromLatin1(":/icon/ball.ico")));
#endif
#if defined (USE_FORCE_GL)
    app.setAttribute(Qt::AA_UseDesktopOpenGL);
#endif
    // Костыль по мотивам QTBUG-30790, QTBUG-28872, QTBUG-28872
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    MainWindow * w = new MainWindow; // главное окно
    w->show();
    return app.exec();
}

