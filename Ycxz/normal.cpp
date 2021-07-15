#include "normal.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QDataStream>
#include <QFileInfoList>

Normal::Normal(QWidget *parent) : QWidget(parent)
{
    // 先设置窗口的头相，资源图片在上面下载
//    this->setWindowIcon(QIcon(":/resources/zero.ico"));
    this->setWindowTitle("受助方");
    this->setWindowFlags(Qt::Widget|Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    // 设置窗口大小
    const int w = 350,  h = 250;
    // 将窗口至中，你必须在widget.h里#include <QDesktopWidget>
    const int x = (QApplication::desktop()->width() - w)/2;
    const int y = (QApplication::desktop()->height() - h)/2;
    this->setGeometry(x, y, w, h);
    //不允许最大化最小化？
    this->setMaximumSize(QSize(w, h));
    this->setMinimumSize(QSize(w, h));
    this->show();

    label_ip = new QLabel(this);
    label_ip->setText("你的IP地址：");
    label_key = new QLabel(this);
    label_key->setText("你的访问密码：");
    mip = new QLineEdit(this);
    mip->setReadOnly(true);
    n_ip = new QComboBox(this);
    connect(n_ip,SIGNAL(currentIndexChanged(int)),this,SLOT(chooseIP()));
    mkey = new QLineEdit(this);
    mkey->setPlaceholderText("6-10位字母数字混合密码");
    connect(mkey,SIGNAL(returnPressed()),this,SLOT(beginServer()));

    /*
     * 开启服务
    */
    begin = new QPushButton(this);
    begin->setText("开启服务");
    connect(begin,SIGNAL(clicked(bool)),this,SLOT(beginServer()));

    stop = new QPushButton(this);
    stop->setText("停止服务");
    connect(stop,SIGNAL(clicked(bool)),this,SLOT(stopServer()));

    fs = new QPushButton(this);
    fs->setText("发送");
    connect(fs,SIGNAL(clicked(bool)),this,SLOT(sendMsg()));

    cs = new QPushButton(this);
    cs->setText("传输文件");
    connect(cs,SIGNAL(clicked(bool)),this,SLOT(beginFileserver()));

    gb = new QPushButton(this);
    gb->setText("关闭传送");
    connect(gb,SIGNAL(clicked(bool)),this,SLOT(stopFileserver()));

    bbutton = new QPushButton(this);
    bbutton->setText("开启屏幕分享");
    connect(bbutton,SIGNAL(clicked()),this,SLOT(startScreen()));

    sbutton = new QPushButton(this);
    sbutton->setText("关闭屏幕分享");
    connect(sbutton,SIGNAL(clicked(bool)),this,SLOT(stopScreen()));

    label_send = new QLabel(this);
    label_send->setText("发送信息：");
    send = new QLineEdit(this);
    connect(send,SIGNAL(returnPressed()),this,SLOT(sendMsg()));

    label_recv = new QLabel(this);
    label_recv->setText("接收信息：");
    recv = new QLineEdit(this);

    QRegExp regExp("[A-Za-z0-9]{6,10}");
    validator = new QRegExpValidator(regExp,this);
    mkey->setValidator(validator);

    /*
     * 可自行设置路径，暂时设定为temp
    */
    dir = new QDir(QDir::currentPath());
    qDebug()<<dir->path();
    qDebug()<<"现在的目录"<<QDir::currentPath();
    dir->mkdir("temp");
    dir->cd("./temp");
//    qDebug()<<dir->path();
//    qDebug()<<dir->currentPath();
    clientdir = new QDir(dir->path());
    qDebug()<<dir->path();
    qDebug()<<clientdir->path();
    qDebug()<<clientdir->currentPath();
    file = new QListWidget(this);
    watcher = new QFileSystemWatcher(this);
    watcher->addPath(dir->path());
    connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(showFiledir()));

    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(label_ip,0,0);
    mainLayout->addWidget(mip,0,1);
    mainLayout->addWidget(n_ip,0,1);
    mainLayout->addWidget(label_key,1,0);
    mainLayout->addWidget(mkey,1,1);
    mainLayout->addWidget(label_send,2,0);
    mainLayout->addWidget(send,2,1);
    mainLayout->addWidget(label_recv,3,0);
    mainLayout->addWidget(recv,3,1);
    mainLayout->addWidget(stop,4,0);
    mainLayout->addWidget(begin,4,0);
    mainLayout->addWidget(fs,4,1);
    mainLayout->addWidget(cs,5,0);
    mainLayout->addWidget(gb,5,0);
    mainLayout->addWidget(sbutton,5,1);
    mainLayout->addWidget(bbutton,5,1);
    mainLayout->addWidget(file,6,0,2,2);
    mip->hide();
    stop->hide();
    gb->hide();
    startScreen();

    showIP();

    showFiledir();
    mkey->setFocus();
    fserver = NULL;
    readdata = new QByteArray;
    fdata = new QByteArray;

}

