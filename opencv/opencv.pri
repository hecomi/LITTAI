INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/image.cpp \
	$$PWD/camera.cpp

HEADERS += \
	$$PWD/image.h \
	$$PWD/camera.h

win32 {

} macx {

	QMAKE_INCDIR += \
		/usr/local/include

	QMAKE_LIBDIR += \
		/usr/local/lib

	QMAKE_LIBS += \
		-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_calib3d
}
