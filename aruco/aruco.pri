INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/marker_tracker.cpp

HEADERS += \
	$$PWD/marker_tracker.h

win32 {

    QMAKE_INCDIR += \
        $$PWD/include \
        $$(OpenCV_DIR)/include

    QMAKE_LIBDIR += \
        $$PWD/lib \
        $$(OpenCV_DIR)/x86/vc12/lib

    CONFIG(debug, debug|release) {
        QMAKE_LIBS += \
            -laruco130d -lpolypartitiond
    }

    CONFIG(release, debug|release) {
        QMAKE_LIBS += \
            -laruco130 -lpolypartition
    }

} macx {

	QMAKE_INCDIR += \
		$$PWD/include \
		/usr/local/include

	QMAKE_LIBDIR += \
		$$PWD/lib \
		/usr/local/lib

	QMAKE_LIBS += \
		-laruco -lopencv_core -lpolypartition

}
