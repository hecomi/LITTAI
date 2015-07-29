win32 {

	INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/kinect_v2.h

    SOURCES += \
        $$PWD/kinect_v2.cpp

    QMAKE_INCDIR += \
        $$(KINECTSDK20_DIR)/inc

    QMAKE_LIBDIR += \
        $$(KINECTSDK20_DIR)/Lib/x86

    QMAKE_LIBS += -lKinect20

}
