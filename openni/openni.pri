
INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/xtion.cpp

HEADERS += \
	$$PWD/xtion.h

win32 {

} macx {

	QMAKE_INCDIR += \
		/usr/local/include

	QMAKE_LIBDIR += \
		/usr/local/lib/ni2

	QMAKE_LIBS += \
		-lopenni2
}
