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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <cmath>
#include <cassert>
#include <QtGlobal>
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QPair>
#include <QLocale>
#include <QResizeEvent>
#include <QActionGroup>
#include <QMap>
#include <QStyleFactory>

#include "widgets/HtmlWindow/HtmlWindow.h"
#include "widgets/SplashScreenWindow/SplashScreenWindow.h"
#include "themes/ThemeUtils.h"
#include "utils/Workarounds.h"
#include "utils/ImageSaver.h"
#include "utils/SettingsWrapper.h"
#include "GraphWindowSpeed.h"
#include "GraphWindowAngular.h"
#include "GraphWindowHeight.h"
#include "PhysicalController.h"
#include "ModelInfo.h"

namespace {

const int helpWindowWidth     = 880;
const int helpWindowHeight    = 540;
const int authorsWindowWidth  = 670;
const int authorsWindowHeight = 400;
const int licenseWindowWidth  = 560;
const int licenseWindowHeight = 380;

const int sliderAngleDefaultPosition        = 60;   ///< угол наклона 60 градусов
const int sliderLengthDefaultPosition       = 6;    ///< длина 6 м
const int sliderLoopRadiusDefaultPosition   = 20;   ///< радиус петли 2 м
const int sliderSphereRadiusDefaultPosition = 3;    ///< радиус шара 0.3 м
const int sliderSpeedDefaultPosition        = 100;  ///< скорость эксперимента 100%
const bool actionBallAnimationDefaultState  = true; ///< анимация вращения шарика

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_physicalController(new PhysicalController(this))
    , m_imageSaver(new ImageSaver(this))
{
    m_ui->setupUi(this);
    setCentralWidget(m_ui->widget);
    setAttribute(Qt::WA_DeleteOnClose);

    QActionGroup * langActions = new QActionGroup(this);
    langActions->addAction(m_ui->actionEnglish);
    langActions->addAction(m_ui->actionRussian);
    langActions->setExclusive(true);

#if defined (Q_OS_MAC)
    // Под Mac OS X из коробки выглядит настолько страшно, что приходится немного стилизовать
    const QList<QGroupBox*> allGroupBoxes = findChildren<QGroupBox*>();
    for(QList<QGroupBox*>::ConstIterator it = allGroupBoxes.begin(); it != allGroupBoxes.end(); ++it)
        (*it)->setStyleSheet(QString::fromLatin1("QGroupBox::title { font-size: 12pt; margin-bottom: 0px; margin-left: 7px; margin-top: 2px; }"));
    const QList<QLabel*> allLabels = findChildren<QLabel*>();
    for(QList<QLabel*>::ConstIterator it = allLabels.begin(); it != allLabels.end(); ++it)
        (*it)->setStyleSheet(QString::fromLatin1("QLabel { font-size: 12pt; }"));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    const QList<QSlider*> allSliders = findChildren<QSlider*>();
    for(QList<QSlider*>::ConstIterator it = allSliders.begin(); it != allSliders.end(); ++it)
        (*it)->setStyle(QStyleFactory::create(QString::fromLatin1("Fusion")));
#endif
#endif

    m_physicalController->resetPhysicalEngine();
    m_ui->widget->setPhysicalController(m_physicalController);
    // Соединение таймера с виджетами
    m_physicalController->connectToTimer(m_ui->widget ,SLOT(updateGL()));
    m_physicalController->connectToTimer(this, SLOT(updateLcdDisplay()));
    m_physicalController->connectToTimer(this, SLOT(checkSimulationState()));
    // Индкатор времени
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    m_ui->lcdNumber->setDigitCount(6);  // Qt 4.6+
#else
    m_ui->lcdNumber->setNumDigits(6);   // Deprecated
#endif
    m_ui->lcdNumber->setDecMode();
    m_ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    // =======
    m_speedWindow.reset(new GraphWindowSpeed);
    m_speedWindow->setPhysicalController(m_physicalController);
    m_speedWindow->setHidden(true);
    // =======
    m_angularWindow.reset(new GraphWindowAngular);
    m_angularWindow->setPhysicalController(m_physicalController);
    m_angularWindow->setHidden(true);
    // =======
    m_heightWindow.reset(new GraphWindowHeight);
    m_heightWindow->setPhysicalController(m_physicalController);
    m_heightWindow->setHidden(true);
    // =======
    m_helpWindow.reset(new HtmlWindow);
    m_helpWindow->setPreferredSize(helpWindowWidth, helpWindowHeight);
    m_helpWindow->setScrollBarEnabled();
    m_helpWindow->setHidden(true);
    // =======
    m_authorsWindow = new HtmlWindow(this);
    m_authorsWindow->setHidden(true);
    m_authorsWindow->setSize(authorsWindowWidth, authorsWindowHeight);
    // =======
    m_licenseWindow = new HtmlWindow(this);
    m_licenseWindow->setHidden(true);
    m_licenseWindow->setSize(licenseWindowWidth, licenseWindowHeight);
    m_licenseWindow->setHidden(true);
    // =======
    // Заполнение параметров по умолчанию
    m_ui->horizontalSliderAngle->setValue(sliderAngleDefaultPosition);               ///< угол наклона
    m_ui->horizontalSliderLength->setValue(sliderLengthDefaultPosition);             ///< длина
    m_ui->horizontalSliderLoopRadius->setValue(sliderLoopRadiusDefaultPosition);     ///< радиус петли
    m_ui->horizontalSliderSphereRadius->setValue(sliderSphereRadiusDefaultPosition); ///< радиус шара
    m_ui->horizontalSliderSpeed->setValue(sliderSpeedDefaultPosition);               ///< скорость эксперимента
    m_ui->actionBallAnimation->setChecked(actionBallAnimationDefaultState);          ///< анимация вращения шарика
    // =======

    // О Qt
    connect(m_ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // Окно-заставка
    m_splashWindow = new SplashScreenWindow(this);

#if defined (Q_OS_MAC)
    // Диалог About, используемый в проекте, ни разу не подходит на роль того About, что есть обычно под OS X.
    // Поэтому сделаем для такого случая отдельный пункт меню, который будет выполнять роль About.
    QAction * fakeAboutAction = new QAction(this);
    fakeAboutAction->setMenuRole(QAction::AboutRole);
    connect(fakeAboutAction, SIGNAL(triggered()), m_splashWindow, SLOT(show()));
    m_ui->menuHelp->addAction(fakeAboutAction);
#endif

    // Подгрузим иконки для меню
    const bool darkBackground = ThemeUtils::MenuHasDarkTheme(m_ui->menuGraphs);
    const QString iconNameTemplate = QString::fromLatin1(":/menuicons/%2_%1.%3")
            .arg(darkBackground ? QString::fromLatin1("white") : QString::fromLatin1("black"));
#if defined (QT_SVG_LIB) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    const QString defaultExt = QString::fromLatin1("svg");
#else
    const QString defaultExt = QString::fromLatin1("png");
#endif
    const QString pixmapExt = QString::fromLatin1("png");
    m_ui->actionSpeed->setIcon(ThemeUtils::CreateScalableIcon(
        iconNameTemplate.arg(QString::fromLatin1("speed")).arg(defaultExt),
        QStringList(iconNameTemplate.arg(QString::fromLatin1("speed")).arg(pixmapExt))));
    m_ui->actionAngular->setIcon(ThemeUtils::CreateScalableIcon(
        iconNameTemplate.arg(QString::fromLatin1("angular")).arg(defaultExt),
        QStringList(iconNameTemplate.arg(QString::fromLatin1("angular")).arg(pixmapExt))));
    m_ui->actionHeight->setIcon(ThemeUtils::CreateScalableIcon(
        iconNameTemplate.arg(QString::fromLatin1("height")).arg(defaultExt),
        QStringList(iconNameTemplate.arg(QString::fromLatin1("height")).arg(pixmapExt))));
    m_ui->actionSaveScreenshot->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_SAVE_AS, darkBackground));
    m_ui->actionExit->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_EXIT, darkBackground));
    m_ui->actionAbout->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_ABOUT, darkBackground));
    m_ui->actionAuthors->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_AUTHORS, darkBackground));
    m_ui->actionLicense->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_TEXT, darkBackground));
    m_ui->actionAboutQt->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_QT, darkBackground));

    // Переводы и подгрузка ресурсов
    updateTranslations();
}

