#ifndef SCREEN_H
#define SCREEN_H

#include <QObject>
#include "tcpsocket.h"
#include "tcpserver.h"
#include <QHostAddress>
#include <QTimer>
#include <QLibrary>
#include <QVector>
#include <windows.h>
#include <winuser.h>
#include "control.h"
#include <QMetaType>
#include <iostream>
#include <fstream>

class Screen : public QObject
{
    Q_OBJECT
public:
    explicit Screen(QObject *parent = nullptr);

public slots:
    void newConnection(QTcpSocket *s);//新的连接
    void writeData(qint64);//记录写了多少字节
    void newData();//数据到达
    void sendData(controltype,QByteArray);//发送数据
    void startScreen();//开始屏幕分享
    void timeComing();//时钟周期事件
    void disconnectWithclient();//断开连接
    void mouseRelease();//处理鼠标信息
    void keyboardRelease();//处理键盘信息
    void stop();//停止屏幕分享
    void keyEvent(QString msg);//处理键盘信息

private:
    TcpServer* mserver;
    TcpSocket* msocket;
    QByteArray* readdata;
    bool fx;
    QTimer mytime;

    int height;
    int width;
    int count;

    std::ofstream outfile;

    const QByteArray BEGIN = QByteArray("begin");
    const QByteArray END = QByteArray("end");
    const QString SPLIT = "split";

    //本来想用dll的，但是效果不尽人意，最后还是采用lib
//    typedef int (*Hello)(int,int,int,int);
//    Hello keybd_event;
//    typedef void (*World)(int,int,int,int,int);
//    World mouse_event;
    bool press;

    //自己定义的接收到的键盘、鼠标消息
    struct kb{
        int code,type;
    };
    QVector<kb> va;

    struct sj{
        int w,h,value,type;
    };
    QVector<sj> v;

signals:
    void begina(QVector<sj> v);
    void keyboard(QVector<kb> v);
};

#endif // SCREEN_H
