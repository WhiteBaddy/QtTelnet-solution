#include "qttelnet.h"
#include "qttelnetprivate.h"
#include "qttelnetrfc.h"


QtTelnet::QtTelnet(QObject *parent)
    : QObject{parent}
    , m_d(new QtTelnetPrivate(this))
{

}

QtTelnet::~QtTelnet()
{
    delete m_d;
}

void QtTelnet::connectToHost(const QString &host, quint16 port)
{
    // 如果已经处于连接状态, 直接退出
    if (m_d->m_connected)
    {
        return;
    }
    // 输入远程主机的 地址 与 端口号
    m_d->m_socket->connectToHost(host, port);
}

void QtTelnet::login(const QString &user, const QString &pass)
{
    // 重置 用户名 和 密码 的输入状态
    m_d->m_triedLogin = false;
    m_d->m_triedPass = false;
    // 把 用户名 和 密码 保存到类的成员变量中, 输入有另外的函数进行
    m_d->m_userName = user;
    m_d->m_passwd = pass;
}

void QtTelnet::setWindowSize(const QSize &size)
{
    setWindowSize(size.width(), size.height());
}

void QtTelnet::setWindowSize(int width, int height)
{
    // 保存修改前窗口尺寸是否有效
    bool wasValid = isValidWindowSize();
    // 设置新的窗口尺寸
    m_d->m_windowSize.setWidth(width);
    m_d->m_windowSize.setHeight(height);
    // 如果 修改前 与 修改后 窗口尺寸都有效, 向远程主机同步新的窗口尺寸
    if (wasValid && isValidWindowSize())
    {
        m_d->sendWindowSize();
    }
    // 如果 修改前 无效, 修改后 有效, 以协商的方式向远程主机发送
    else if (isValidWindowSize() == true)
    {
        m_d->sendCommand(Common::WILL, Common::NAWS);
    }
    // 如果 修改前 有效, 修改后 无效, 就请求远程主机拒绝协商
    else if (wasValid == true)
    {
        m_d->sendCommand(Common::WONT, Common::NAWS);
    }
}

QSize QtTelnet::windowSize() const
{
    // 如果协商窗口尺寸的请求模式为 true， 返回 当前 窗口, 否则返回一个 空窗口
    return (m_d->m_modes[Common::NAWS] ? m_d->m_windowSize : QSize());
}

bool QtTelnet::isValidWindowSize() const
{
    return windowSize().isValid();
}

void QtTelnet::setSocket(QTcpSocket *socket)
{
    m_d->setSocket(socket);
}

QTcpSocket *QtTelnet::socket() const
{
    return m_d->m_socket;
}

void QtTelnet::setPromptPattern(const QRegularExpression &pattern)
{
    m_d->m_promptPattern = pattern;
}

void QtTelnet::setPromptPattern(const QString &pattern)
{
    setPromptPattern(QRegularExpression(QRegularExpression::escape(pattern)));
}

void QtTelnet::setLoginPattern(const QRegularExpression &pattern)
{
    m_d->m_loginPattern = pattern;
}

void QtTelnet::setLoginPattern(const QString &pattern)
{
    setLoginPattern(QRegularExpression(QRegularExpression::escape(pattern)));
}

void QtTelnet::setPasswordPattern(const QRegularExpression &pattern)
{
    m_d->m_passPattern = pattern;
}

void QtTelnet::setPasswordPattern(const QString &pattern)
{
    setPasswordPattern(QRegularExpression(QRegularExpression::escape(pattern)));
}

void QtTelnet::close()
{
    if (m_d->m_connected == false)
    {
        return;
    }

    delete m_d->m_notifier;
    m_d->m_notifier = nullptr;
    m_d->m_connected = false;
    m_d->m_socket->close();
//    emit loggedOut();
}

void QtTelnet::logout()
{
    m_d->sendCommand(Common::DO, Common::Logout);
}

void QtTelnet::sendControl(Control ctrl)
{
    bool needSendSync = false;
    char ch;

    switch (ctrl)
    {
    case InterruptProcess:  // Ctrl-C                   // 中断进程
        ch = Common::IP;
        needSendSync = true;
        break;
    case AbortOutput:       // suspend/resume output    // 终止输出操作
        ch = Common::AO;
        needSendSync = true;
        break;
    case Break:             //                          // 发送一个中断信号, 打断当前操作
        ch = Common::BRK;
        break;
    case Suspend:           // Ctrl-Z                   // 暂停当前操作
        ch = Common::SUSP;
        break;
    case EndOfFile:
        ch = Common::CEOF;
        break;
    case Abort:
        ch = Common::ABORT;
        break;
    case GoAhead:
        ch = Common::GA;
        break;
    case AreYouThere:
        ch = Common::AYT;
        needSendSync = true;
        break;
    case EraseCharacter:
        ch = Common::EC;
        break;
    case EraseLine:
        ch = Common::EL;
        break;
    default:
        return;
    }
    // 发送指令
    const char command[2] = {
		(char)Common::IAC
		, ch
		};
    m_d->sendCommand(command, sizeof(command));

    // 如果需要发送同步序列, 就发送
    if (needSendSync == true)
    {
        sendSync();
    }
}

void QtTelnet::sendData(const QString &data)
{
    if (m_d->m_connected == false)
    {
        return;
    }

    QByteArray byteArray = data.toLocal8Bit();
    m_d->m_socket->write(byteArray);
}

void QtTelnet::sendSync()
{
    // 如果未连接, 直接退出
    if (m_d->m_connected == false)
    {
        return;
    }
    // 确保所有待发送的数据立即发送
    m_d->m_socket->flush();
    // 发送 DM 数据标记
    char toSend = (char)Common::DM;
    m_d->m_socket->write(&toSend, 1);

//    unsigned long long s = m_d->m_socket->socketDescriptor();
//    ::send(s, &toSend, 1, MSG_OOB); // 原版代码中这里使用的是winsock2中的send函数

}
