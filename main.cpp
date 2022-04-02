#include "ui/dbcat.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator myappTranslator;
    if(!myappTranslator.load("./languages/lang_" + QLocale::system().name()))
    {
        myappTranslator.load("./languages/lang_zh.qm");
    }
    a.installTranslator(&myappTranslator);

    DBcat w;
    w.show();
    w.initWidgetSize();
    return a.exec();
}
