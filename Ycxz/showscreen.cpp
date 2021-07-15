#include "showscreen.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>


static HWND hWnd = NULL;
// 键盘钩子句柄
static HHOOK gHHook = NULL;

//考虑到有时候组合键或者按键太快，所以还是决定等键盘消息数量比较多再一次性传递。
static int count1 = 0;
static int count2 = 0;
static QByteArray* totaldata = NULL;
static const QString SPLITA = "split";

showScreen::showScreen(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("屏幕观看");
    this->setWindowFlags(Qt::Widget|Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    const int w = 800, h = 450;
    const int x = (QApplication::desktop()->width() - w) >> 1;
    const int y = (QApplication::desktop()->height() - h) >> 1;
    this->setGeometry(x, y, w, h);
    this->setMouseTracking(true);

    // 图片label
    showPic = new QLabel(this);
    showPic->setGeometry(0,0, w, h);
    showPic->setScaledContents(true);
    showPic->installEventFilter(this);
    height = showPic->height();
    width = showPic->width();
    showPic->setMouseTracking(true);

    gHHook = installKeyboardHook();
    if (!gHHook) {
        qDebug()<<"安装钩子失败";
    } else {
        qDebug()<<"安装钩子成功";
    }

//    log = new QFile("log.txt");
//    log->open(QIODevice::WriteOnly);
    outfile.open("log.txt",std::ios::trunc);

    totaldata = new QByteArray;
}

void showScreen::showPicture(QByteArray msg)
{
    QPixmap p;
    p.loadFromData(msg);
    p = p.scaled(width,height);
    showPic->setPixmap(p);
}
//实际上已经没用了。
int showScreen::translateKeycode(int key)
{
    int k = key;
        bool legal = true;
        if(k >= Qt::Key_0 && k <= Qt::Key_9)
        {
        }
        else if(k >= Qt::Key_A && k <= Qt::Key_Z)
        {
        }
        else if(k >= Qt::Key_F1 && k <= Qt::Key_F24)
        {
            k &= 0x000000ff;
            k += 0x40;
        }
        else if(k==Qt::Key_Apostrophe)
        {
            k += 0xb7;
        }
        else if(k==Qt::Key_Asterisk)
        {
            k +=0x40;
        }
        else if(k==Qt::Key_Plus)
        {
            k = 107;
        }
        else if(k>=Qt::Key_Comma &&k<=Qt::Key_Slash)
        {
            k +=0x90;
        }
        else if(k == Qt::Key_Tab)
        {
            k = 0x09;
        }
        else if(k == Qt::Key_Backspace)
        {
            k = 0x08;
        }
        else if(k == Qt::Key_Return)
        {
            k = 0x0d;
        }
        else if(k == Qt::Key_Semicolon)
        {
            k = 0xba;
        }
        else if(k>=Qt::Key_BracketLeft&&k<=Qt::Key_BracketRight)
        {
            k +=0x80;
        }
        else if(k==Qt::Key_QuoteLeft)
        {
            k +=0x60;
        }
        else if(key ==Qt::Key_Escape)
        {
            k = 0x1b;
        }
        else if(k==Qt::Key_Clear)
        {
            k = 0x0c;
        }
        else if(k==Qt::Key_Enter)
        {
            k = 0x0d;
        }
        else if(k==Qt::Key_Equal)
        {
            k = 187;
        }
        else if(k <= Qt::Key_Down && k >= Qt::Key_Left)
        {
            int off = k - Qt::Key_Left;
            k = 0x25 + off;
        }
        else if(k == Qt::Key_Shift)
        {
            k = 0x10;
        }
        else if(k == Qt::Key_Control)
        {
            k = 0x11;
        }
        else if(k == Qt::Key_Alt)
        {
            k = 0x12;
        }
        else if(k == Qt::Key_Meta)
        {
            k = 0x5b;
        }
        else if(k == Qt::Key_Insert)
        {
            k = 0x2d;
        }
        else if(k == Qt::Key_Delete)
        {
            k = 0x2e;
        }
        else if(k == Qt::Key_Home)
        {
            k = 0x24;
        }
        else if(k == Qt::Key_End)
        {
            k = 0x23;
        }
        else if(k == Qt::Key_PageUp)
        {
            k = 0x21;
        }
        else if(k == Qt::Key_PageDown)
        {
            k = 0x22;
        }
        else if(k == Qt::Key_CapsLock)
        {
            k = 0x14;
        }
        else if(k == Qt::Key_NumLock)
        {
            k = 0x90;
        }
        else if(k == Qt::Key_Space)
        {
            k = 0x20;
        }
        else
            legal = false;

        if(!legal)
            return 0;
        return k;
}

HHOOK showScreen::installKeyboardHook()
{
    return SetWindowsHookExA(13,keyboardHookProc, GetModuleHandleA(NULL), 0);
}

void showScreen::uninstallKeyboardHook(HHOOK hHook)
{
    UnhookWindowsHookEx(hHook);
}

LRESULT showScreen::keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(!focus)
    {
        qDebug()<<"没有获取焦点，不用处理";
        return CallNextHookEx(gHHook,nCode,wParam,lParam);
    }
    //键盘按下，就添加进消息里面
    if (wParam == WM_KEYDOWN) {
            HookStruct *hs = (HookStruct *)lParam;
            int code = hs->iCode;
            qDebug()<<"键盘按下 "<<code; 
            count1++;
            totaldata->append(QString::number(code).toUtf8());
//            QByteArray data = QString::number(code).toUtf8();
//            emit showScreen::mw->sendData(KEYP,data);
            totaldata->append(' ');
            totaldata->append('a');
            totaldata->append(SPLITA);
            showScreen::mw->outfile<<"键盘按下 "<<code<<std::endl;
            return 1;
    } else if(wParam == WM_KEYUP) {
        //键盘抬起，并且键值对应，就传送控制消息
        HookStruct *hs = (HookStruct *)lParam;
        int code = hs->iCode;
        qDebug()<<"键盘抬起 "<<code;
        count2++;
        totaldata->append(QString::number(code).toUtf8());
        totaldata->append(' ');
        totaldata->append('b');
        totaldata->append(SPLITA);
        qDebug()<<"total = "<<*totaldata;
        if(count1==count2)
        {
            count1=0;
            count2=0;
            emit showScreen::mw->sendData(KEYN,*totaldata);
            totaldata->resize(0);
        }

//        QByteArray data = QString::number(code).toUtf8();
//        emit showScreen::mw->sendData(KEYR,data);
        showScreen::mw->outfile<<"键盘抬起 "<<code<<std::endl;
    }
    return 1;
}

