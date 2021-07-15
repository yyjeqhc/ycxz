#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>

//此文件大体来自https://github.com/jiexixijie/ZeroNet，本人仅做适量修改以便使用
class TcpServer : public QObject
{
    Q_OBJECT
public:
    //explicit：显示调用，禁止隐式调用
    explicit TcpServer(QObject *parent = 0);

    // 启动服务端
    // @port: 监听的端口
    void start(int port);
    void stop();
    void close();

    // 反回服务器
    QTcpServer *server() {
        return mServer;
    }

private:
    QTcpServer *mServer;  // 在构造函数里初始化

signals:
    // 当新的连接进来时发送的信号
    // @sock: 新的连接
    void newConnection(QTcpSocket *sock);

public slots:
    // 当有从mServer中接收到新连接后，获取新连接的socket，然后再
    // 发射newConnection信号
    void newConnection();
};

#endif // TCPSERVER_H
