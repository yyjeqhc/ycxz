#include "tcpsocket.h"
#include <QDataStream>

TcpSocket::TcpSocket(QTcpSocket *sock, QObject *parent):
    QObject(parent), mSock(sock)
{
    mSock->setParent(this);
    connect(mSock, SIGNAL(readyRead()), this, SLOT(readReady()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(mSock,SIGNAL(connected()),this,SLOT(connected()));
}

void TcpSocket::close()
{
    mSock->disconnectFromHost();
    if(mSock!=NULL)
    {
        mSock->close();
    }
    mSock = NULL;
}

void TcpSocket::write(QByteArray data)
{
    mSock->write(data);
    if (!mSock->waitForBytesWritten(3000)) {
        // 发送数据超时
        close();
        emit disconnected();
        // 输出信息
        qDebug() << mSock->peerAddress().toString() << ":" << mSock->peerPort() \
                 << " 写入失败：" << mSock->errorString();
    }
}

void TcpSocket::readReady()
{
    emit newData();
}

void TcpSocket::disconnected()
{
    emit disconnectedFromserver();
}

void TcpSocket::connected()
{
    qDebug()<<"建立了连接";
}
