win32 {

	INCLUDEPATH += $$PWD

	SOURCES += \
        $$PWD/realsense.cpp

	HEADERS += \
		$$PWD/realsense.h

    QMAKE_INCDIR += \
        $$(RSSDK_DIR)/include

    QMAKE_LIBDIR += \
        $$(RSSDK_DIR)/lib/Win32

    QMAKE_LIBS += -ladvapi32

    CONFIG(debug, debug|release) {
        QMAKE_LIBS += -llibpxc_d
    }

    CONFIG(release, debug|release) {
        QMAKE_LIBS += -llibpxc
    }

}
