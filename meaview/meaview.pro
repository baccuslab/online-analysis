######################################################################
# Automatically generated by qmake (3.0) Sun Mar 1 10:20:03 2015
######################################################################

TEMPLATE = app
TARGET = meaview
INCLUDEPATH += ./include ../ \
		/usr/local/Cellar/hdf5/1.8.14/include \
		/usr/local/Cellar/boost/1.57.0/include/
MOC_DIR = ./build
OBJECTS_DIR = ./build
QT += widgets printsupport core concurrent
CONFIG += c++11 debug
QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++ -g -Wno-deprecated-register -O0 
LIBS += -L/usr/local/lib -lh5recording

# Input
HEADERS += 	../h5recording/include/h5recording.h \
			include/channelplot.h \
			include/settings.h \
			include/qcustomplot.h \
			include/windows.h \
			include/plotwindow.h \
			include/files.h \
			include/recording.h \
			include/ctrlwindow.h \
			include/playback.h
SOURCES += 	src/channelplot.cpp \
			src/settings.cpp \
			src/main.cpp \
			src/qcustomplot.cpp \
			src/windows.cpp \
			src/plotwindow.cpp \
			src/files.cpp \
			src/recording.cpp \
			src/ctrlwindow.cpp \
			src/playback.cpp