void showScreen::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    showPic->setGeometry(0,0,size.width(),size.height());
    height = showPic->height();
    width = showPic->width();
}
//用来判断是否控制
bool showScreen::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==(QObject*)showPic) {
        switch (event->type()) {
        case QEvent::Enter:
            qDebug()<<"进入label范围";
            focus = true;
            break;
        case QEvent::Leave:
            qDebug()<<"离开QLabel范围";
            focus = false;
            break;
        default:
            break;
        }
    }
    return QObject::eventFilter(watched, event);
}

void showScreen::keyPressEvent(QKeyEvent *event)
{
    /*if(!focus)
    {
        return;
    }
    int key = translateKeycode(event->key());
    QByteArray data = QString::number(key).toUtf8();
    emit sendData(KEYP,data);
    qDebug()<<"键盘按下 "<<event->key();*/
}

void showScreen::keyReleaseEvent(QKeyEvent *event)
{
    /*if(!focus)
    {
        return;
    }
    int key = translateKeycode(event->key());
    QByteArray data = QString::number(key).toUtf8();
    emit sendData(KEYR,data);
    qDebug()<<"键盘抬起 "<<event->key();*/
}
//鼠标按下事件
void showScreen::mousePressEvent(QMouseEvent *event)
{
    if(!focus)
    {
        return;
    }

    QString msg = "";
    msg.append(QString::number(width));
    msg.append(SPLIT);
    msg.append(QString::number(height));
    msg.append(SPLIT);
    msg.append(QString::number(event->x()));
    msg.append(SPLIT);
    msg.append(QString::number(event->y()));
    msg.append(SPLIT);
    int button = event->button();
    if(button == 1)
    {
        msg.append(QString::number(2));
    } else if(button == 2) {
        msg.append(QString::number(8));
    } else if(button == 4) {
        msg.append(QString::number(32));
    }
    QByteArray data = msg.toUtf8();
    emit sendData(MOUSEP,data);
//    qDebug()<<"鼠标按下事件"<<event->x()<<" "<<event->y()<<" "<<event->button()<<" "<<event->globalX()<<" "<<event->globalY();
}
//鼠标抬起
void showScreen::mouseReleaseEvent(QMouseEvent *event)
{
    if(!focus)
    {
        return;
    }
    QString msg = "";
    msg.append(QString::number(width));
    msg.append(SPLIT);
    msg.append(QString::number(height));
    msg.append(SPLIT);
    msg.append(QString::number(event->x()));
    msg.append(SPLIT);
    msg.append(QString::number(event->y()));
    msg.append(SPLIT);
    int button = event->button();
    if(button == 1)
    {
        msg.append(QString::number(4));
    } else if(button == 2) {
        msg.append(QString::number(16));
    } else if(button == 4) {
        msg.append(QString::number(64));
    }
    QByteArray data = msg.toUtf8();
    emit sendData(MOUSER,data);
//    qDebug()<<"鼠标抬起事件"<<event->x()<<" "<<event->y()<<" "<<event->button()<<" "<<event->globalX()<<" "<<event->globalY();
}
//鼠标双击
void showScreen::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!focus)
    {
        return;
    }
    QString msg = "";
    msg.append(QString::number(width));
    msg.append(SPLIT);
    msg.append(QString::number(height));
    msg.append(SPLIT);
    msg.append(QString::number(event->x()));
    msg.append(SPLIT);
    msg.append(QString::number(event->y()));
    msg.append(SPLIT);
    int button = event->button();
    if(button == 1)
    {
        msg.append(QString::number(2));
    } else if(button == 2) {
        msg.append(QString::number(8));
    } else if(button == 4) {
        msg.append(QString::number(32));
    }
    QByteArray data = msg.toUtf8();
    emit sendData(MOUSED,data);
