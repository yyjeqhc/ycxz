#ifndef TEACHER_H
#define TEACHER_H

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

//本意是想利用udp组播实现一个老师多个学生的效果，但是技术不足，难以测试，最后还是没写。
class Teacher : public QWidget
{
    Q_OBJECT
public:
    explicit Teacher(QWidget *parent = nullptr);

};

#endif // TEACHER_H
