#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

//定义消息类型，貌似太多，有些没用上。
enum datatype{
    CHECK,RECHECK,TEXT,FILEA,FILEB,FILEC,ERRORA,QUIT,DIR,REFRESH,CD,SCREENA,SCREENB
};
//定义控制信息，还是太多，有些没用上。
enum controltype{
    KEYP,KEYR,MOUSEP,MOUSER,MOUSED,MOUSEW,MOUSEM,NONE,SDATA,KEYN
};

//此文件大体来自https://github.com/jiexixijie/ZeroNet，本人仅做适量修改以便使用
class TcpSocket : public QObject
{
    Q_OBJECT
public:
    // 初始化socket
    // @sock: 把sock加到这个类mSock的私有变量中
    explicit TcpSocket(QTcpSocket *sock, QObject *parent = 0);
    // 获取socket
    QTcpSocket *socket() {
        return mSock;
    }
    // 获取缓存区
    QByteArray *buffer() {
        return &mBuf;
    }
    // 断开和客户之间的连接
    void close();
    // 发送数据
    void write(QByteArray data);

private:
    QTcpSocket *mSock;  // 客户
    QByteArray mBuf;    // 数据缓冲区，从客户里接收到的数据都会先放在这里

signals:
    // 当有新数据加入到mBuf后就发射这个信号，让调用这个类的类知道，
    // 然后在对新的数据作出相应的处理
    void newData();
    // 当客户断开是发射的信号
    void disconnectedFromserver();

public slots:
    void readReady();
    void disconnected();
    void connected();
};

#endif // TCPSOCKET_H