/// @brief Функция для применения локализации
void MainWindow::updateTranslations(QString language)
{
    // Отображение название языка -> соответствующая ему менюшка
    static QMap<QString, QAction *> languagesMap;
    if(languagesMap.isEmpty())
    {
        languagesMap[QString::fromLatin1("en")] = m_ui->actionEnglish;
        languagesMap[QString::fromLatin1("ru")] = m_ui->actionRussian;
    }

    // Определим системную локаль
    static QString systemLang;
    if(systemLang.isEmpty())
    {
        const QString systemLocale = QLocale::system().name().toLower();
        for(QMap<QString, QAction *>::Iterator it = languagesMap.begin(); it != languagesMap.end(); ++it)
        {
            if(systemLocale.startsWith(it.key()))
            {
                systemLang = it.key();
                break;
            }
        }
        if(systemLang.isEmpty())
            systemLang = QString::fromLatin1("en");
    }

    // Посмотрим в настройки, не сохранен ли случайно в них язык
    SettingsWrapper settings;
    if(language.isEmpty())
    {
        const QVariant rawValue = settings.value(QString::fromLatin1("Language"), systemLang);
        const QString value = rawValue.isValid() ? rawValue.toString() : systemLang;
        language = languagesMap.find(value) != languagesMap.end() ? value : systemLang;
    }
    else
    {
        settings.setValue(QString::fromLatin1("Language"), language);
    }

    // Удалим старый перевод, установим новый
    static QTranslator qtTranslator;
    static QTranslator appTranslator;
    if(!qtTranslator.isEmpty())
        qApp->removeTranslator(&qtTranslator);
    if(!appTranslator.isEmpty())
        qApp->removeTranslator(&appTranslator);
    (void)qtTranslator.load(QString::fromLatin1("qt_%1").arg(language),
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
                            QLibraryInfo::path(QLibraryInfo::TranslationsPath));
#else
                            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
#endif
    (void)appTranslator.load(QString::fromLatin1(":/translations/%1").arg(language));
    qApp->installTranslator(&qtTranslator);
    qApp->installTranslator(&appTranslator);
    m_ui->retranslateUi(this);

    // Пофиксим шрифты
    Workarounds::FontsFix(language);

    // Проставим галочку на нужном нам языке и снимем с остальных
    languagesMap[language]->setChecked(true);

    // У кнопки старт/пауза текст зависит от состояния
    switch(m_physicalController->currentState())
    {
    case PhysicalController::SimulationNotRunning:
        m_ui->pushButtonStart->setText(tr("Start"));
        break;
    case PhysicalController::SimulationRunning:
        m_ui->pushButtonStart->setText(tr("Pause"));
        break;
    case PhysicalController::SimulationPaused:
        m_ui->pushButtonStart->setText(tr("Resume"));
        break;
    }

    // Перегрузим ресурсы в окнах
    setWindowTitle(tr("Loop the Loop"));
    m_helpWindow->setTitle(tr("About"));
    m_helpWindow->loadHtml(QString::fromLatin1(":/html/help_%1.html").arg(language));
    m_authorsWindow->setTitle(tr("Credits"));
    m_authorsWindow->loadHtml(QString::fromLatin1(":/html/author_%1.html").arg(language));
    m_licenseWindow->setTitle(tr("License"));
    m_licenseWindow->loadHtml(QString::fromLatin1(":/html/license_%1.html").arg(language));
#if defined (QT_SVG_LIB) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    m_splashWindow->setSVG(QString::fromLatin1(":/splash/splash_%1.svg").arg(language));
#else
    m_splashWindow->setPixmap(QString::fromLatin1(":/splash/splash_%1.png").arg(language));
#endif
    m_splashWindow->setTitle(tr("Loop the Loop"));

    // Также следует пересчитать геометрию виждетов
    QApplication::postEvent(this, new QResizeEvent(size(), size()));

    // Зададим дефолтное имя файла сохранялке скриншотов
    m_imageSaver->setDefaultFilePath(tr("Screenshot.png"));
}

