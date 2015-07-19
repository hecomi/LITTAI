INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/xtion.cpp

HEADERS += \
	$$PWD/xtion.h

win32 {

        QMAKE_INCDIR += \
                $$(OPENNI_DIR)/Include

        QMAKE_LIBDIR += \
                $$(OPENNI_DIR)/Lib

        QMAKE_LIBS += \
                -lOpenNI2

} macx {

        QMAKE_INCDIR += \
		/usr/local/include

	QMAKE_LIBDIR += \
		/usr/local/lib/ni2

	QMAKE_LIBS += \
		-lopenni2
}
