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
    if (m_d->m_connected)
    {
        return;
    }
    m_d->m_socket->connectToHost(host, port);
}

void QtTelnet::login(const QString &user, const QString &pass)
{
    m_d->m_triedLogin = false;
    m_d->m_triedPass = false;

    m_d->m_login = user;
    m_d->m_pass = pass;
}

void QtTelnet::setWindowSize(const QSize &size)
{
    setWindowSize(size.width(), size.height());
}

void QtTelnet::setWindowSize(int width, int height)
{
    bool wasValid = isValidWindowSize();

    m_d->m_windowSize.setWidth(width);
    m_d->m_windowSize.setHeight(height);

    if (wasValid && isValidWindowSize())
    {
        m_d->sendWindowSize();
    }
    else if (isValidWindowSize() == true)
    {
        m_d->sendCommand(Common::WILL, Common::NAWS);
    }
    else if (wasValid == true)
    {
        m_d->sendCommand(Common::WONT, Common::NAWS);
    }
}

QSize QtTelnet::windowSize() const
{
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

void QtTelnet::setPromptParrern(const QRegularExpression &pattern)
{
    m_d->m_promptp = pattern;
}

void QtTelnet::setPromptParrern(const QString &pattern)
{
    setPromptParrern(QRegularExpression(QRegularExpression::escape(pattern)));
}

void QtTelnet::setLoginPattern(const QRegularExpression &pattern)
{
    m_d->m_loginp = pattern;
}

void QtTelnet::setLoginPattern(const QString &pattern)
{
    setLoginPattern(QRegularExpression(QRegularExpression::escape(pattern)));
}

void QtTelnet::setPasswordPattern(const QRegularExpression &pattern)
{
    m_d->m_passp = pattern;
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
    bool sendsync = false;
    char ch;

    switch (ctrl)
    {
    case InterruptProcess:  // Ctrl-C
        ch = Common::IP;
        sendsync = true;
        break;
    case AbortOutput:       // suspend/resume output
        ch = Common::AO;
        sendsync = true;
        break;
    case Break:             //
        ch = Common::BRK;
        break;
    case Suspend:           // Ctrl-Z
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
        sendsync = true;
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

    const char command[2] = {
		(char)Common::IAC
		, ch
		};
    m_d->sendCommand(command, sizeof(command));
    if (sendsync == true)
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
    if (m_d->m_connected == false)
    {
        return;
    }

    m_d->m_socket->flush();

    char toSend = (char)Common::DM;
    m_d->m_socket->write(&toSend, 1);

//    unsigned long long s = m_d->m_socket->socketDescriptor();
//    ::send(s, &toSend, 1, MSG_OOB); // 原版代码中这里使用的是winsock2中的send函数

}
