#include "control.h"
#include <QDebug>
#include <QThread>

control::control(QObject *parent) : QObject(parent)
{

}

void control::begina(QVector<control::sj> v)
{
    sj s;
    bool ys = false;
    int l = v.size();
    if(v[l-2].type==0)
    {
        ys = true;
    }
    for(int i=0;i<l;i++)
    {

        s = v[i];
        mouse_event(32769,s.w,s.h,0,0);
        switch (s.type) {
        case 0:
            break;
        case 1:
             mouse_event(s.value,0,0,0,0);
             if(ys)
             {
                QThread::usleep(10000);
             }
            break;
        case 2:
            mouse_event(6,0,0,0,0);
            mouse_event(2,0,0,0,0);
            break;
        case 3:
            if(ys)
            {
                QThread::usleep(10000);
            }
            mouse_event(s.value,0,0,0,0);
        default:
            break;
        }
    }
}

void control::keyboard(QVector<control::kb> v)
{
    qDebug()<<"线程正在处理键盘事件";
    int size = v.size();
    qDebug()<<"长度 "<<size;
    kb s;
    for(int i=0;i<size;i++)
    {
        s = v[i];
        qDebug()<<s.code;
        if(s.type==1)
        {
            keybd_event(s.code,0,0,0);
        }
        else if(s.type==2)
        {
            keybd_event(s.code,0,2,0);
        }
    }
    mouse_event(2048,0,0,120,0);
    mouse_event(2048,0,0,-120,0);
}
