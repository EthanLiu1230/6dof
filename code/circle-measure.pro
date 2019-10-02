#-------------------------------------------------
#
# Project created by QtCreator 2017-07-05T23:35:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = circle-measure
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cvmatqimage.cpp \
    mylabel.cpp \
    singlecircle.cpp

HEADERS  += mainwindow.h \
    cvmatqimage.h \
    mylabel.h \
    singlecircle.h \
    watershedSegmentation.h

FORMS    += mainwindow.ui

# PC
LIBS += -L/usr/local/lib \
        -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_videoio


## ARM
#INCLUDEPATH += /usr/local/oecore-x86_64/sysroots/armv7at2hf-neon-angstrom-linux-gnueabi/usr/include
#LIBS += -L/usr/local/oecore-x86_64/sysroots/armv7at2hf-neon-angstrom-linux-gnueabi/lib \
#        -L/usr/local/oecore-x86_64/sysroots/armv7at2hf-neon-angstrom-linux-gnueabi/usr/lib \
#        -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_videoio

#INCLUDEPATH+=C:\opencv\build\include\opencv\
#C:\opencv\build\include\opencv2\
#C:\opencv\build\include
#CONFIG(debug,debug|release){
#LIBS+=C:\opencv\build\x86\vc12\lib\opencv_calib3d2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_contrib2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_core2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_features2d2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_flann2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_gpu2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_highgui2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_imgproc2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_legacy2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_ml2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_nonfree2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_objdetect2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_photo2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_stitching2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_video2413d.lib\
#C:\opencv\build\x86\vc12\lib\opencv_videostab2413d.lib
#}else{
#LIBS+=C:\opencv\build\x86\vc12\lib\opencv_calib3d2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_contrib2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_core2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_features2d2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_flann2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_gpu2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_highgui2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_imgproc2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_legacy2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_ml2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_nonfree2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_objdetect2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_photo2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_stitching2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_video2413.lib\
#C:\opencv\build\x86\vc12\lib\opencv_videostab2413.lib
#}

RESOURCES += \
    btn.qrc


