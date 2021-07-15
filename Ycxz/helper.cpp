#include "helper.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDataStream>
#include <QInputDialog>
#include <QRegExp>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QTime>
#include <QRect>

Helper::Helper(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("协助方");
    this->setWindowFlags(Qt::Widget|Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    // 设置窗口大小
    const int w = 600,  h = 500;
    // 将窗口至中，你必须在widget.h里#include <QDesktopWidget>
    // 以后我就不再多说了，没有include的类就自己在.h文件里include
    const int x = (QApplication::desktop()->width() - w)/2;
    const int y = (QApplication::desktop()->height() - h)/2;
    this->setGeometry(x, y, w, h);
    //不允许最大化最小化？
    this->setMaximumSize(QSize(w, h));
    this->setMinimumSize(QSize(w, h));
//    this->show();

    lj = new QPushButton(this);
    lj->setText("连接");
    dk = new QPushButton(this);
    dk->setText("断开");
    fs = new QPushButton(this);
    fs->setText("发送");
    connect(lj,SIGNAL(clicked(bool)),this,SLOT(connectToserver()));
    connect(dk,SIGNAL(clicked(bool)),this,SLOT(disconnectWithserver()));
    connect(fs,SIGNAL(clicked(bool)),this,SLOT(sendMsg()));

    pm = new QPushButton(this);
    pm->setText("观看屏幕");
    connect(pm,SIGNAL(clicked(bool)),this,SLOT(startScreen()));
//    connect(pm,SIGNAL(clicked(bool)),this,SLOT(startScreen()));

    label_ip = new QLabel(this);
    label_ip->setText("服务器ip：");
    label_ip->hide();

    ip = new QLineEdit(this);
    ip->setText("10.194.199.34");
    label_send = new QLabel(this);
    label_send->setText("发送信息：");
//    label_send->hide();
    send = new QLineEdit(this);
    connect(send,SIGNAL(returnPressed()),this,SLOT(sendMsg()));

    label_recv = new QLabel(this);
    label_recv->setText("接收信息：");
//    label_recv->hide();
    recv = new QLineEdit(this);

    label_s = new QLabel();
    label_s->setGeometry(0,0,800,700);

    file = new QListWidget(this);
    file->installEventFilter(this);
    connect(file,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(slotDirShow(QListWidgetItem*)));

    menu = new QMenu(this);
    QAction *serverRefresh = menu->addAction("刷新");
    connect(serverRefresh,SIGNAL(triggered(bool)), this, SLOT(refreshServerDir()));
    QAction *serverDownload= menu->addAction("下载（只能对文件进行操作）");
    connect(serverDownload,SIGNAL(triggered(bool)), this,SLOT(downloadServerfile()));

    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(pm,0,0);
//    mainLayout->addWidget(label_ip,0,0);
    mainLayout->addWidget(ip,0,1);
    mainLayout->addWidget(label_send,1,0);
    mainLayout->addWidget(send,1,1);
    mainLayout->addWidget(label_recv,2,0);
    mainLayout->addWidget(recv,2,1);
    mainLayout->addWidget(lj,3,0);
    mainLayout->addWidget(dk,3,0);
    mainLayout->addWidget(fs,3,1);
    mainLayout->addWidget(file,4,0,2,2);
//    mainLayout->addWidget(label_s,6,0,4,4);
    dk->hide();
//    label_s->setGeometry(0,0,700,700);
//    label_s->setText("hello world");
    /*pm->setGeometry(700,0,800,100);
    ip->setGeometry(700,100,800,150);
    send->setGeometry(700,150,800,200);
    recv->setGeometry(700,200,800,250);
    lj->setGeometry(700,250,800,300);
    dk->setGeometry(700,350,800,400);
    fs->setGeometry(700,450,800,500);
    file->setGeometry(700,500,800,550);*/
//    QGridLayout* total = new QGridLayout(this);
//    QGridLayout* left = new QGridLayout();
//    left->addWidget(label_s);
//    label_s->setText("hello world");
//    total->addLayout(left,0,0,14,7);
//    total->addLayout(mainLayout,0,7,1,1);

    lianjie = false;
    msocket = NULL;
    readdata = new QByteArray;
    fdata = new QByteArray;
    sdata = new QByteArray;
    serverIp = new QHostAddress();

//    screena = new class showScreen();

}

void Helper::connectToserver()
{
    if(!serverIp->setAddress(ip->text()))
    {
        QMessageBox::warning(0,"警告","ip地址不合法");
        return;
    }
    ip->setReadOnly(true);
    QTcpSocket* socket = new QTcpSocket(this);
//    QHostAddress* serverIp = new QHostAddress("127.0.0.1");
    socket->connectToHost(*serverIp,5972);
    connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(writeData(qint64)));
    lj->hide();
    dk->show();
    QTime mytime;
    mytime.start();
    bool connected = socket->waitForConnected(1500);
    if(!connected)
    {
        disconnectWithserver();
        qDebug()<<mytime.elapsed();
        QMessageBox::warning(0,"错误","主机不在线");
        return;
    }
    msocket = new TcpSocket(socket,this);
    connect(msocket,SIGNAL(newData()),this,SLOT(newData()));
    connect(msocket,SIGNAL(disconnectedFromserver()),this,SLOT(disconnectByserver()));
    lianjie = true;
}

