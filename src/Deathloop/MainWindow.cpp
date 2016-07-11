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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#if defined (Q_OS_WIN) && defined (USE_WIN98_WORKAROUNDS)
#include <windows.h>
#endif

#include <cmath>
#include <cassert>
#if defined (HAVE_QT5)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QMessageBox>
#include <QTranslator>
#include <QPair>
#include <QMenu>
#include <QSettings>
#include <QLocale>
#include <QResizeEvent>
#include <QObject>
#include <QFont>
#include <QFontDatabase>

#include "widgets/HtmlWindow/HtmlWindow.h"
#include "widgets/SplashScreenWindow/SplashScreenWindow.h"
#include "GraphWindowSpeed.h"
#include "GraphWindowAngular.h"
#include "GraphWindowHeight.h"
#include "PhysicalController.h"
#include "ModelInfo.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_physicalController(new PhysicalController(this))
{
    m_ui->setupUi(this);
    setCentralWidget(m_ui->widget);
    m_physicalController->resetPhysicalEngine();
    m_ui->widget->setPhysicalController(m_physicalController);
    // Соединение таймера с виджетами
    m_physicalController->connectToTimer(m_ui->widget ,SLOT(updateGL()));
    m_physicalController->connectToTimer(this, SLOT(updateLcdDisplay()));
    m_physicalController->connectToTimer(this, SLOT(checkSimulationState()));
    // Индкатор времени
    m_ui->lcdNumber->setDecMode();
    m_ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    // =======
    m_speedWindow = new GraphWindowSpeed;
    m_speedWindow->setPhysicalController(m_physicalController);
    m_speedWindow->setHidden(true);
    // =======
    m_angularWindow = new GraphWindowAngular;
    m_angularWindow->setPhysicalController(m_physicalController);
    m_angularWindow->setHidden(true);
    // =======
    m_heightWindow = new GraphWindowHeight;
    m_heightWindow->setPhysicalController(m_physicalController);
    m_heightWindow->setHidden(true);
    // =======
    m_helpWindow = new HtmlWindow;
    m_helpWindow->setSize(880, 540);
    m_helpWindow->setScrollBarEnabled();
    m_helpWindow->setHidden(true);
    // =======
    m_authorsWindow = new HtmlWindow;
    m_authorsWindow->setHidden(true);
    m_authorsWindow->setSize(670, 400);
    // =======
    m_licenseWindow = new HtmlWindow;
    m_licenseWindow->setHidden(true);
    m_licenseWindow->setSize(560, 380);
    m_licenseWindow->setHidden(true);
    // =======
    // Заполнение параметров по умолчанию
    // угол наклона 60 градусов
    m_ui->horizontalSliderAngle->setValue(60);
    // длина 6 м
    m_ui->horizontalSliderLength->setValue(6);
    // радиус петли 2 м
    m_ui->horizontalSliderLoopRadius->setValue(20);
    // радиус шара 0.3 м
    m_ui->horizontalSliderSphereRadius->setValue(3);
    // скорость эксперимента 100%
    m_ui->horizontalSliderSpeed->setValue(100);
    // Анимация вращения шарика
    m_ui->actionBallAnimation->setChecked(true);
    // =======

    // О Qt
    connect(m_ui->actionAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));

    // Соединяем графики друг с другом, чтобы они могли сообщать об изменении настроек
    connect(m_speedWindow, SIGNAL(settingsChanged()), m_angularWindow, SLOT(onSettingsChanged()));
    connect(m_speedWindow, SIGNAL(settingsChanged()), m_heightWindow, SLOT(onSettingsChanged()));
    connect(m_angularWindow, SIGNAL(settingsChanged()), m_speedWindow, SLOT(onSettingsChanged()));
    connect(m_angularWindow, SIGNAL(settingsChanged()), m_heightWindow, SLOT(onSettingsChanged()));
    connect(m_heightWindow, SIGNAL(settingsChanged()), m_speedWindow, SLOT(onSettingsChanged()));
    connect(m_heightWindow, SIGNAL(settingsChanged()), m_angularWindow, SLOT(onSettingsChanged()));

    // Окно-заставка
    m_splashWindow = new SplashScreenWindow;

    // Переводы и подгрузка ресурсов
    updateTranslations();
}