void Normal::showIP()
{
    QString hostName = QHostInfo::localHostName();
    qDebug()<<hostName;
//    hostName = "baidu.com";//可以查看域名的IP地址
    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    QList<QHostAddress> listAddress = hostInfo.addresses();
    foreach(QHostAddress ha,listAddress)
    {
        if(ha.protocol()==QAbstractSocket::IPv4Protocol)
        {
//            qDebug()<<ha.toString();
            n_ip->addItem(ha.toString());
        }
    }

    /*qDebug()<<"间隔";
    qDebug()<<n_ip->currentText();*/

    /*QString detail = "";
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface a,list)
    {
        qDebug()<<a.name();
    }*/
}

void Normal::beginServer()//5972
{
    mserver = new TcpServer(this);
    int pos=0;
    key = mkey->text();
    QValidator::State st=validator->validate(key,pos);
    if(QValidator::Acceptable!=st)
    {
        QMessageBox::warning(0,"提示","访问密码输入不合法，请重新输入");
        return;
    }
//    mkey->setReadOnly(true);
    QHostAddress ipdz("127.0.0.1");
    if(!mserver->server()->listen(QHostAddress::Any,5972))
    {
        qDebug()<<"监听失败："<<mserver->server()->errorString();
        return;
    }
    connect(mserver,SIGNAL(newConnection(QTcpSocket*)), this,SLOT(newConnection(QTcpSocket*)));
    msocket = NULL;
    //修改key的合法性
    listen=true;
    ip = n_ip->currentText();

    n_ip->hide();
    mip->show();
    mip->setText(ip);

    begin->hide();
    stop->show();
    qDebug()<<"开启服务";
}

void Normal::stopServer()
{
    qDebug()<<"点击按钮停止服务："<<msocket<<" "<<msocket->socket();
    mserver->stop();
    if(msocket!=NULL)
    {
        QByteArray data("主机关闭了服务");
        datatype data_type = QUIT;
        sendData(data_type,data);
    }
    qDebug()<<"stop server";
    /*if(mserver->server()->isListening())
    {
        mserver->server()->close();
        if(msocket!=NULL)
        {
            QByteArray data("主机关闭了服务");
            datatype data_type = QUIT;
            sendData(data_type,data);
            msocket->close();
        }
    }
    else
    {
        qDebug()<<"并没有在监听";
    }*/

    listen = false;
    mkey->setReadOnly(false);
    mip->hide();
    n_ip->show();
    stop->hide();
    begin->show();
    msocket = NULL;
    qDebug()<<msocket<<"已经设为空";

}

void Normal::chooseIP()
{
    QString c_ip = n_ip->currentText();
    qDebug()<<"正在切换IP"<<c_ip;
}

void Normal::newConnection(QTcpSocket *s)
{
    qDebug()<<"新的连接："<<msocket;
    if(msocket!=NULL)
    {
        qDebug()<<"已有一个连接，关闭新的连接";
        QByteArray dataa("你已被关闭连接");
//        sendData(QUIT,data);
        datatype data_type = QUIT;
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        out.setVersion(QDataStream::QDataStream::Qt_5_9);
        out<<data_type;
        out<<dataa;
        QByteArray fsdata;
        fsdata.append(BEGIN);
        fsdata.append(data);
        fsdata.append(END);
        msocket->write(fsdata);
        s->write(fsdata);
        s->disconnectFromHost();
        s->close();
        return;
    }
    check = false;
    checktimes = 3;
    msocket = new TcpSocket(s,this);
    connect(s,SIGNAL(bytesWritten(qint64)),this,SLOT(writeData(qint64)));
    connect(msocket,SIGNAL(newData()),this,SLOT(newData()));
    connect(msocket,SIGNAL(disconnectedFromserver()),this,SLOT(disconnectWithclient()));
    qDebug()<<"建立连接";
    beginCheck();
    clientIp = msocket->socket()->peerAddress();
}

void Normal::writeData(qint64 bytesnum)
{
    qDebug()<<"服务端写了"<<bytesnum<<"个字节的数据";
//    qApp->processEvents();
//    qDebug()<<trans;
//    if(trans)
//    {
//        qDebug()<<"为什么没有继续传送呢";
//        fsFile();
//    }
}

