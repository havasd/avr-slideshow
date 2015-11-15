TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt


CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.rcc
UI_DIR = $${DESTDIR}/.ui


SOURCES += \
    src/main.c

HEADERS += \
    src/charmap.h

