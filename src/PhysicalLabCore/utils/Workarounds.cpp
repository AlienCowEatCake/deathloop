/*
   Copyright (C) 2011-2016,
        Andrei V. Kurochkin     <kurochkin.andrei.v@yandex.ru>
        Mikhail E. Aleksandrov  <alexandroff.m@gmail.com>
        Peter S. Zhigalov       <peter.zhigalov@gmail.com>

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

#include "Workarounds.h"
#include <cstdlib>
#include <QtGlobal>
#include <QSysInfo>
#include <QApplication>
#include <QList>
#include <QPair>
#include <QFont>
#include <QFontDatabase>
#include <QStringList>

namespace Workarounds {

/// @brief Исправить отображение локализованных шрифтов под Windows
/// @param[in] language - язык, для которого будет проводиться исправление
/// @note Функцию следует вызывать при смене языка
void FontsFix(const QString & language)
{
#if defined (Q_OS_WIN)

    // Отображение название языка -> соответствующая ему WritingSystem
    static QList<QPair<QString, QFontDatabase::WritingSystem> > writingSystemMap =
            QList<QPair<QString, QFontDatabase::WritingSystem> >()
            << qMakePair(QString::fromLatin1("en"), QFontDatabase::Latin)
            << qMakePair(QString::fromLatin1("ru"), QFontDatabase::Cyrillic);

    // Найдем WritingSystem для текущего языка
    QFontDatabase::WritingSystem currentWritingSystem = QFontDatabase::Any;
    for(QList<QPair<QString, QFontDatabase::WritingSystem> >::Iterator it = writingSystemMap.begin(); it != writingSystemMap.end(); ++it)
    {
        if(it->first == language)
        {
            currentWritingSystem = it->second;
            break;
        }
    }

    // Шрифт стандартный, по умолчанию
    static QFont defaultFont = qApp->font();
    // Шрифт Tahoma, если стандартный не поддерживает выбранный язык
    QFont fallbackFont = defaultFont;
    fallbackFont.setFamily(QString::fromLatin1("Tahoma"));

    // Проверим, умеет ли стандартный шрифт писать на новом языке
    static QFontDatabase fontDatabase;
    if(!fontDatabase.families(currentWritingSystem).contains(defaultFont.family(), Qt::CaseInsensitive))
        qApp->setFont(fallbackFont);   // Если не умеет - заменим на Tahoma
    else
        qApp->setFont(defaultFont);    // Если умеет, то вернем его обратно

    // Для Win98 форсированно заменяем шрифты на Tahoma для всех не-английских локалей
    if(QSysInfo::windowsVersion() <= QSysInfo::WV_Me)
    {
        if(language != QString::fromLatin1("en"))
            qApp->setFont(fallbackFont);
        else
            qApp->setFont(defaultFont);
    }

#else

    Q_UNUSED(language);

#endif
}

/// @brief Автоматически увеличить масштаб отображения для высоких DPI
/// @note Функцию следует вызывать перед созданием QApplication
/// @attention Актуально только для Qt 5.4+
void HighDPIFix()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    static char newEnv [] = "QT_AUTO_SCREEN_SCALE_FACTOR=1";
    if(!getenv("QT_AUTO_SCREEN_SCALE_FACTOR") && !getenv("QT_DEVICE_PIXEL_RATIO"))
        putenv(newEnv);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    static char newEnv [] = "QT_DEVICE_PIXEL_RATIO=auto";
    if(!getenv("QT_DEVICE_PIXEL_RATIO"))
        putenv(newEnv);
#endif

    // Qt::AA_UseHighDpiPixmaps доступен и в более ранних версиях,
    // однако без поддержки HighDPI его применение нецелесообразно
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
}

} // Workarounds

