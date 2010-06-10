//IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

// By downloading, copying, installing or using the software you agree to this license.
// If you do not agree to this license, do not download, install,
// copy or use the software.


//                          License Agreement
//               For Open Source Computer Vision Library

//Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
//Copyright (C) 2008-2010, Willow Garage Inc., all rights reserved.
//Third party copyrights are property of their respective owners.

//Redistribution and use in source and binary forms, with or without modification,
//are permitted provided that the following conditions are met:

//  * Redistribution's of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.

//  * Redistribution's in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.

//  * The name of the copyright holders may not be used to endorse or promote products
//  derived from this software without specific prior written permission.

//This software is provided by the copyright holders and contributors "as is" and
//any express or implied warranties, including, but not limited to, the implied
//warranties of merchantability and fitness for a particular purpose are disclaimed.
//In no event shall the Intel Corporation or contributors be liable for any direct,
//indirect, incidental, special, exemplary, or consequential damages
//(including, but not limited to, procurement of substitute goods or services;
//loss of use, data, or profits; or business interruption) however caused
//and on any theory of liability, whether in contract, strict liability,
//or tort (including negligence or otherwise) arising in any way out of
//the use of this software, even if advised of the possibility of such damage.

//--------------------Google Code 2010 -- Yannick Verdie--------------------//

#include "precomp.hpp"

#if defined(OPENCV_GL)
#include <QGLWidget>
#endif

#include <QAbstractEventDispatcher>
#include <QtGui/QApplication>
#include <QFile>
#include <QPushButton>
#include <QtGui/QGraphicsView>
#include <QSizePolicy>
#include <QInputDialog>
#include <QtGui/QBoxLayout>
#include <QSettings>
#include <qtimer.h>
#include <QtConcurrentRun>
#include <QWaitCondition>
#include <QKeyEvent>
#include <QMetaObject>
#include <QPointer>
#include <QSlider>
#include <QLabel>
#include <QIODevice>



//Macro here
#define CV_MODE_NORMAL   0
#define CV_MODE_OPENGL   1
#define PATH_QSLIDERCSS "../SliderCss.qss"
//end macro

//Here declare everything
//need a .h to allow QT to use MOC (in order to use signal, slots, etc)

class GuiReceiver : public QObject
{
    Q_OBJECT

public:
    GuiReceiver();
    int start();
    bool _bTimeOut;
private:


public slots:
    void createWindow( QString name, int flags = 0 );
    void destroyWindow(QString name);
    void destroyAllWindow();
    void addSlider(QString trackbar_name, QString window_name, void* value, int count, void* on_change);
    void moveWindow(QString name, int x, int y);
    void resizeWindow(QString name, int width, int height);
    void showImage(QString name, void* arr);
    void displayInfo( QString name, QString text, int delayms );
    void refreshEvents();
    void timeOut();
};

class CvTrackbar : public QHBoxLayout
{
    Q_OBJECT
public:
    CvTrackbar(CvWindow* parent, QString name, int* value, int count, CvTrackbarCallback on_change = NULL);
    ~CvTrackbar();

    QString trackbar_name;
    QPointer<QSlider> slider;

private slots:
    void createDialog();
    void update(int myvalue);

private:
    void setLabel(int myvalue);

    QString createLabel();
    QPointer<QPushButton > label;
    CvTrackbarCallback callback;
    CvWindow* parent;
    int* dataSlider;
};

class CvWindow : public QWidget
{
    Q_OBJECT
public:
    CvWindow(QString arg2, int flag = CV_WINDOW_NORMAL);
    ~CvWindow();
    void addSlider(QString name, int* value, int count, CvTrackbarCallback on_change = NULL);
    void setMouseCallBack(CvMouseCallback m, void* param);
    void updateImage(void* arr);
    void displayInfo(QString text, int delayms );

    QPointer<QBoxLayout> layout;
    QString name;
    int flags;

protected:
    void readSettings();
    void writeSettings();

    virtual void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QPointer<ViewPort> myview;
    
    int status;//0 normal, 1 fullscreen (YV)

    CvMouseCallback on_mouse;
    void* on_mouse_param;

};



class ViewPort : public QGraphicsView
{
    Q_OBJECT
public:
    ViewPort(QWidget* centralWidget, int mode = CV_MODE_NORMAL);
    ~ViewPort();
    void updateImage(void* arr);
    void startDisplayInfo(QString text, int delayms);

public slots:
    void zoomIn() { scale(1.2, 1.2); }
    void zoomOut() { scale(1 / 1.2, 1 / 1.2); }
    void rotateLeft() { rotate(-10); }
    void rotateRight() { rotate(10); }

private:
    int mode;
    IplImage* image2Draw;
    bool isSameSize(IplImage* img1,IplImage* img2);
    QSize sizeHint() const;
    QWidget* centralWidget;
    QTimer* timerDisplay;
    bool drawInfo;
    QString infoText;
    //QImage* image;

    void paintEvent(QPaintEvent* paintEventInfo);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void drawInstructions(QPainter *painter);
    void draw2D(QPainter *painter);

#if defined(OPENCV_GL)
    void draw3D();
    void unsetGL();
    void initGL();
    void setGL(int width, int height);
#endif

private slots:
    void stopDisplayInfo();
};
