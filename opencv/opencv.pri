INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/opencv_image.cpp

HEADERS += \
	$$PWD/opencv_image.h

win32 {

} macx {

	QMAKE_INCDIR += \
		/usr/local/include

	QMAKE_LIBDIR += \
		/usr/local/lib

	QMAKE_LIBS += \
		-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_calib3d
}

QMAKE_CXXFLAGS_WARN_OFF += -Wno-overloaded-virtual