void Normal::newData()
{
    qDebug()<<"服务端收到数据";
    /*QDataStream in(msocket->socket());
    in.setVersion(QDataStream::Qt_5_9);
    int data_type;
    in>>data_type;
    datatype t = datatype(data_type);
    QByteArray data;
    in>>data;*/
//    qDebug()<<readdata;
//    readdata = new QByteArray;
    (*readdata).append(msocket->socket()->readAll());
//    QDataStream in(msocket->socket());
//    return;
//    qDebug()<<msocket->socket()->readAll();
    int size = (*readdata).size()-END.size();
    /*qDebug()<<readdata->size();
    qDebug()<<readdata->lastIndexOf(END);
    qDebug()<<END.size();*/
    /*if(readdata->indexOf(BEGIN)!=0||readdata->lastIndexOf(END)!=size)
    {
        qDebug()<<"数据接收不完整，还需要继续读取";
        return;
    }
    readdata->replace(BEGIN,NULL);
    readdata->replace(END,NULL);*/
    int left = readdata->indexOf(BEGIN);
    int right = readdata->lastIndexOf(END);
    if(left!=0||right!=size)
    {
        qDebug()<<"数据接收不完整，还需要继续读取";
        return;
    }
    readdata->replace(0,BEGIN.size(),NULL);
    right = readdata->lastIndexOf(END);
    readdata->replace(right,END.size(),NULL);
    QDataStream in(readdata,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_9);
    int data_type;
    in>>data_type;
    datatype t = datatype(data_type);
    QByteArray data;
    in>>data;
    readdata->resize(0);
    QString msg;
    if(!check)
    {
        msg = QString(data);
        qDebug()<<msg;
        switch (t){
        case CHECK:
            checkLogin(msg);
            break;
        case ERRORA:
             QMessageBox::warning(0,"错误",msg);
            break;
        case QUIT:
            qDebug()<<"客户选择断开连接："<<msg;
            break;
        default:
            QByteArray data("你已被关闭连接");
            sendData(QUIT,data);
//            msocket->socket()->abort();
            msocket->close();
            break;
        }
        return;
    }
    switch (t){
    case CHECK:
        break;
    case RECHECK:
        sendFileDir();
        break;
    case TEXT:
        msg = QString(data);
        recv->setText(msg);
        break;
    case FILEA:
        msg = QString(data);
        sendFile(msg);
        break;
    case FILEB:
        break;
    case FILEC:
        msg = QString(data);
        qDebug()<<msg;
        fsFile();
        break;
    case QUIT:
        msg = QString(data);
        QMessageBox::information(0,"结束",msg);
        msocket = NULL;
        break;
    case CD:
        msg = QString(data);
//        qDebug()<<"客户端请求切换路径："<<msg;
//        qDebug()<<"clientdir->path()="<<clientdir->path();
        clientdir->cd(msg);
//        qDebug()<<"切换后的clientdir->path()="<<clientdir->path();
        sendFileDir();
    case REFRESH:
        msg = QString(data);
        qDebug()<<msg;
        sendFileDir();
    case SCREENA:
        if(screen!=NULL)
        {
                QByteArray data("屏幕服务器已开启");
                sendData(SCREENA,data);
        }
        else
        {
            QByteArray data("nothing");//SCREENB代表屏幕分享没有开启
            sendData(SCREENB,data);
        }
        break;
    default:
        break;
    }
}

void Normal::sendMsg()
{
    datatype data_type = TEXT;
    QString text = send->text();
    if(text.isEmpty())
    {
        QMessageBox::warning(0,"提示","发送消息不能为空");
        return;
    }
    QByteArray data = text.toUtf8();
    sendData(data_type,data);
}

void Normal::beginCheck()
{
    check = false;
    checktimes=3;
    /*QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::QDataStream::Qt_5_9);
    datatype data_type = CHECK;
    out<<data_type;*/
    QByteArray msg("请输入指定的访问密码：");
//    out<<msg;
//    msocket->write(data);
    sendData(CHECK,msg);
}

void Normal::checkLogin(QString keya)
{
    qDebug()<<keya;
    if(key==keya)
    {
        QByteArray data("密码输入正确，开启正常访问");
        sendData(RECHECK,data);
        check = true;
    }
    else
    {
        checktimes--;
        if(checktimes<=0)
        {
            QByteArray data("密码输入错误，已关闭连接");
            sendData(QUIT,data);
            msocket->close();
            return;
        }
        QString str = "密码输入错误，还有"+QString::number(checktimes)+"次机会";
        QByteArray data = str.toUtf8();
        sendData(ERRORA,data);
    }
}

