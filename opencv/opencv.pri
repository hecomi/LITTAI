INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/image.cpp \
	$$PWD/camera.cpp \
	$$PWD/homography.cpp \
    $$PWD/diff_image.cpp \
    $$PWD/landolt_tracker.cpp

HEADERS += \
	$$PWD/image.h \
	$$PWD/camera.h \
	$$PWD/homography.h \
    $$PWD/diff_image.h \
    $$PWD/landolt_tracker.h

win32 {

} macx {

	QMAKE_INCDIR += \
		/usr/local/include

	QMAKE_LIBDIR += \
		/usr/local/lib

	QMAKE_LIBS += \
		-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_calib3d
}
