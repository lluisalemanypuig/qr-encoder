QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += openglwidgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -fopenmp

QMAKE_CXXFLAGS_DEBUG += -DDEBUG -D_GLIBCXX_DEBUG -Og
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -UDEBUG -DNDEBUG -fstrict-aliasing -O3

LIBS += -lgmp -fopenmp -lpthread

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qrcode/qrcodegen.cpp \
    qrrenderer.cpp \
    resizablemessagebox.cpp \
    sliderandlabel.cpp

HEADERS += \
    mainwindow.hpp \
    qrcode/qrcodegen.hpp \
    qrrenderer.hpp \
    resizablemessagebox.hpp \
    sliderandlabel.hpp

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
	qr-encoder_en_GB.ts \
	qr-encoder_ca_ES.ts \
	qr-encoder_es_ES.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