//    qDebug()<<"双击事件"<<event->x()<<" "<<event->y()<<" "<<event->button()<<" "<<event->globalX()<<" "<<event->globalY();
}
//滑轮事件
void showScreen::wheelEvent(QWheelEvent *event)
{
    if(!focus)
    {
        return;
    }
    QString msg = "";
    msg.append(QString::number(width));
    msg.append(SPLIT);
    msg.append(QString::number(height));
    msg.append(SPLIT);
    msg.append(QString::number(event->x()));
    msg.append(SPLIT);
    msg.append(QString::number(event->y()));
    msg.append(SPLIT);
    msg.append(QString::number(event->delta()));
    QByteArray data = msg.toUtf8();
    emit sendData(MOUSEW,data);
//    qDebug()<<event->delta();
}
//鼠标移动
void showScreen::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug()<<"move event";
    if(!focus)
    {
        return;
    }
    QString msg = "";
    msg.append(QString::number(width));
    msg.append(SPLIT);
    msg.append(QString::number(height));
    msg.append(SPLIT);
    msg.append(QString::number(event->x()));
    msg.append(SPLIT);
    msg.append(QString::number(event->y()));
    QByteArray data = msg.toUtf8();
    emit sendData(MOUSEM,data);
//    qDebug()<<event->x()<<" "<<event->y();
}

void showScreen::closeEvent(QCloseEvent *event)
{
    emit closeScreen();
}
