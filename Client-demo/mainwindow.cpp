#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QtTelnet_global.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::MainWindow 第 %1 次执行").arg(tmpCnt);
#endif

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

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::MainWindow 第 %1 次结束").arg(tmpCnt);
#endif
}

MainWindow::~MainWindow()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::~MainWindow 第 %1 次执行").arg(tmpCnt);
#endif

    delete ui;
    delete m_telnet;

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::~MainWindow 第 %1 次结束").arg(tmpCnt);
#endif
}

void MainWindow::telnetLoginRequired()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::telnetLoginRequired 第 %1 次执行").arg(tmpCnt);
#endif

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

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::telnetLoginRequired 第 %1 次结束").arg(tmpCnt);
#endif
}

void MainWindow::telnetLoginFailed()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::telnetLoginFailed 第 %1 次执行").arg(tmpCnt);
#endif

    ui->textEdit->append("登陆失败!");

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::telnetLoginFailed 第 %1 次结束").arg(tmpCnt);
#endif
}

void MainWindow::telnetLoginIn()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::telnetLoginIn 第 %1 次执行").arg(tmpCnt);
#endif

    ui->textEdit->append("登陆成功!");

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::telnetLoginIn 第 %1 次结束").arg(tmpCnt);
#endif
}

void MainWindow::telnetLoginOut()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::telnetLoginOut 第 %1 次执行").arg(tmpCnt);
#endif

    ui->textEdit->append("登出成功!");

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::telnetLoginOut 第 %1 次结束").arg(tmpCnt);
#endif
}

void MainWindow::telnetMessage(const QString& data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::telnetMessage(QString) 第 %1 次执行").arg(tmpCnt);
#endif

    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->insertPlainText(data);

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::telnetMessage(QString) 第 %1 次结束").arg(tmpCnt);
#endif
}

void MainWindow::telnetConnectionError(QAbstractSocket::SocketError error)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::telnetConnectionError 第 %1 次执行").arg(tmpCnt);
#endif

    ui->textEdit->append(QString("连接错误: %1").arg(error));

    #if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::telnetConnectionError 第 %1 次结束").arg(tmpCnt);
#endif
}


void MainWindow::onPushButtonConnectClicked()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::onPushButtonConnectClicked 第 %1 次执行").arg(tmpCnt);
#endif

    qDebug() << "连接键被按下了";
    QString host = ui->lineEditIP->text();
    ui->textEdit->append(host);
    if(host == "")
    {
        ui->textEdit->append("IP地址不能为空!");
        return ;
    }
    m_telnet->connectToHost(host, 23);

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::onPushButtonConnectClicked 第 %1 次结束").arg(tmpCnt);
#endif
}


void MainWindow::onPushButtonDisconnectClicked()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::onPushButtonDisconnectClicked 第 %1 次执行").arg(tmpCnt);
#endif

    qDebug() << "断开键被按下了";
    m_telnet->close();

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::onPushButtonDisconnectClicked 第 %1 次结束").arg(tmpCnt);
#endif
}


void MainWindow::onPushButtonSendClicked()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("MainWindow::onPushButtonSendClicked 第 %1 次执行").arg(tmpCnt);
#endif

    qDebug() << "发送键被按下了";
    QString cmd = ui->lineEditSend->text() + "\r\n";
    m_telnet->sendData(cmd);

#if DUBUG_FUN_RUN
    qDebug() << QString("MainWindow::onPushButtonSendClicked 第 %1 次结束").arg(tmpCnt);
#endif
}

