//     Copyright (c) 2014 spacelan1993@gmail.com All rights reserved.

#include "nehewidget.h"
#include "ui_nehewidget.h"
#include "math.h"

//构造函数
NeHeWidget::NeHeWidget(QGLWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::NeHeWidget)
{
    ui->setupUi(this);

    quatToMatrix(1,0,0,0);

    //旋转角度
    xRot = yRot = zRot = 0;

    //定时更新
    refreshTimer = new QTimer(this);
    connect(refreshTimer,SIGNAL(timeout()),this,SLOT(update()));
    refreshTimer->setInterval(40);
    refreshTimer->start();

    //窗口位置标题
    setGeometry(100,100,648,480);
    setWindowTitle(tr("终于弄好了有木有！！！"));

    //是否全屏
    fullscreen = false;
}

//析构函数
NeHeWidget::~NeHeWidget()
{
    delete refreshTimer;
    delete ui;
}

//初始化函数
void NeHeWidget::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    //纹理
    loadGLTextures();
    glEnable( GL_TEXTURE_2D );

}

//绘制函数
void NeHeWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //真实视点
    gluLookAt(-0.5,-4,6,0,0,0,0,0,1);
    glRotatef(xRot,1,0,0);
    glRotatef(yRot,0,1,0);
    glRotatef(zRot,0,0,1);
    //绘制长方体
    glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_CURRENT_BIT);
    glPushMatrix();

    glMultMatrixf(Matrix4);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_QUADS);
        //z+
        glTexCoord2f( 0.0, 0.0 ); glVertex3f( -1.0, -1.0,  0.1/2 );
        glTexCoord2f( 1.0, 0.0 ); glVertex3f(  1.0, -1.0,  0.1/2 );
        glTexCoord2f( 1.0, 1.0 ); glVertex3f(  1.0,  1.0,  0.1/2 );
        glTexCoord2f( 0.0, 1.0 ); glVertex3f( -1.0,  1.0,  0.1/2 );
    glEnd();

    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBegin(GL_QUADS);
        //z-
        glTexCoord2f( 1.0, 0.0 ); glVertex3f( -1.0, -1.0, -0.1/2 );
        glTexCoord2f( 1.0, 1.0 ); glVertex3f( -1.0,  1.0, -0.1/2 );
        glTexCoord2f( 0.0, 1.0 ); glVertex3f(  1.0,  1.0, -0.1/2 );
        glTexCoord2f( 0.0, 0.0 ); glVertex3f(  1.0, -1.0, -0.1/2 );
    glEnd();

    glBegin(GL_QUADS);
        //y+
        glVertex3f( -1.0,  1.0, -0.1/2 );
        glVertex3f( -1.0,  1.0,  0.1/2 );
        glVertex3f(  1.0,  1.0,  0.1/2 );
        glVertex3f(  1.0,  1.0, -0.1/2 );
        //y-
        glVertex3f( -1.0, -1.0, -0.1/2 );
        glVertex3f(  1.0, -1.0, -0.1/2 );
        glVertex3f(  1.0, -1.0,  0.1/2 );
        glVertex3f( -1.0, -1.0,  0.1/2 );
        //x+
        glVertex3f(  1.0, -1.0, -0.1/2 );
        glVertex3f(  1.0,  1.0, -0.1/2 );
        glVertex3f(  1.0,  1.0,  0.1/2 );
        glVertex3f(  1.0, -1.0,  0.1/2 );
        //x-
        glVertex3f( -1.0, -1.0, -0.1/2 );
        glVertex3f( -1.0, -1.0,  0.1/2 );
        glVertex3f( -1.0,  1.0,  0.1/2 );
        glVertex3f( -1.0,  1.0, -0.1/2 );
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glPopAttrib();

    //画世界坐标轴
    glPushMatrix();
    glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_CURRENT_BIT);

    float LENGTH = 2;
    float ARROW = 0.1 * LENGTH;
    float HEIGHT = 0.06 * LENGTH;
    glBegin(GL_LINES);
    {
        glColor3f(1,0,0);
        glVertex3f(-LENGTH,0.0,0.0);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f((LENGTH - ARROW),HEIGHT,0.0);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f((LENGTH - ARROW),-HEIGHT,0.0);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f((LENGTH - ARROW),0.0,HEIGHT);
        glVertex3f(LENGTH,0.0,0.0);
        glVertex3f((LENGTH - ARROW),0.0,-HEIGHT);
        //
        glColor3f(0,1,0);
        glVertex3f(0.0,-LENGTH,0.0);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(0.0,(LENGTH - ARROW),HEIGHT);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(0.0,(LENGTH - ARROW),-HEIGHT);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(HEIGHT,(LENGTH - ARROW),0.0);
        glVertex3f(0.0,LENGTH,0.0);
        glVertex3f(-HEIGHT,(LENGTH - ARROW),0.0);
        //
        glColor3f(0,0,1);
        glVertex3f(0.0,0.0,-LENGTH);
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(HEIGHT,0.0,(LENGTH - ARROW));
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(-HEIGHT,0.0,(LENGTH - ARROW));
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(0.0,HEIGHT,(LENGTH - ARROW));
        glVertex3f(0.0,0.0,LENGTH);
        glVertex3f(0.0,-HEIGHT,(LENGTH - ARROW));
    }
    glEnd();
    glPopMatrix();
    glPopAttrib();
}


