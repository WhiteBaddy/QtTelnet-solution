#include "mainwindow.h"

#include <QApplication>

//#include <QTextDocument>
//#include <QTextBrowser>

//#include <QTextLayout>
//#include <QPainter>
//#include <QTextOption>
//#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTextBrowser textBrowser;
//    QTextDocument textDocument;

//    QString text = "\x1B[31mHello, World!\x1B[0m";
//    qDebug() << text.length();
//    textDocument.setPlainText(text);
//    textBrowser.setDocument(&textDocument);
//    textBrowser.show();

    MainWindow w;
    w.show();
    return a.exec();
}