void Helper::disconnectWithserver()//使用按钮断开连接
{
    qDebug()<<"主动断开连接";
    if(msocket!=NULL)
    {
        QByteArray data("客户端主动断开连接");
        sendData(QUIT,data);
        msocket->close();
        msocket = NULL;
    }
    dk->hide();
    lj->show();
    check = false;
    lianjie = false;
}

void Helper::sendMsg()
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

void Helper::newData()
{
    qDebug()<<"客户端收到数据";
    (*readdata).append(msocket->socket()->readAll());
    int size = (*readdata).size()-END.size();
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
        qDebug()<<"返回的信息 "<<msg;
        switch (t){
        case CHECK:
            checkLogin(msg);
            break;
        case RECHECK:
            check = true;
            qDebug()<<"验证通过";
            sendData(RECHECK,QByteArray("获取路径"));
            break;
        case ERRORA:
             QMessageBox::warning(0,"错误",msg);
             checkLogin(msg);
            break;
        case QUIT:
            QMessageBox::warning(0,"错误",msg);
        default:
            break;
        }
        return;
    }
    switch (t){
    case CHECK:
        break;
    case RECHECK:
        break;
    case TEXT:
       msg = QString(data);
        recv->setText(msg);
        break;
    case FILEA:
        msg = QString(data);
        qDebug()<<"文件大小"<<msg;
        totalsize = msg.toInt();
        recvsize = 0;
        saveFile();
        break;
    case FILEB:
        filename = "";//文件不存在
        msg = QString(data);
        QMessageBox::information(0,"提示",msg);
        break;
    case FILEC:
        recvfile->write(data);
        recvsize+=data.size();
        qDebug()<<"这次接收大小："<<data.size();
        qDebug()<<data.size();
        qDebug()<<recvsize;
        if(recvsize==totalsize)
        {
            recvfile->close();
            QByteArray data("接收完毕");
            sendData(FILEC,data);
        }
        else
        {
            QByteArray data("还没有接收完毕");
            sendData(FILEC,data);
        }
        break;
    case DIR:
//        qDebug()<<"获得路径"<<data.size();
        msg = QString(data);
        showFileDir(msg);
        break;
    case QUIT:
        msg = QString(data);
        QMessageBox::information(0,"结束",msg);
        qDebug()<<"只是关闭服务而已 "<<msocket<<" "<<msocket->socket();
        msocket->close();
        break;
    case SCREENA:
        msg = QString(data);
        qDebug()<<"客户端 screena:"<<msg;
        screena = showScreen::get_instance();
        screena->show();
        connect(screena,SIGNAL(sendData(controltype,QByteArray)),this,SLOT(sendScreenData(controltype,QByteArray)));
        connect(screena,SIGNAL(closeScreen()),this,SLOT(closeScreen()));
        connectToScreen();
        break;
    case SCREENB:
        QMessageBox::information(0,"提示","对方没有开启屏幕分享");
        break;
    default:
        break;
    }
}
void Helper::writeData(qint64 bytesnum)
{
//    qDebug()<<"客户端写了"<<bytesnum<<"个字节的数据";
}

