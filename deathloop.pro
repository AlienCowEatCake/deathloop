#-------------------------------------------------
#
# Project created by QtCreator 2011-10-29T19:43:53
#
# Authors: Alexandrov Mikhail, Zhigalov Peter, Kurochkin Andrey
#-------------------------------------------------

# TODO: Модели кривоваты

# ==============================================================================

DEFINES += VERSION_NUMBER=\\\"v0.37\\\"

TEMPLATE = app
TARGET = deathloop

INCLUDEPATH += src

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += HAVE_QT5
    contains(QT_CONFIG, dynamicgl) {
        win32-g++* {
            QMAKE_LIBS += -lopengl32
        } else {
            QMAKE_LIBS += opengl32.lib
        }
        DEFINES += USE_FORCE_GL
    } else {
        contains(QT_CONFIG, opengles.) | contains(QT_CONFIG, angle) {
            error("This program requires Qt to be configured with -opengl desktop (recommended) or -opengl dynamic")
        }
    }
}

win32:lessThan(QT_VERSION, 4.5.0) {
    win32-g++*|win32-msvc|win32-msvc.net|win32-msvc200* {
        DEFINES += HAVE_OLD_QT
    }
}

HEADERS += src/cscene3d.h \
           src/mainwindow.h \
           src/main.h \
           src/cmodel.h \
           src/caction.h \
           src/frame.h \
           src/help.h \
           src/author.h \
           src/cscene2dn.h \
           src/gframe_hight.h \
           src/gframe_speed.h \
           src/gframe_angular.h \
           src/license.h

SOURCES += src/main.cpp \
           src/cscene3d.cpp \
           src/mainwindow.cpp \
           src/cmodel.cpp \
           src/cmilkshapemodel.cpp \
           src/caction.cpp \
           src/frame.cpp \
           src/help.cpp \
           src/author.cpp \
           src/cscene2dn.cpp \
           src/gframe_hight.cpp \
           src/gframe_speed.cpp \
           src/gframe_angular.cpp \
           src/license.cpp

FORMS +=   src/frame.ui \
           src/gframe_hight.ui \
           src/gframe_speed.ui \
           src/gframe_angular.ui \
           src/mainwindow.ui

win32 {
  FORMS += src/win32/author.ui \
           src/win32/help.ui \
           src/win32/license.ui
  RC_FILE += src/resources/icon.rc
}

unix {
  FORMS += src/linux/author.ui \
           src/linux/help.ui \
           src/linux/license.ui
}

RESOURCES += src/resources/mres.qrc \
             src/resources/help/help.qrc \
             src/resources/models/models.qrc \
             src/resources/menuicons/menuicons.qrc

# === Сборочные директории =====================================================

DESTDIR = .
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

# === Опции компиляции =========================================================

QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9
CONFIG += warn_on

*g++*|*clang* {
    QMAKE_CXXFLAGS_WARN_ON *= -Wextra
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -O3
    QMAKE_CXXFLAGS_RELEASE *= -ffast-math
    QMAKE_CXXFLAGS_RELEASE *= -fno-math-errno
}

*msvc* {
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -Ox
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += _USE_MATH_DEFINES
}

