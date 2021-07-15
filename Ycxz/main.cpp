#include "dialog.h"

#include <QApplication>
#include "showscreen.h"
showScreen* showScreen::mw = NULL;
bool showScreen::focus = false;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();
}
