#ifndef SHOWSCREEN_H
#define SHOWSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtDebug>
#include <windows.h>
#include <iostream>
#include <fstream>

//这个是协助方用来显示受助方屏幕，并加以控制的文件
class showScreen : public QWidget
{
    Q_OBJECT
public:
    explicit showScreen(QWidget *parent = nullptr);
    static bool focus;//QLabel是否获取焦点，没在别人的屏幕上，就不需要控制
    //涉及到一些信号的处理，只好采用静态单例
    static showScreen* mw;
    static showScreen* get_instance(){
        if(mw==nullptr){
              mw = new showScreen;
        }
        return mw;
    }
public slots:
    void showPicture(QByteArray msg);//直接把字节数组当成图片数据
    //本来不用hook的话是有用的，但是hook以后就不需要转换了。
    static int translateKeycode(int key);
    //安装钩子
    HHOOK installKeyboardHook();
    //卸载钩子
    void uninstallKeyboardHook(HHOOK hHook);
    //钩子函数
    static LRESULT CALLBACK keyboardHookProc(int nCode,WPARAM wParam, LPARAM lParam);
private:
    QLabel* showPic;//展示图片的QLabel
    int width;//图片需要缩放的宽度
    int height;//图片需要缩放的高度
    const QString SPLIT = "split";
    enum controltype{
        KEYP,KEYR,MOUSEP,MOUSER,MOUSED,MOUSEW,MOUSEM,NONE,SDATA,KEYN
    };

    QFile* log;

    std::ofstream outfile;
    typedef struct
    {
        int iCode;
        int iScanCode;
        int iFlags;
        int iTime;
        int iExtraInfo;
    } HookStruct;

protected:
    void resizeEvent(QResizeEvent * event);//窗口调整事件
    bool eventFilter(QObject *watched, QEvent *event);//事件过滤器
    void keyPressEvent(QKeyEvent *event);//键盘按下
    void keyReleaseEvent(QKeyEvent *event);//键盘抬起
    void mousePressEvent(QMouseEvent * event);//鼠标按下
    void mouseReleaseEvent(QMouseEvent* event);//鼠标抬起
    void mouseDoubleClickEvent(QMouseEvent *event);//鼠标双击
    void wheelEvent(QWheelEvent *event);//鼠标滑轮滚动
    void mouseMoveEvent(QMouseEvent *event);//鼠标移动
    void closeEvent(QCloseEvent *event);
signals:
    void sendData(controltype type,QByteArray data);
    void closeScreen();
};

#endif // SHOWSCREEN_H