void Helper::checkLogin(QString msg)
{
    bool ok;
    QString key = QInputDialog::getText(this, "检测用户身份",msg, QLineEdit::Normal,0, &ok);
    QRegExp regExp("[A-Za-z0-9]{6,10}");
    QRegExpValidator *valitora= new QRegExpValidator(regExp,this);
    int pos=0;
    qDebug()<<"key = "<<key;
    QValidator::State st=valitora->validate(key,pos);
    qDebug()<<st;
    qDebug()<<ok;
    if(ok && !key.isEmpty())
    {
        if(QValidator::Acceptable!=st)
        {
            QMessageBox::warning(0,"密码输入错误","密码输入不符合规范");
            checkLogin(msg);
            return;
        }
        QByteArray data = key.toUtf8();
        sendData(CHECK,data);
    }
    else
    {
        QByteArray data("客户端取消了密码验证");
        sendData(QUIT,data);
        msocket->close();
    }
}

void Helper::sendData(datatype data_type, QByteArray dataa)
{
    if(msocket==NULL)
    {
        QMessageBox::warning(0,"错误","没有连接不能发送消息");
        return;
    }
    qDebug()<<"客户端开始发送数据"<<data_type;
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
    qDebug()<<"客户端数据发送完毕";
}

void Helper::disconnectByserver()
{
    ip->setReadOnly(false);
    qDebug()<<"客户端与服务器断开连接";
    dk->hide();
    lj->show();
    check = false;
    lianjie = false;
    file->clear();
    qDebug()<<"目录个数"<<file->count();
    if(msocket!=NULL)
    {
        if(msocket->socket()!=NULL)
        {
            msocket->close();
        }
        msocket = NULL;
    }
}

void Helper::showFileDir(QString msg)
{
    file->clear();
//    qDebug()<<"正在更新路径："<<msg;
    QStringList list = msg.split(SPLIT);
    if(list.count()!=2)
    {
        qDebug()<<"未知的错误";
        return;
    }
    QString dirs = list.at(0);
    QStringList dira;
    if(dirs!="")
    {
        dira = dirs.split(DIRS);
        foreach(QString name,dira)
        {
            if(name=="")
                break;
            QIcon icon("dir.png");
            QListWidgetItem *tmp = new QListWidgetItem(icon,name);
            tmp->setWhatsThis(DIRS);
            file->addItem(tmp);
        }
    }
    QString files = list.at(1);
    QStringList filea;
    if(files!="")
    {
        filea = files.split(FILES);
        foreach(QString name,filea)
        {
            if(name=="")
                break;
            QIcon icon("file.png");
            QListWidgetItem *tmp = new QListWidgetItem(icon,name);
            tmp->setWhatsThis(FILES);
            file->addItem(tmp);
        }
    }
    if(file->count()>0)
    {
        file->item(0)->setSelected(true);
    }
}

void Helper::slotDirShow(QListWidgetItem * item)
{
    qDebug()<<"进行切换路径";
    if(item->whatsThis()!=DIRS)
    {
        return;
    }
    qDebug()<<"客户端切换目录->"<<item->text();
    QByteArray data = item->text().toUtf8();
    sendData(CD,data);
}

void Helper::refreshServerDir()
{
    if(file->count()<1)
    {
        QMessageBox::information(0,"提示","当前列表框内并没有可操作的文件哦！");
        return;
    }
    qDebug()<<"点击了刷新按钮";
    QByteArray data("刷新当前客户端展示目录");
    sendData(REFRESH,data);
}