void NeHeWidget::resizeGL(int width, int height)
{
    if(height == 0)
        height = 1;
    glViewport(0,0,(GLint) width,(GLint) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(GLfloat) width / (GLfloat) height,0.1,100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//键盘事件
void NeHeWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_F2:
        fullscreen = !fullscreen;
        if(fullscreen)
            showFullScreen();
        else
        {
            showNormal();
           // setGeometry();
        }
        break;
    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_Up:
        xRot -= 2;
        break;
    case Qt::Key_Down:
        xRot += 2;
        break;
    case Qt::Key_Left:
        yRot -= 2;
        break;
    case Qt::Key_Right:
        yRot += 2;
        break;
    case Qt::Key_PageUp:
        zRot += 2;
        break;
    case Qt::Key_PageDown:
        zRot -= 2;
        break;
    default:
        break;
    }
    updateGL();
}

void NeHeWidget::loadGLTextures()
{
    QImage tex,buf;
    QImage myImage1(":/myImage/up.png");
    tex = QGLWidget::convertToGLFormat(myImage1);
    glGenTextures(2,&texture[0]);
    glBindTexture( GL_TEXTURE_2D,texture[0] );
    glTexImage2D( GL_TEXTURE_2D,0,3,tex.width(),tex.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,tex.bits() );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );

    QImage myImage2(":/myImage/down.png");
    buf = QGLWidget::convertToGLFormat(myImage2);
//    glGenTextures(1,&texture[1]);
    glBindTexture( GL_TEXTURE_2D,texture[1] );
    glTexImage2D( GL_TEXTURE_2D,0,3,buf.width(),buf.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,buf.bits() );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
}

void NeHeWidget::closeEvent(QCloseEvent *e)
{
    emit closeWidget();
    e->accept();
}

void NeHeWidget::quatToMatrix(float w, float x, float y, float z)
{
    //不知道为什么，旋转方向是反的，所以求反
    x = -x; y = -y; z = -z;

    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    // This calculation would be a lot more complicated for non-unit length quaternions
    // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
    //   OpenGL
//    Matrix4 = {1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
//            2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
//            2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
//            0.0f, 0.0f, 0.0f, 1.0f};
    Matrix4[0] = 1.0f - 2.0f * (y2 + z2);
    Matrix4[1] = 2.0f * (xy - wz);
    Matrix4[2] = 2.0f * (xz + wy);
    Matrix4[3] = 0.0f;
    Matrix4[4] = 2.0f * (xy + wz);
    Matrix4[5] = 1.0f - 2.0f * (x2 + z2);
    Matrix4[6] = 2.0f * (yz - wx);
    Matrix4[7] = 0.0f;
    Matrix4[8] = 2.0f * (xz - wy);
    Matrix4[9] = 2.0f * (yz + wx);
    Matrix4[10] = 1.0f - 2.0f * (x2 + y2);
    Matrix4[11] = 0.0f;
    Matrix4[12] = 0.0f;
    Matrix4[13] = 0.0f;
    Matrix4[14] = 0.0f;
    Matrix4[15] = 1.0f;

    paintGL();
}



