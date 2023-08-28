#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qttelnet.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private slots:
    void telnetLoginRequired();
    void telnetLoginFailed();
    void telnetLoginIn();
    void telnetLoginOut();
    void telnetMessage(const QString& data);
    void telnetConnectionError(QAbstractSocket::SocketError error);

    void onPushButtonConnectClicked();
    void onPushButtonDisconnectClicked();

    void onPushButtonSendClicked();

private:
    Ui::MainWindow *ui;
    QtTelnet* m_telnet;
};
#endif // MAINWINDOW_H