void Helper::downloadServerfile()
{
    if(file->count()<1)
    {
        QMessageBox::information(0,"提示","当前列表框内并没有可操作的文件哦！");
        return;
    }
    qDebug()<<"点击了下载按钮";
    QListWidgetItem* item = file->currentItem();
    qDebug()<<item->whatsThis();
    if(item->whatsThis()==DIRS)
    {
        QMessageBox::information(0,"提示","只能下载文件哦！");
        return;
    }
    filename = item->text();
    QByteArray data = filename.toUtf8();
    sendData(FILEA,data);
}

void Helper::saveFile()
{
    recvfile = new QFile(filename);
    recvfile->open(QFile::WriteOnly);
    QByteArray data("已准备好接收文件！");
    if(!fconnect)
    {
        QTcpSocket* socket = new QTcpSocket(this);
//        QHostAddress* serverIp = new QHostAddress("127.0.0.1");
        socket->connectToHost(*serverIp,5973);
        connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(writeData(qint64)));
        QTime mytime;
        mytime.start();
        bool connected = socket->waitForConnected(1500);
        if(!connected)
        {
            qDebug()<<mytime.elapsed();
            QMessageBox::warning(0,"错误","主机不在线");
            return;
        }
        fsocket = new TcpSocket(socket,this);
        connect(fsocket,SIGNAL(newData()),this,SLOT(newfileData()));
        connect(fsocket,SIGNAL(disconnectedFromserver()),this,SLOT(disconnectByfileserver()));
        fconnect = true;
    }
    sendfileData(FILEC,data);
}

void Helper::newfileData()
{
    qDebug()<<"文件客户端收到数据";
    (*fdata).append(fsocket->socket()->readAll());
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
        break;
    case TEXT:
       msg = QString(data);
        recv->setText(msg);
        break;
    case FILEA:
//        msg = QString(data);
//        qDebug()<<"文件大小"<<msg;
//        totalsize = msg.toInt();
//        recvsize = 0;
//        saveFile();
        break;
    case FILEB:
//        filename = "";//文件不存在
//        msg = QString(data);
//        QMessageBox::information(0,"提示",msg);
        break;
    case FILEC:
        recvfile->write(data);
        recvsize+=data.size();
        qDebug()<<"这次接收大小："<<data.size();
        qDebug()<<data.size();
        qDebug()<<recvsize;
        if(recvsize==totalsize)
        {
            recvfile->close();
            QByteArray data("接收完毕");
            sendfileData(FILEC,data);
            QMessageBox::information(0,"提示","文件下载完毕！");
        }
        else
        {
            QByteArray data("还没有接收完毕");
            sendfileData(FILEC,data);
        }
        break;
    case DIR:
//        qDebug()<<"获得路径"<<data.size();
        msg = QString(data);
        showFileDir(msg);
        break;
    case QUIT:
        msg = QString(data);
        QMessageBox::information(0,"结束",msg);
        break;
    default:
        break;
    }
}


void Helper::writefileData(qint64)
{

}

void Helper::sendfileData(datatype data_type, QByteArray dataa)
{
    if(fsocket==NULL)
    {
        QMessageBox::warning(0,"错误","没有连接不能发送消息");
        return;
    }
    qDebug()<<"客户端开始发送数据"<<data_type;
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::QDataStream::Qt_5_9);
    out<<data_type;
    out<<dataa;
    QByteArray fsdata;
    fsdata.append(BEGIN);
    fsdata.append(data);
    fsdata.append(END);
    fsocket->write(fsdata);
    qDebug()<<"客户端文件数据发送完毕";
}

void Helper::disconnectByfileserver()
{
    qDebug()<<"客户端与文件服务器断开连接";
}

void Helper::startScreen()
{
    QByteArray data("开始屏幕");
    sendData(SCREENA,data); 
}

