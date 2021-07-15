#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <windows.h>
#include <winuser.h>
#include <QVector>

class control : public QObject
{
    Q_OBJECT
public:
    explicit control(QObject *parent = nullptr);

private:
    struct sj{
        int w,h,value,type;
    };
    struct kb{
        int code,type;
    };

public slots:
    void begina(QVector<sj> v);
    void keyboard(QVector<kb> v);

signals:

};

#endif // CONTROL_H
