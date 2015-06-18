INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/marker_tracker.cpp

HEADERS += \
	$$PWD/marker_tracker.h

win32 {

} macx {

	QMAKE_INCDIR += \
		$$PWD/include \
		/usr/local/include

	QMAKE_LIBDIR += \
		$$PWD/lib \
		/usr/local/lib

	QMAKE_LIBS += \
		-laruco -lopencv_core
}
