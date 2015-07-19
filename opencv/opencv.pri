INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/image.cpp \
    $$PWD/camera.cpp \
    $$PWD/homography.cpp \
    $$PWD/diff_image.cpp \
    $$PWD/landolt_tracker.cpp \
    $$PWD/reverse_image.cpp \

HEADERS += \
    $$PWD/image.h \
    $$PWD/camera.h \
    $$PWD/homography.h \
    $$PWD/diff_image.h \
    $$PWD/landolt_tracker.h \
    $$PWD/reverse_image.h \

win32 {

    QMAKE_INCDIR += \
        $$(OpenCV_DIR)/include

    QMAKE_LIBDIR += \
        $$(OpenCV_DIR)/x86/vc12/lib

    CONFIG(debug, debug|release) {
        QMAKE_LIBS += \
            -lopencv_core2411d -lopencv_highgui2411d -lopencv_imgproc2411d -lopencv_calib3d2411d
    }

    CONFIG(release, debug|release) {
        QMAKE_LIBS += \
            -lopencv_core2411 -lopencv_highgui2411 -lopencv_imgproc2411 -lopencv_calib3d2411
    }

}

macx {

	QMAKE_INCDIR += \
		/usr/local/include

	QMAKE_LIBDIR += \
		/usr/local/lib

	QMAKE_LIBS += \
		-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_calib3d
}
