# Использовать софтверный растеризатор
#CONFIG += use_swrast

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
CONFIG += object_with_source object_parallel_to_source no_batch warn_on

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += HAVE_QT5
    contains(QT_CONFIG, opengles.) | contains(QT_CONFIG, angle) {
        !contains(QT_CONFIG, dynamicgl) : CONFIG += use_swrast
    }
    !use_swrast {
        contains(QT_CONFIG, dynamicgl) {
            win32-g++*|win32-clang* {
                QMAKE_LIBS += -lopengl32
            } else {
                QMAKE_LIBS += opengl32.lib
            }
            DEFINES += USE_FORCE_GL
        }
    }
}

lessThan(QT_VERSION, 4.5.0) {
    DEFINES += HAVE_LESS_THAN_QT45
    win32-g++*|win32-msvc|win32-msvc.net|win32-msvc200* {
        DEFINES += USE_WIN98_WORKAROUNDS
    }
}

*g++*|*clang* {
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -O3
    QMAKE_CXXFLAGS_RELEASE *= -DNDEBUG
}

*msvc* {
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -Ox
    QMAKE_CXXFLAGS_RELEASE -= -GS
    QMAKE_CXXFLAGS_RELEASE *= -GS-
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += _CRT_SECURE_NO_DEPRECATE
    DEFINES += _USE_MATH_DEFINES
}

HEADERS += \
    $$files($$PWD/models/*.h) \
    $$files($$PWD/utils/*.h) \
    $$files($$PWD/widgets/GLWidgetImpl/*.h) \
    $$files($$PWD/widgets/GLWidgetImpl/native/*.h) \
    $$files($$PWD/widgets/GraphWindowAbstract/*.h) \
    $$files($$PWD/widgets/HtmlWindow/*.h) \
    $$files($$PWD/widgets/Scene2D/*.h) \
    $$files($$PWD/widgets/Scene3DAbstract/*.h) \
    $$files($$PWD/widgets/SplashScreenWindow/*.h)

SOURCES += \
    $$files($$PWD/models/*.cpp) \
    $$files($$PWD/utils/*.cpp) \
    $$files($$PWD/widgets/GraphWindowAbstract/*.cpp) \
    $$files($$PWD/widgets/HtmlWindow/*.cpp) \
    $$files($$PWD/widgets/Scene2D/*.cpp) \
    $$files($$PWD/widgets/Scene3DAbstract/*.cpp) \
    $$files($$PWD/widgets/SplashScreenWindow/*.cpp)

FORMS += \
    $$files($$PWD/widgets/GraphWindowAbstract/*.ui) \
    $$files($$PWD/widgets/HtmlWindow/*.ui) \
    $$files($$PWD/widgets/SplashScreenWindow/*.ui)

use_swrast {
    QT -= opengl
    DEFINES += USE_SWRAST
    SOURCES += $$files($$PWD/widgets/GLWidgetImpl/software/*.cpp)
    HEADERS += $$files($$PWD/widgets/GLWidgetImpl/software/*.h)
}
