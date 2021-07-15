#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QRadioButton>
#include <QGridLayout>
#include <QButtonGroup>
#include <QAbstractButton>
#include "teacher.h"
#include "student.h"
#include "normal.h"
#include "helper.h"

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    /*
     * 挑选模式
    */
    QRadioButton *teacher;
    QRadioButton *student;
    QRadioButton *normal;
    QRadioButton *helper;
    QLabel *ts;
    QButtonGroup *group;
    QGridLayout *mainLayout;

    /*
     * 4种模式
    */
    Teacher* mteacher;
    Student* mstudent;
    Normal* mnormal;
    Helper* mhelper;

private slots:
    void choose(QAbstractButton*);
};
#endif // DIALOG_H
