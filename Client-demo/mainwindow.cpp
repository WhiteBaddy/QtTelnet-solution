#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    m_telnet = new QtTelnet(this);

    connect(m_telnet, &QtTelnet::loginRequired,
            this, &MainWindow::telnetLoginRequired);
    connect(m_telnet, &QtTelnet::loginFailed,
            this, &MainWindow::telnetLoginFailed);
    connect(m_telnet, &QtTelnet::loggedIn,
            this, &MainWindow::telnetLoginIn);
    connect(m_telnet, &QtTelnet::loggedOut,
            this, &MainWindow::telnetLoginOut);
    connect(m_telnet, &QtTelnet::message,
            this, &MainWindow::telnetMessage);
    connect(m_telnet, &QtTelnet::connectionError,
            this, &MainWindow::telnetConnectionError);


    connect(ui->pushButtonConnect, &QPushButton::clicked,
            this, &MainWindow::onPushButtonConnectClicked);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked,
            this, &MainWindow::onPushButtonDisconnectClicked);
    connect(ui->pushButtonSend, &QPushButton::clicked,
            this, &MainWindow::onPushButtonSendClicked);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_telnet;
}

void MainWindow::telnetLoginRequired()
{
    QString user = ui->lineEditUserName->text();
    QString passwd  = ui->lineEditPassword->text();
    qDebug() << "用户名是 :" << user;
    qDebug() << "密码是 :" << passwd;

    if(user == "" || passwd == "")
    {
        ui->textEdit->append("用户名和密码不能为空!");
        return;
    }
    user += "\r\n";
    passwd += "\r\n";
    m_telnet->login(user, passwd); // 登入telnet

}

void MainWindow::telnetLoginFailed()
{
    ui->textEdit->append("登陆失败!");
}

void MainWindow::telnetLoginIn()
{
    ui->textEdit->append("登陆成功!");
}

void MainWindow::telnetLoginOut()
{
    ui->textEdit->append("登出成功!");
}

void MainWindow::telnetMessage(const QString& data)
{
//    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->insertPlainText(data);
}

void MainWindow::telnetConnectionError(QAbstractSocket::SocketError error)
{
    ui->textEdit->append(QString("连接错误: %1").arg(error));
}


void MainWindow::onPushButtonConnectClicked()
{
    qDebug() << "连接键被按下了";
    QString host = ui->lineEditIP->text();
    ui->textEdit->append(host);
    if(host == "")
    {
        ui->textEdit->append("IP地址不能为空!");
        return ;
    }
    m_telnet->connectToHost(host, 23);
}


void MainWindow::onPushButtonDisconnectClicked()
{
    qDebug() << "断开键被按下了";
    m_telnet->close();
}


void MainWindow::onPushButtonSendClicked()
{
    qDebug() << "发送键被按下了";
    QString cmd = ui->lineEditSend->text() + "\r\n";
    m_telnet->sendData(cmd);
}

