#ifndef STUDENT_H
#define STUDENT_H

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

//本意是想利用udp组播实现一个老师多个学生的效果，但是技术不足，难以测试，最后还是没写。
class Student : public QWidget
{
    Q_OBJECT
public:
    explicit Student(QWidget *parent = nullptr);

};

#endif // STUDENT_H
