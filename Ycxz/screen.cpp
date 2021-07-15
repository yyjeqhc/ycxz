#include "screen.h"
#include <QDebug>
#include <QMessageBox>
#include <QScreen>
#include <QPixmap>
#include <QIODevice>
#include <QDateTime>
#include <QGuiApplication>
#include <QBuffer>
#include <QDesktopWidget>
#include <QApplication>
#include <QThread>

Screen::Screen(QObject *parent) : QObject(parent)
{
    width = QApplication::desktop()->width();
    height = QApplication::desktop()->height();
    mserver = new TcpServer(this);
    if(!mserver->server()->listen(QHostAddress::Any,5974))
    {
        qDebug()<<"监听失败："<<mserver->server()->errorString();
        return;
    }
    connect(mserver,SIGNAL(newConnection(QTcpSocket*)), this,SLOT(newConnection(QTcpSocket*)));
    msocket = NULL;

    qDebug()<<"开启屏幕分享服务";
    readdata = new QByteArray;

//    QLibrary mylib("key.dll");
//    if(mylib.load())
//    {
//        keybd_event = (Hello)mylib.resolve("keybd_eventa");
//        mouse_event = (World)mylib.resolve(("mouse_eventa"));
//        qDebug()<<"加载成功";
//    }
    press = false;
    //要想signal自定义类型，就需要注册
    qRegisterMetaType<QVector<sj>>("QVector<sj>");//注册diskInformation类型
    qRegisterMetaType<QVector<kb>>("QVector<kb>");//注册diskInformation类型

    count = 0;
    outfile.open("log.txt",std::ios::trunc);
}

void Screen::newConnection(QTcpSocket *s)
{
    msocket = new TcpSocket(s,this);
    connect(s,SIGNAL(bytesWritten(qint64)),this,SLOT(writeData(qint64)));
    connect(msocket,SIGNAL(newData()),this,SLOT(newData()));
    connect(msocket,SIGNAL(disconnectedFromserver()),this,SLOT(disconnectWithclient()));
    qDebug()<<"屏幕分享服务器建立新的连接";
    control* hi = new control();
    QThread* thread = new QThread();
    hi->moveToThread(thread);
    thread->start();
    connect(this,SIGNAL(begina(QVector<sj>)),hi,SLOT(begina(QVector<sj>)));
    connect(this,SIGNAL(keyboard(QVector<kb>)),hi,SLOT(keyboard(QVector<kb>)));
}

void Screen::writeData(qint64 bytenum)
{
//    qDebug()<<"屏幕分享写了"<<bytenum/1024<<"KB的数据";
}

void Screen::newData()
{
//    qDebug()<<"屏幕服务端收到数据";
    (*readdata).append(msocket->socket()->readAll());
    int size = (*readdata).size()-END.size();
    int left = readdata->indexOf(BEGIN);
    int right = readdata->lastIndexOf(END);
    if(left!=0||right!=size)
    {
//        qDebug()<<"数据接收不完整，还需要继续读取";
        return;
    }
    readdata->replace(0,BEGIN.size(),NULL);
    right = readdata->lastIndexOf(END);
    readdata->replace(right,END.size(),NULL);
    QDataStream in(readdata,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_9);
    int data_type;
    in>>data_type;
    controltype t = controltype(data_type);
    QByteArray data;
    in>>data;
    readdata->resize(0);
    QString msg = QString(data);
//    qDebug()<<"msg = "<<msg;
    QStringList list = msg.split(SPLIT);
//    qDebug()<<"长度："<<list.length();
    int key;
//    qDebug()<<"开始处理屏幕数据";
    sj s;
    kb kba;
    switch (t){
    case NONE:
        startScreen();
        qDebug()<<"开始屏幕分享";
        break;
    case KEYP:
        key = list.at(0).toInt();
        kba.code = key;
        kba.type = 1;
        va.push_back(kba);
        qDebug()<<"按钮按下 count="<<count<<" code="<<key;
        outfile<<"键盘按下 "<<key<<std::endl;
        keybd_event(key,0,0,0);
        break;
    case KEYR:
        key = list.at(0).toInt();
        kba.code = key;
        kba.type = 2;
        va.push_back(kba);
        qDebug()<<"按钮抬起 count="<<count<<" code="<<key<<" size="<<va.size();
        outfile<<"键盘抬起 "<<key<<std::endl;
        keybd_event(key,0,2,0);
        break;
        //本来键盘事件就用的KEYP和KEYR，效果不尽人意，就采取KEYN，接收几个键盘信息，再一次处理
    case KEYN:
        qDebug()<<"keyN "<<msg;
        keyEvent(msg);
        break;
    case MOUSEP:
        width = 65535*list.at(2).toInt()/list.at(0).toInt();
        height = 65535*list.at(3).toInt()/list.at(1).toInt();
        press = true;
        key = list.at(4).toInt();
        s.w = width;
        s.h = height;
        s.value = key;
        s.type = 1;
        v.push_back(s);
        qDebug()<<"鼠标按下";
        break;
    case MOUSER:
        width = 65535*list.at(2).toInt()/list.at(0).toInt();
        height = 65535*list.at(3).toInt()/list.at(1).toInt();
        key = list.at(4).toInt();
        s.w = width;
        s.h = height;
        s.value = key;
        s.type = 3;
        v.push_back(s);
        mouseRelease();
        qDebug()<<"鼠标抬起";
        break;
    case MOUSED:
        width = 65535*list.at(2).toInt()/list.at(0).toInt();
        height = 65535*list.at(3).toInt()/list.at(1).toInt();
        key = list.at(4).toInt();
        press = true;
        s.w = width;
        s.h = height;
        s.value = key;
        s.type = 2;
        v.push_back(s);
        qDebug()<<"鼠标双击";
        break;
    case MOUSEM:
        width = 65535*list.at(2).toInt()/list.at(0).toInt();
        height = 65535*list.at(3).toInt()/list.at(1).toInt();
        if(press)
        {
            s.w = width;
            s.h = height;
            s.value = 32769;
            s.type = 0;
            v.push_back(s);
        }
        else
        {
            mouse_event(32769,width,height,0,0);
        }
        break;
    case MOUSEW:
        width = 65535*list.at(2).toInt()/list.at(0).toInt();
        height = 65535*list.at(3).toInt()/list.at(1).toInt();
        key = list.at(4).toInt();
        mouse_event(32769,width,height,0,0);
        mouse_event(2048,0,0,key,0);
        qDebug()<<"鼠标滚动";
        break;
    default:
        break;
    }
}


