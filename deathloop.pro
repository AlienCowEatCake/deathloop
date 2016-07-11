#-------------------------------------------------
#
# Project created by QtCreator 2011-10-29T19:43:53
#
#-------------------------------------------------

# === Настраиваемые опции ======================================================

# Использовать софтверный растеризатор
#CONFIG += use_swrast

# ==============================================================================

# TODO: Модели кривоваты

TEMPLATE = app
TARGET = deathloop

INCLUDEPATH += src/Deathloop src/Deathloop-old

#DEFINES += QT_NO_CAST_FROM_ASCII

include(src/PhysicalLabCore/PhysicalLabCore.pri)

HEADERS += \
    src/Deathloop/Action.h \
    src/Deathloop/PhysicalController.h \
    src/Deathloop/ModelInfo.h \
    src/Deathloop/ModelSphere.h \
    src/Deathloop/ModelTrackS1.h \
    src/Deathloop/ModelTrackS2.h \
    src/Deathloop/ModelTrackS3.h \
    src/Deathloop/ModelTrackS4.h \
    src/Deathloop/Scene3D.h \
    src/Deathloop/GraphWindowAngular.h \
    src/Deathloop/GraphWindowHeight.h \
    src/Deathloop/GraphWindowSpeed.h \
    src/Deathloop/MainWindow.h

SOURCES += \
    src/Deathloop/Action.cpp \
    src/Deathloop/PhysicalController.cpp \
    src/Deathloop/ModelInfo.cpp \
    src/Deathloop/ModelSphere.cpp \
    src/Deathloop/ModelTrackS1.cpp \
    src/Deathloop/ModelTrackS2.cpp \
    src/Deathloop/ModelTrackS3.cpp \
    src/Deathloop/ModelTrackS4.cpp \
    src/Deathloop/Scene3D.cpp \
    src/Deathloop/GraphWindowAngular.cpp \
    src/Deathloop/GraphWindowHeight.cpp \
    src/Deathloop/GraphWindowSpeed.cpp \
    src/Deathloop/MainWindow.cpp \
    src/Deathloop/main.cpp

FORMS += src/Deathloop/MainWindow.ui

TRANSLATIONS += \
    src/Deathloop/resources/translations/en.ts \
    src/Deathloop/resources/translations/ru.ts

win32 {
    RESOURCES += src/Deathloop/resources/html/stylesheet/stylesheet-windows.qrc
    RC_FILE += src/Deathloop/resources-old/icon.rc
}

unix:!macx {
    RESOURCES += src/Deathloop/resources/html/stylesheet/stylesheet-linux.qrc
}

macx {
    lessThan(QT_MAJOR_VERSION, 5) {
        RESOURCES += src/Deathloop/resources/html/stylesheet/stylesheet-macosx-qt4.qrc
    } else {
        RESOURCES += src/Deathloop/resources/html/stylesheet/stylesheet-macosx.qrc
    }
}

RESOURCES += \
    src/Deathloop/resources-old/mres.qrc \
    src/Deathloop/resources/html/html.qrc \
    src/Deathloop/resources/help/help.qrc \
    src/Deathloop/resources/models/models.qrc \
    src/Deathloop/resources/menuicons/menuicons.qrc \
    src/Deathloop/resources/translations/translations.qrc

# === Сборочные директории =====================================================

DESTDIR = .
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

# === Опции компиляции =========================================================

QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9

*g++*|*clang* {
    QMAKE_CXXFLAGS_RELEASE *= -ffast-math
    QMAKE_CXXFLAGS_RELEASE *= -fno-math-errno
}

