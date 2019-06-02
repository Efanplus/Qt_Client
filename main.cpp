#include "mainwindow_client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow_Client w;
    w.show();

    return a.exec();
}
