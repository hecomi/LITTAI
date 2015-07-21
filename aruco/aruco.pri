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
            -laruco130d -lopencv_core2411d -lopencv_imgproc2411d -lopencv_highgui2411d -lopencv_video2411d -lpolypartitiond
    }

    CONFIG(release, debug|release) {
        QMAKE_LIBS += \
            -laruco130 -lopencv_core2411 -lopencv_imgproc2411 -lopencv_highgui2411 -lopencv_video2411 -lpolypartition
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
