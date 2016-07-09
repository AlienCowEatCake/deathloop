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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

    /// @brief Обработчик события закрытия окна
    void closeEvent(QCloseEvent *);

private slots:

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

    void checkSimulationState();

    void on_action_4_triggered();

    void on_action_5_triggered();

    void on_action_7_triggered();

    void on_action_ball_animation_triggered();

    /// @brief Слот на обновление времени на дисплее
    void updateLcdDisplay();

    /// @brief Слот на включение английского языка из меню
    void on_actionEnglish_triggered();
    /// @brief Слот на включение русского языка из меню
    void on_actionRussian_triggered();

private:

    /// @brief Исправить отображение локализованных шрифтов под Windows
    void fontsFix(const QString & language);

    /// @brief Функция для применения локализации
    /// @todo Это не особо рабочая заготовка, надо переписать
    void updateTranslations(QString language = QString());

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
};

#endif // MAINWINDOW_H_INCLUDED