void Normal::sendData(datatype data_type, QByteArray dataa)
{
    if(msocket==NULL||!listen)
    {
        QMessageBox::warning(0,"错误","没有连接不能发送消息");
        return;
    }
    qDebug()<<"服务端开始发送数据"<<data_type;
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::QDataStream::Qt_5_9);
    out<<data_type;
    out<<dataa;
    QByteArray fsdata;
    fsdata.append(BEGIN);
    fsdata.append(data);
    fsdata.append(END);
    msocket->write(fsdata);
    qDebug()<<"服务端发送数据完毕";
}

void Normal::disconnectWithclient()
{
    qDebug()<<"为什么关闭了连接呢";
    if(msocket!=NULL)
    {
        if(msocket->socket()!=NULL)
        {
            msocket->close();
        }
        msocket = NULL;
        qDebug()<<"清空msocket";
    }
    clientIp = NULL;
    qDebug()<<"服务端与客户断开了连接";
    stopFileserver();
}

void Normal::showFiledir()
{
    file->clear();
    QFileInfoList list = dir->entryInfoList(QDir::AllEntries|QDir::NoDot);
    qDebug()<<"个数"<<list.count();
    for(int i=0;i<list.count();i++)
    {
        QFileInfo tmpInfo = list.at(i);
        if(tmpInfo.isDir())
        {
            QIcon icon("dir.png");
            QString fileName = tmpInfo.fileName();
            QListWidgetItem *tmp = new QListWidgetItem(icon,fileName);
            file->addItem(tmp);
        }
        else if(tmpInfo.isFile())
        {
            QIcon icon("file.png");
            QString fileName = tmpInfo.fileName();
            QListWidgetItem *tmp = new QListWidgetItem(icon,fileName);
            file->addItem(tmp);
        }
    }
}

void Normal::sendFileDir()
{
    QString str;
    QFileInfoList list = clientdir->entryInfoList(QDir::Dirs|QDir::NoDot);
    for(int i=0;i<list.count();i++)
    {
        str.append(list.at(i).fileName());
        str.append(DIRS);
    }
    str.append(SPLIT);
    list = clientdir->entryInfoList(QDir::Files|QDir::NoSymLinks);
    for(int i=0;i<list.count();i++)
    {
        str.append(list.at(i).fileName());
        str.append(FILES);
    }
    QByteArray data = str.toUtf8();
    sendData(DIR,data);
}

void Normal::beginFileserver()
{
    if(!check)
    {
        QMessageBox::information(0,"错误","还没有连接上客户端");
        return;
    }
    fserver = new TcpServer(this);
    if(!fserver->server()->listen(QHostAddress::Any,5973))
    {
        qDebug()<<"监听失败："<<mserver->server()->errorString();
        return;
    }
    connect(fserver,SIGNAL(newConnection(QTcpSocket*)), this,SLOT(FilenewConnection(QTcpSocket*)));
    cs->hide();
    gb->show();
    qDebug()<<"开启文件传输服务成功";
    filebegin = true;
}

void Normal::stopFileserver()
{
    qDebug()<<"停止文件服务 "<<fserver;
    filebegin = false;
    if(fserver == NULL)
    {
//        qDebug()<<"应该直接退出的";
        return;
    }
    //需要看文件传输完毕没有
    /*if(fserver->server()->isListening())
    {
        fserver->server()->close();
    }
    else
    {
        qDebug()<<"并没有在监听";
    }
    fsocket.clear();
    fserver = NULL;*/
    gb->hide();
    cs->show();

}

void Normal::FilenewConnection(QTcpSocket *s)
{
   TcpSocket* client = new TcpSocket(s,this);
   connect(s,SIGNAL(bytesWritten(qint64)),this,SLOT(writeData(qint64)));
   connect(client,SIGNAL(newData()),this,SLOT(newfileData()));
//   connect(client,SIGNAL(disconnectedFromserver()),this,SLOT(disconnectWithclient()));
   qDebug()<<"建立连接";
   if(s->peerAddress()==clientIp)
   {
       fsocket.append(client);
       qDebug()<<"新的连接";
       return;
   }
   QByteArray dataa("你已被关闭连接");;
   datatype data_type = QUIT;
   QByteArray data;
   QDataStream out(&data,QIODevice::WriteOnly);
   out.setVersion(QDataStream::QDataStream::Qt_5_9);
   out<<data_type;
   out<<dataa;
   QByteArray fsdata;
   fsdata.append(BEGIN);
   fsdata.append(data);
   fsdata.append(END);
   s->write(fsdata);
   s->disconnectFromHost();
   s->close();
   qDebug()<<"直接关闭连接";
}

