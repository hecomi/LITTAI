INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/image.cpp \
	$$PWD/camera.cpp \
	$$PWD/homography.cpp \
	$$PWD/analyzer.cpp \
    $$PWD/tracker.cpp

HEADERS += \
	$$PWD/image.h \
	$$PWD/camera.h \
	$$PWD/homography.h \
	$$PWD/analyzer.h \
    $$PWD/tracker.h

win32 {

} macx {

	QMAKE_INCDIR += \
		/usr/local/include

	QMAKE_LIBDIR += \
		/usr/local/lib

	QMAKE_LIBS += \
		-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_calib3d
}