void Screen::sendData(controltype data_type, QByteArray dataa)
{
    if(msocket==NULL)
    {
        QMessageBox::warning(0,"错误","没有连接不能发送消息");
        return;
    }
//    qDebug()<<"屏幕服务端开始发送数据"<<data_type;
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
//    qDebug()<<"屏幕服务端发送数据完毕";
}

void Screen::startScreen()
{
    mytime.start(50);//每秒2次
    connect(&mytime,SIGNAL(timeout()),this,SLOT(timeComing()));
}

void Screen::timeComing()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap p = screen->grabWindow(0);
    QByteArray array;
    QBuffer buffer(&array);
    //屏幕处理本来想用jpeg库的，但是懒，就简单的处理了。
    p.save(&buffer, "jpg",50);
    buffer.close ();
//    qDebug()<<"本次截屏大小："<<array.size();
    sendData(SDATA,array);
}

void Screen::disconnectWithclient()
{
    mytime.stop();
    if(msocket !=NULL)
    {
        msocket->close();
        msocket = NULL;
    }
    qDebug()<<"关闭屏幕分享 客户端";
}

void Screen::mouseRelease()
{
    //这是一些鼠标组合消息，比如拖放等。
    emit begina(v);
    v.resize(0);
    press = false;
}
/*qDebug()<<"开始执行 "<<QThread::currentThread();
qDebug()<<"长度："<<v.size();
sj s;
for(int i=0;i<v.size();i++)
{

    s = v[i];
//        qDebug()<<"第"<<i<<"次 "<<s.w<<" "<<s.h<<" "<<s.value<<" "<<s.type;
    mouse_event(32769,s.w,s.h,0,0);
    qDebug()<<s.value<<" value";
    switch (s.type) {
    case 0:
        qDebug()<<"type 0";
        break;
    case 1:
        qDebug()<<"type 1";
         mouse_event(s.value,0,0,0,0);
         QThread::sleep(15);
        break;
    case 2:
        qDebug()<<"type 2";
        mouse_event(s.value,0,0,0,0);
        mouse_event(s.value,0,0,0,0);
        break;
    case 3:
        qDebug()<<"type 3";
        mouse_event(s.value,0,0,0,0);
    default:
        break;
    }
    qDebug()<<"一次运行完毕";
}*/
//本来想把键盘和鼠标一样处理的，但是效果不尽人意，改成了上面的方式，这个基本没用了。
void Screen::keyboardRelease()
{
    int size = va.size();
    qDebug()<<"count = "<<count;
    qDebug()<<"size = "<<size;

    if(count == size/2)
    {
        qDebug()<<"遍历调试";
        for(int i=0;i<size;i++)
        {
            qDebug()<<va[i].code<<" "<<va[i].type;
        }
        qDebug()<<"调试结束";
        emit keyboard(va);
        va.resize(0);
        count = 0;
    }
}

void Screen::stop()
{
    qDebug()<<"关闭屏幕分享 stop";
    if(msocket!=NULL)
    {
        msocket->socket()->close();
        msocket = NULL;
    }
    if(mserver!=NULL)
    {
        mserver->close();
//        qDebug()<<"服务器也已关闭";
        mserver = NULL;
    }
    //    qDebug()<<"mserver = "<<mserver;//正在分享中关闭服务器
}
//这个就是一次性处理多个键盘消息了。
void Screen::keyEvent(QString msg)
{
    QStringList str1 = msg.split(SPLIT);
    qDebug()<<"length "<<str1.length();
    QString k;
    int key;
    for(int i=0;i<str1.length()-1;i++)
    {
        QStringList str2 = str1.at(i).split(" ");
        qDebug()<<str2.length();
        key = str2.at(0).toInt();
        qDebug()<<"key = "<<key;
        k = str2.at(1);
        qDebug()<<"k = "<<k;
        if(k=="a")
        {
            keybd_event(key,0,0,0);
        }
        else if(k=="b")
        {
            keybd_event(key,0,2,0);
        }
    }
    qDebug()<<"循环结束 ";
}