/// @brief Исправить отображение локализованных шрифтов под Windows
void MainWindow::fontsFix(const QString & language)
{
#if defined (Q_OS_WIN)

    // Отображение название языка -> соответствующая ему WritingSystem
    static QList<QPair<QString, QFontDatabase::WritingSystem> > writingSystemMap =
            QList<QPair<QString, QFontDatabase::WritingSystem> >()
            << qMakePair(QString("en"), QFontDatabase::Latin)
            << qMakePair(QString("ru"), QFontDatabase::Cyrillic);

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
    fallbackFont.setFamily("Tahoma");

    // Проверим, умеет ли стандартный шрифт писать на новом языке
    static QFontDatabase fontDatabase;
    if(!fontDatabase.families(currentWritingSystem).contains(defaultFont.family(), Qt::CaseInsensitive))
        qApp->setFont(fallbackFont);   // Если не умеет - заменим на Tahoma
    else
        qApp->setFont(defaultFont);    // Если умеет, то вернем его обратно

#if defined (USE_WIN98_WORKAROUNDS)
    // Для Win98 форсированно заменяем шрифты на Tahoma для всех не-английских локалей
    static DWORD dwVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));
    if(dwVersion <= 4)
    {
        if(language != "en")
            qApp->setFont(fallbackFont);
        else
            qApp->setFont(defaultFont);
    }
#endif

#else

    Q_UNUSED(language);

#endif
}

