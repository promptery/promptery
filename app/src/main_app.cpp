#include "mainwindow_app.h"

#include <common/log.h>

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(ui);

    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);

    GlobalLogger::installGlobalLogger(a);

    MainWindowApp w;
    w.show();
    return a.exec();
}