MainWindow::~MainWindow()
{
    qApp->quit();
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

    QMessageBox messageBox(QMessageBox::Information, tr("Status"), QString(), QMessageBox::Ok, this);
    switch(status)
    {
    case Action::StateGoBack:
        messageBox.setText(tr("The ball could not pass a loop"));
        messageBox.setInformativeText(tr("Reason: The ball rolled back"));
        break;
    case Action::StateFallDown:
        messageBox.setText(tr("The ball could not pass a loop"));
        messageBox.setInformativeText(tr("Reason: The ball dropped out of the loop"));
        break;
    case Action::StateError:
        messageBox.setText(tr("Error while performing the simulation"));
        messageBox.setInformativeText(tr("Please try a different combination of initial conditions"));
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
        m_ui->pushButtonStart->setText(tr("Pause"));
        m_physicalController->startSimulation();
        break;
    case PhysicalController::SimulationPaused:
        m_ui->pushButtonStart->setText(tr("Pause"));
        m_physicalController->resumeSimulation();
        break;
    case PhysicalController::SimulationRunning:
        m_ui->pushButtonStart->setText(tr("Resume"));
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
    const double value1 = value / 10.0;
    m_ui->labelLoopRadius->setText(QString::number(value1));
    m_physicalController->action().set_LoopR(value1);
    m_physicalController->resetPhysicalEngine();
    updateGraphWindows();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка радиуса шарика
void MainWindow::on_horizontalSliderSphereRadius_valueChanged(int value)
{
    const double value1 = value / 10.0;
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

/// @brief Слот на запрос сохранения скриншота из меню
void MainWindow::on_actionSaveScreenshot_triggered()
{
    const QImage screenshot = m_ui->widget->grabFrameBuffer();
    m_imageSaver->save(screenshot);
}

/// @brief Слот на запрос выхода из меню
void MainWindow::on_actionExit_triggered()
{
    this->close();
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
    updateTranslations(QString::fromLatin1("en"));
}

/// @brief Слот на включение русского языка из меню
void MainWindow::on_actionRussian_triggered()
{
    updateTranslations(QString::fromLatin1("ru"));
}

