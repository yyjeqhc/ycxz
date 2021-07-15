#include "dialog.h"
#include <QDebug>
//注意修改一下尺寸
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("远程协助");
    ts = new QLabel(this);
    ts->setText("请选择你的模式");
    teacher = new QRadioButton(this);
    teacher->setText("老师");
    student = new QRadioButton(this);
    student->setText("学生");
    normal = new QRadioButton(this);
    normal->setText("受助方");
    helper = new QRadioButton(this);
    helper->setText("帮助者");
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(ts,0,0,1,2);
    mainLayout->addWidget(teacher,1,0);
    mainLayout->addWidget(student,1,1);
    mainLayout->addWidget(normal,2,0);
    mainLayout->addWidget(helper,2,1);
    group = new QButtonGroup(this);
    group->addButton(teacher);
    group->addButton(student);
    group->addButton(normal);
    group->addButton(helper);
    connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(choose(QAbstractButton*)));
}

Dialog::~Dialog()
{
}

void Dialog::choose(QAbstractButton * button)
{
    qDebug()<<button->text();
    if(button==teacher)
    {
        qDebug()<<"老师";
    }
    else if(button==student)
    {
        qDebug()<<"学生";
    }
    else if(button==normal)
    {
        mnormal = new Normal();
    }
    else if(button==helper)
    {
        mhelper = new Helper();
        mhelper->show();
    }
//    this->close();
}