/// @brief Функция для применения локализации
/// @todo Это не особо рабочая заготовка, надо переписать
void MainWindow::updateTranslations(QString language)
{
    // Отображение название языка -> соответствующая ему менюшка
    static QList<QPair<QString, QAction *> > languagesMap = QList<QPair<QString, QAction *> >()
            << qMakePair(QString("en"), m_ui->actionEnglish)
            << qMakePair(QString("ru"), m_ui->actionRussian);

    // Определим системную локаль
    static QString systemLang;
    if(systemLang.isEmpty())
    {
        QString systemLocale = QLocale::system().name().toLower();
        for(QList<QPair<QString, QAction *> >::Iterator it = languagesMap.begin(); it != languagesMap.end(); ++it)
        {
            if(systemLocale.startsWith(it->first))
            {
                systemLang = it->first;
                break;
            }
        }
        if(systemLang.isEmpty())
            systemLang = "en";
    }

    // Посмотрим в настройки, не сохранен ли случайно в них язык
    QSettings settings;
    if(language.isEmpty())
        language = settings.value("Language", systemLang).toString();
    else
        settings.setValue("Language", language);

    // Удалим старый перевод, установим новый
    static QTranslator translator;
    if(!translator.isEmpty())
        qApp->removeTranslator(&translator);
    translator.load(QString(":/translations/%1").arg(language));
    qApp->installTranslator(&translator);
    m_ui->retranslateUi(this);

    // Пофиксим шрифты
    fontsFix(language);

    // Пробежим по меню и проставим галочку на нужном нам языке и снимем с остальных
    for(QList<QPair<QString, QAction *> >::Iterator it = languagesMap.begin(); it != languagesMap.end(); ++it)
        it->second->setChecked(it->first == language);

    // У кнопки старт/пауза текст зависит от состояния
    if(m_physicalController->currentState() == PhysicalController::SimulationRunning)
        m_ui->pushButtonStart->setText(tr("Stop"));
    else
        m_ui->pushButtonStart->setText(tr("Start"));

    // Перегрузим ресурсы в окнах
    setWindowTitle(trUtf8("Мертвая петля"));
    m_speedWindow->setLabels("Скорость шара", "t, c", "v, м/с");
    m_angularWindow->setLabels("Угловая скорость шара", "t, c", "w, рад/с");
    m_heightWindow->setLabels("Изменение высоты (y)", "t, c", "y, м");
    m_helpWindow->setTitle("О программе");
    m_helpWindow->loadHtml(":/html/help_ru.html");
    m_authorsWindow->setTitle("Авторы");
    m_authorsWindow->loadHtml(":/html/author_ru.html");
    m_licenseWindow->setTitle("Лицензия");
    m_licenseWindow->loadHtml(":/html/license_ru.html");
    m_splashWindow->setPixmap(":/mres/splash.png");
    m_splashWindow->setTitle(trUtf8("Мертвая петля"));

    // Также следует пересчитать геометрию виждетов
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

/// @brief Обработчик события закрытия окна
void MainWindow::closeEvent(QCloseEvent *)
{
    delete m_speedWindow;
    delete m_angularWindow;
    delete m_heightWindow;
    delete m_helpWindow;
    delete m_authorsWindow;
    delete m_licenseWindow;
    delete m_splashWindow;
}

/// @brief Слот на обновление времени на дисплее
void MainWindow::updateLcdDisplay()
{
    QString str;
    str.setNum(m_physicalController->currentTime() / 1000.0, 10, 2);
    m_ui->lcdNumber->display(str);
}

/// @brief Слот на проверку текущего состояния системы
void MainWindow::checkSimulationState()
{
    Action::BallState status = m_physicalController->action().get_State();
    if(status == Action::StateNormal)
        return;
    m_physicalController->pauseSimulation();
    m_ui->pushButtonStart->setEnabled(false);
    if(status == Action::StateFinished)
        return;

    QMessageBox messageBox(QMessageBox::Information, trUtf8("Статус"), QString(), QMessageBox::Ok, this);
    switch(status)
    {
    case Action::StateGoBack:
        messageBox.setText(trUtf8("Шар не смог совершить мертвую петлю"));
        messageBox.setInformativeText(trUtf8("Причина: начал катиться обратно"));
        break;
    case Action::StateFallDown:
        messageBox.setText(trUtf8("Шар не смог совершить мертвую петлю"));
        messageBox.setInformativeText(trUtf8("Причина: выпал из петли"));
        break;
    case Action::StateError:
        messageBox.setText(trUtf8("Ошибка при моделировании"));
        messageBox.setInformativeText(trUtf8("Попробуйте ввести другие параметры"));
        break;
    default:
        assert(false);
        break;
    }
    messageBox.exec();
}

/// @brief Слот на нажатие кнопки старт/пауза
void MainWindow::on_pushButtonStart_clicked()
{
    switch(m_physicalController->currentState())
    {
    case PhysicalController::SimulationNotRunning:
        m_ui->horizontalSliderLength->setEnabled(false);
        m_ui->horizontalSliderLoopRadius->setEnabled(false);
        m_ui->horizontalSliderSphereRadius->setEnabled(false);
        m_ui->horizontalSliderAngle->setEnabled(false);
        m_ui->pushButtonStart->setText(tr("Stop"));
        m_physicalController->startSimulation();
        break;
    case PhysicalController::SimulationPaused:
        m_ui->pushButtonStart->setText(tr("Stop"));
        m_physicalController->resumeSimulation();
        break;
    case PhysicalController::SimulationRunning:
        m_ui->pushButtonStart->setText(tr("Start"));
        m_physicalController->pauseSimulation();
    }
}

/// @brief Слот на нажатие кнопки стоп
void MainWindow::on_pushButtonStop_clicked()
{
    switch(m_physicalController->currentState())
    {
    case PhysicalController::SimulationRunning:
    case PhysicalController::SimulationPaused:
        m_physicalController->stopSimulation();
        m_ui->horizontalSliderLength->setEnabled(true);
        m_ui->horizontalSliderLoopRadius->setEnabled(true);
        m_ui->horizontalSliderSphereRadius->setEnabled(true);
        m_ui->horizontalSliderAngle->setEnabled(true);
        m_ui->pushButtonStart->setText(tr("Start"));
        m_ui->pushButtonStart->setEnabled(true);
        m_ui->lcdNumber->display(0);
        m_ui->widget->updateGL();
        break;
    case PhysicalController::SimulationNotRunning:
        break;
    }
}

/// @brief Функция, вызывающая обновление всех графиков
void MainWindow::updateGraphWindows()
{
    m_speedWindow->update();
    m_angularWindow->update();
    m_heightWindow->update();
}

/// @brief Слот на включение/отключение анимации вращения шарика из меню
void MainWindow::on_actionBallAnimation_triggered()
{
    m_ui->widget->setBallAnimated(m_ui->actionBallAnimation->isChecked());
}

/// @brief Слот на открытие графика скорости из меню
void MainWindow::on_actionSpeed_triggered()
{
    m_speedWindow->setHidden(!m_speedWindow->isHidden());
}

/// @brief Слот на открытие графика угловой скорости из меню
void MainWindow::on_actionAngular_triggered()
{
    m_angularWindow->setHidden(!m_angularWindow->isHidden());
}

/// @brief Слот на открытие графика высоты из меню
void MainWindow::on_actionHeight_triggered()
{
    m_heightWindow->setHidden(!m_heightWindow->isHidden());
}

/// @brief Слот на изменение ползунка угла наклона первого сегмента
void MainWindow::on_horizontalSliderAngle_valueChanged(int value)
{
    m_ui->labelAngle->setText(QString::number(value));
    m_physicalController->action().set_BetaAngle(0.01745329251994329576923690768489 * value);
    m_physicalController->resetPhysicalEngine();
    updateGraphWindows();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка длины первого сегмента
void MainWindow::on_horizontalSliderLength_valueChanged(int value)
{
    m_ui->labelLength->setText(QString::number(value));
    m_physicalController->action().set_FirstLength(value);
    m_physicalController->resetPhysicalEngine();
    updateGraphWindows();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка радиуса петли
void MainWindow::on_horizontalSliderLoopRadius_valueChanged(int value)
{
    double value1 = value / 10.0;
    m_ui->labelLoopRadius->setText(QString::number(value1));
    m_physicalController->action().set_LoopR(value1);
    m_physicalController->resetPhysicalEngine();
    updateGraphWindows();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка радиуса шарика
void MainWindow::on_horizontalSliderSphereRadius_valueChanged(int value)
{
    double value1 = value / 10.0;
    m_ui->labelSphereRadius->setText(QString::number(value1));
    m_physicalController->action().set_SphereR(value1);
    m_physicalController->resetPhysicalEngine();
    updateGraphWindows();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка скорости эксперимента
void MainWindow::on_horizontalSliderSpeed_valueChanged(int value)
{
    m_ui->labelSpeed->setText(QString::number(value));
    m_physicalController->setSimulationSpeed(value / 100.0);
    m_ui->widget->updateGL();
}

/// @brief Слот на открытие окна "О программе" из меню
void MainWindow::on_actionAbout_triggered()
{
    m_helpWindow->setHidden(!m_helpWindow->isHidden());
}

/// @brief Слот на открытие окна "Разработчики" из меню
void MainWindow::on_actionAuthors_triggered()
{
    m_authorsWindow->setHidden(!m_authorsWindow->isHidden());
}

/// @brief Слот на открытие окна "Лицензия" из меню
void MainWindow::on_actionLicense_triggered()
{
    m_licenseWindow->setHidden(!m_licenseWindow->isHidden());
}

/// @brief Слот на включение английского языка из меню
void MainWindow::on_actionEnglish_triggered()
{
    updateTranslations("en");
}

/// @brief Слот на включение русского языка из меню
void MainWindow::on_actionRussian_triggered()
{
    updateTranslations("ru");
}