void Helper::connectToScreen()
{
    QTcpSocket* socket = new QTcpSocket(this);
    socket->connectToHost(*serverIp,5974);
    connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(writeData(qint64)));
    QTime mytime;
    mytime.start();
    bool connected = socket->waitForConnected(1500);
    if(!connected)
    {
        qDebug()<<mytime.elapsed();
        QMessageBox::warning(0,"错误","主机不在线");
        return;
    }
    ssocket = new TcpSocket(socket,this);
    connect(ssocket,SIGNAL(newData()),this,SLOT(newScreenData()));
    QByteArray data("开始屏幕分享吧！");
    sendScreenData(NONE,data);
    connect(ssocket,SIGNAL(disconnectedFromserver()),this,SLOT(disconnectByScreenserver()));

}

void Helper::newScreenData()
{
    (*sdata).append(ssocket->socket()->readAll());
    int size = (*sdata).size()-END.size();
    int left = sdata->indexOf(BEGIN);
    int right = sdata->lastIndexOf(END);
    if(left!=0||right!=size)
    {
//        qDebug()<<"数据接收不完整，还需要继续读取";
        return;
    }
    sdata->replace(0,BEGIN.size(),NULL);
    right = sdata->lastIndexOf(END);
    sdata->replace(right,END.size(),NULL);
    QDataStream in(sdata,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_9);
    int data_type;
    in>>data_type;
    controltype t = controltype(data_type);
    QByteArray data;
    in>>data;
    sdata->resize(0);
    QString msg;
    switch (t){
     case SDATA:
        showScreen(data);
        break;
    default:
        break;
    }
}

void Helper::showScreen(QByteArray msg)
{
//    QPixmap p;
//    p.loadFromData(msg);
//    qDebug()<<"图片大小："<<p.height()<<" "<<p.width();
//    width = p.width();
//    height = p.height();
////    p = p.scaled(width,height);
//    qDebug()<<p.size();
//    label_s->setGeometry(0,0,width,height);
//    label_s->setPixmap(p);
//    label_s->show();
    screena->showPicture(msg);
}

void Helper::sendScreenData(controltype data_type, QByteArray dataa)
{
    if(ssocket==NULL)
    {
        QMessageBox::warning(0,"错误","没有连接不能发送消息");
        return;
    }
//    qDebug()<<"客户端开始发送数据"<<data_type;
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::QDataStream::Qt_5_9);
    out<<data_type;
    out<<dataa;
    QByteArray fsdata;
    fsdata.append(BEGIN);
    fsdata.append(data);
    fsdata.append(END);
    ssocket->write(fsdata);
    //    qDebug()<<"客户端屏幕数据发送完毕";
}

void Helper::disconnectByScreenserver()
{
    if(screena != NULL)
    {
        QMessageBox::information(0,"提示","对方已关闭屏幕分享");
        screena->close();
    }
    if(ssocket!=NULL)
    {
        if(ssocket->socket()!=NULL)
        {
            ssocket->socket()->close();
        }
        ssocket = NULL;
    }
}

void Helper::closeScreen()
{
    screena = NULL;
    qDebug()<<"客户端关闭屏幕画面";
    if(ssocket!=NULL)
    {
        if(ssocket->socket()!=NULL)
        {
            ssocket->socket()->close();
        }
        ssocket = NULL;
    }


}


bool Helper::eventFilter(QObject *watched, QEvent *event)
{
    // 右键弹出菜单
    if (watched == (QObject*)file) {
        if (event->type() == QEvent::ContextMenu) {
            menu->exec(QCursor::pos());
        }
    }
    return QObject::eventFilter(watched, event);
}

void Helper::closeEvent(QCloseEvent *event)
{
    if(screena!=NULL)//如果处于屏幕分享状态，就关闭屏幕
    {
        screena->close();
    }
    //如果文件正在传输，暂时不处理
    if(msocket!=NULL)//如果有连接，就关闭连接
    {
        msocket->close();
        msocket = NULL;
    }
    qDebug()<<"helper 客户端已关闭";
}
