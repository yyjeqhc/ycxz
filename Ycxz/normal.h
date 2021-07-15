#ifndef NORMAL_H
#define NORMAL_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include "tcpserver.h"
#include "tcpsocket.h"
#include <QGridLayout>
#include <QRegExp>
#include <QRegExpValidator>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QDir>
#include <QFileSystemWatcher>
#include <QHostAddress>
#include <QFile>
#include <QTime>
#include "screen.h"

//这个是受助方。
class Normal : public QWidget
{
    Q_OBJECT
public:
    explicit Normal(QWidget *parent = nullptr);
    void showIP();//展示各网卡拥有的IP地址

public slots:
    void beginServer();//开启按钮
    void stopServer();//关闭按钮
    void chooseIP();//选择IP
    void newConnection(QTcpSocket *s);//新的连接
    void writeData(qint64);//记录写了多少字节
    void newData();//数据到达
    void sendMsg();//发送聊天消息
    void beginCheck();//开启验证
    void checkLogin(QString);//登录验证
    void sendData(datatype,QByteArray);//发送数据
    void disconnectWithclient();//断开连接
    void showFiledir();//展示文件
    void sendFileDir();//传输文件目录
    void beginFileserver();//开启传输文件
    void stopFileserver();//关闭传输文件
    void FilenewConnection(QTcpSocket *s);//新的文件传输连接
    void sendFile(QString msg);//传输文件
    void fsFile();//发送文件
    void newfileData();//文件传输数据
    void sendfileData(TcpSocket*,datatype,QByteArray);//发送数据
    void startScreen();//开始屏幕分享
    void stopScreen();//停止屏幕分享

private:
    /*
     *界面
    */
    QString key;    //访问需要密码
    QString ip;     //访问需要IP地址
    QLineEdit* mip; //暂时用下拉列表框代替
    QComboBox *n_ip; //临时代替IP
    QLabel* label_ip;
    QLabel* label_key;
    QLineEdit* mkey;
    QGridLayout *mainLayout;
    QPushButton* begin;
    QPushButton* stop;
    QPushButton* fs;
    QPushButton* cs;
    QPushButton* gb;

    QLabel* label_send;
    QLineEdit* send;
    QLabel* label_recv;
    QLineEdit* recv;

    QListWidget* file;
    QDir* dir;//本地
    QDir* clientdir;
    QFileSystemWatcher* watcher;
    QFile* cfile;
    qint64 totalsize;
    qint64 sendsize;
    qint64 tobesend;
    qint64 oncesize;
    QTime time;

    QRegExpValidator* validator;
    TcpServer* mserver;
    TcpSocket* msocket;
    QHostAddress clientIp;

    TcpServer* fserver;
    QList<TcpSocket*> fsocket;
    QByteArray* readdata;
    QByteArray* fdata;
    Screen* screen;
    QPushButton* bbutton;
    QPushButton* sbutton;

    bool listen;
    bool check;
    bool trans;
    bool filebegin;
    int checktimes;



    const QString DIRS = "dirs";
    const QString FILES = "files";
    const QString SPLIT = "split";
    const QByteArray BEGIN = QByteArray("begin");
    const QByteArray END = QByteArray("end");


signals:

};

#endif // NORMAL_H
