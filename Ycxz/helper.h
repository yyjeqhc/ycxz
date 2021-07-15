#ifndef HELPER_H
#define HELPER_H

#include <QWidget>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QVBoxLayout>

#include "tcpsocket.h"
#include <QHostAddress>
#include <QListWidget>
#include <QMenu>
#include <QFile>
#include <QPixmap>

#include "showscreen.h"

//协助方。
class Helper : public QWidget
{
    Q_OBJECT
public:
    explicit Helper(QWidget *parent = nullptr);

public slots:
    void connectToserver();//连接按钮
    void disconnectWithserver();//断开按钮
    void sendMsg();//发送聊天消息
    void newData();//接收到消息
    void writeData(qint64);//记录写的字节数
    void checkLogin(QString);//登录验证
    void sendData(datatype,QByteArray);//发送数据
    void disconnectByserver();//与服务器断开了连接
    void showFileDir(QString);//展示服务端发过来的路径
    void slotDirShow(QListWidgetItem*);//双击目录事件
    void refreshServerDir();//刷新目录列表
    void downloadServerfile();//下载选中文件，暂时不支持多选
    void saveFile();//保存文件
    void newfileData();//接收到消息
    void writefileData(qint64);//记录写的字节数
    void sendfileData(datatype,QByteArray);//发送数据
    void disconnectByfileserver();//与服务器断开了连接
    void startScreen();
    void connectToScreen();
    void newScreenData();
    void showScreen(QByteArray msg);
    void sendScreenData(controltype type,QByteArray data);
    void disconnectByScreenserver();
    void closeScreen();
private:
    QGridLayout* mainLayout;
    QPushButton* lj;
    QPushButton* dk;
    QPushButton* fs;
    QPushButton* pm;
    QLabel* label_ip;
    QLineEdit* ip;
    QLabel* label_send;
    QLineEdit* send;
    QLabel* label_recv;
    QLineEdit* recv;
    QListWidget* file;
    QMenu* menu;
    QLabel* label_s;
    bool lianjie;
    bool check;
    bool fconnect;

    QHostAddress* serverIp;
    TcpSocket* msocket;
    QByteArray* readdata;
    TcpSocket* fsocket;
    QByteArray* fdata;
    QString filename;
    QFile* recvfile;
    qint64 totalsize;
    qint64 recvsize;
    TcpSocket* ssocket;
    QByteArray* sdata;
    int height;
    int width;

    class showScreen* screena;


    const QString DIRS = "dirs";
    const QString FILES = "files";
    const QString SPLIT = "split";
    const QByteArray BEGIN = QByteArray("begin");
    const QByteArray END = QByteArray("end");


protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);
signals:

};

#endif // HELPER_H
