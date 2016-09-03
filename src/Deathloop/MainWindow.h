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

#if !defined (MAINWINDOW_H_INCLUDED)
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class HtmlWindow;
class SplashScreenWindow;
class GraphWindowHeight;
class GraphWindowSpeed;
class GraphWindowAngular;
class PhysicalController;
class ImageSaver;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    /// @brief Слот на обновление времени на дисплее
    void updateLcdDisplay();
    /// @brief Слот на проверку текущего состояния системы
    void checkSimulationState();
    /// @brief Слот на нажатие кнопки старт/пауза
    void on_pushButtonStart_clicked();
    /// @brief Слот на нажатие кнопки стоп
    void on_pushButtonStop_clicked();
    /// @brief Слот на включение/отключение анимации вращения шарика из меню
    void on_actionBallAnimation_triggered();

    /// @brief Слот на открытие графика скорости из меню
    void on_actionSpeed_triggered();
    /// @brief Слот на открытие графика угловой скорости из меню
    void on_actionAngular_triggered();
    /// @brief Слот на открытие графика высоты из меню
    void on_actionHeight_triggered();

    /// @brief Слот на изменение ползунка угла наклона первого сегмента
    void on_horizontalSliderAngle_valueChanged(int value);
    /// @brief Слот на изменение ползунка длины первого сегмента
    void on_horizontalSliderLength_valueChanged(int value);
    /// @brief Слот на изменение ползунка радиуса петли
    void on_horizontalSliderLoopRadius_valueChanged(int value);
    /// @brief Слот на изменение ползунка радиуса шарика
    void on_horizontalSliderSphereRadius_valueChanged(int value);
    /// @brief Слот на изменение ползунка скорости эксперимента
    void on_horizontalSliderSpeed_valueChanged(int value);

    /// @brief Слот на запрос сохранения скриншота из меню
    void on_actionSaveScreenshot_triggered();
    /// @brief Слот на запрос выхода из меню
    void on_actionExit_triggered();
    /// @brief Слот на открытие окна "О программе" из меню
    void on_actionAbout_triggered();
    /// @brief Слот на открытие окна "Разработчики" из меню
    void on_actionAuthors_triggered();
    /// @brief Слот на открытие окна "Лицензия" из меню
    void on_actionLicense_triggered();

    /// @brief Слот на включение английского языка из меню
    void on_actionEnglish_triggered();
    /// @brief Слот на включение русского языка из меню
    void on_actionRussian_triggered();

private:

    /// @brief Функция для применения локализации
    void updateTranslations(QString language = QString());

    /// @brief Функция, вызывающая обновление всех графиков
    void updateGraphWindows();

    Ui::MainWindow *m_ui;

    PhysicalController *m_physicalController;

    SplashScreenWindow *m_splashWindow;     ///< окно-заставка
    GraphWindowSpeed *m_speedWindow;        ///< график скорости
    GraphWindowAngular *m_angularWindow;    ///< график угловой скорости
    GraphWindowHeight *m_heightWindow;      ///< график высоты
    HtmlWindow *m_helpWindow;               ///< справка
    HtmlWindow *m_authorsWindow;            ///< об авторах
    HtmlWindow *m_licenseWindow;            ///< лицензия

    ImageSaver *m_imageSaver;
};

#endif // MAINWINDOW_H_INCLUDED