void Normal::
sendFile(QString msg)
{
    QFileInfoList list = clientdir->entryInfoList(QDir::Files|QDir::NoSymLinks);
    foreach(QFileInfo info,list)
    {
        if(info.fileName()==msg)
        {
            qDebug()<<info.fileName();
            QString path = info.path()+"/"+info.fileName();
            cfile = new QFile(path);
            cfile->open(QFile::ReadOnly);
            totalsize = cfile->size();
            tobesend = totalsize;
            oncesize = 64*1024;
            QString size = QString::number(totalsize);
            QByteArray data = size.toUtf8();
            sendData(FILEA,data);
            time.start();
            qDebug()<<cfile->size();
            sendsize = 0;
            trans = true;
            if(!filebegin)
            {
                beginFileserver();
            }
            return;
        }
    }
    QByteArray data("你要下载的文件已不存在，请刷新目录列表！");
    sendData(FILEB,data);
}

void Normal::fsFile()
{
    if(tobesend==0)
    {
        qDebug()<<"传输完成，花费时间"<<time.elapsed()/1000<<"秒";
        trans = false;
        cfile->close();
        return;
    }
    qint64 thissend = qMin(oncesize,tobesend);
    qDebug()<<"这次发送大小："<<thissend;
    QByteArray data = cfile->read(thissend);
    qDebug()<<"实际发送大小："<<data.size();
    tobesend-=thissend;
    qDebug()<<"还需要传送"<<tobesend<<"个字节的数据";
    sendfileData(fsocket.at(0),FILEC,data);
}

void Normal::newfileData()
{
    qDebug()<<"文件服务端收到数据";
    (*fdata).append(fsocket.at(0)->socket()->readAll());
    int size = (*fdata).size()-END.size();

    int left = fdata->indexOf(BEGIN);
    int right = fdata->lastIndexOf(END);
    if(left!=0||right!=size)
    {
        qDebug()<<"数据接收不完整，还需要继续读取";
        return;
    }
    fdata->replace(0,BEGIN.size(),NULL);
    right = fdata->lastIndexOf(END);
    fdata->replace(right,END.size(),NULL);
    QDataStream in(fdata,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_9);
    int data_type;
    in>>data_type;
    datatype t = datatype(data_type);
    QByteArray data;
    in>>data;
    fdata->resize(0);
    QString msg;
    switch (t){
    case CHECK:
        break;
    case RECHECK:
        sendFileDir();
        break;
    case TEXT:
        msg = QString(data);
        recv->setText(msg);
        break;
    case FILEA:
//        msg = QString(data);
//        sendFile(msg);
        break;
    case FILEB:
        break;
    case FILEC:
        msg = QString(data);
        qDebug()<<msg;
        fsFile();
        break;
    case QUIT:
        msg = QString(data);
        QMessageBox::information(0,"结束",msg);
        msocket = NULL;
        break;
    case CD:
//        msg = QString(data);
//        clientdir->cd(msg);
//        sendFileDir();
    case REFRESH:
//        msg = QString(data);
//        qDebug()<<msg;
//        sendFileDir();
    default:
        break;
    }
}

void Normal::sendfileData(TcpSocket * client, datatype data_type, QByteArray dataa)
{
    if(msocket==NULL||!listen)
    {
        QMessageBox::warning(0,"错误","没有连接不能发送消息");
        return;
    }
    qDebug()<<"文件服务端开始发送数据"<<data_type;
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::QDataStream::Qt_5_9);
    out<<data_type;
    out<<dataa;
    QByteArray fsdata;
    fsdata.append(BEGIN);
    fsdata.append(data);
    fsdata.append(END);
    client->write(fsdata);
    qDebug()<<"文件服务端发送数据完毕";
}

void Normal::startScreen()
{
    screen = new Screen(this);
    qDebug()<<"屏幕分享已开启";
    bbutton->hide();
    sbutton->show();
}

void Normal::stopScreen()//主动停止分享屏幕
{
    sbutton->hide();
    bbutton->show();
    qDebug()<<"停止屏幕分享";
    screen->stop();
    screen = NULL;
}
