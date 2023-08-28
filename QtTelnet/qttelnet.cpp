#include "qttelnet.h"
#include "qttelnetprivate.h"
#include "qttelnetrfc.h"

#include "QtTelnet_global.h"


QtTelnet::QtTelnet(QObject *parent)
    : QObject{parent}
    , m_d(new QtTelnetPrivate(this))
{
    #if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::QtTelnet 第 %1 次执行").arg(tmpCnt);
#endif
#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::QtTelnet 第 %1 次结束").arg(tmpCnt);
#endif
}

QtTelnet::~QtTelnet()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::~QtTelnet 第 %1 次执行").arg(tmpCnt);
#endif
    delete m_d;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::~QtTelnet 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::connectToHost(const QString &host, quint16 port)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::connectToHost 第 %1 次执行").arg(tmpCnt);
    #endif

    if (m_d->m_connected)
    {
        return;
    }
    m_d->m_socket->connectToHost(host, port);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::connectToHost 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::login(const QString &user, const QString &pass)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::login 第 %1 次执行").arg(tmpCnt);
    #endif

    m_d->m_triedLogin = false;
    m_d->m_triedPass = false;

    m_d->m_login = user;
    m_d->m_pass = pass;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::login 第 %1 次结束").arg(tmpCnt);
    #endif

}

void QtTelnet::setWindowSize(const QSize &size)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setWindowSize(QSize) 第 %1 次执行").arg(tmpCnt);
    #endif

    setWindowSize(size.width(), size.height());

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setWindowSize(QSize) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::setWindowSize(int width, int height)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setWindowSize(int, int) 第 %1 次执行").arg(tmpCnt);
    #endif

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

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setWindowSize(int, int) 第 %1 次结束").arg(tmpCnt);
#endif
}

QSize QtTelnet::windowSize() const
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::windowSize() 第 %1 次执行").arg(tmpCnt);
    #endif

    QSize debugSize = m_d->m_modes[Common::NAWS] ? m_d->m_windowSize : QSize();

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::windowSize() 第 %1 次结束").arg(tmpCnt);
    #endif

//    return (m_d->m_modes[Common::NAWS] ? m_d->m_windowSize : QSize());
    return debugSize;
}

bool QtTelnet::isValidWindowSize() const
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::isValidWindowSize() 第 %1 次执行").arg(tmpCnt);
    #endif

    bool debugBool = windowSize().isValid();

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::isValidWindowSize() 第 %1 次结束").arg(tmpCnt);
    #endif

//    return windowSize().isValid();
    return debugBool;
}

void QtTelnet::setSocket(QTcpSocket *socket)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setSocket(QTcpSocket *) 第 %1 次执行").arg(tmpCnt);
    #endif

    m_d->setSocket(socket);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setSocket(QTcpSocket *) 第 %1 次结束").arg(tmpCnt);
#endif
}

QTcpSocket *QtTelnet::socket() const
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::socket() 第 %1 次执行").arg(tmpCnt);
    #endif

    QTcpSocket* debugSocket = m_d->m_socket;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::socket() 第 %1 次结束").arg(tmpCnt);
    #endif

//    return m_d->m_socket;
    return debugSocket;
}

void QtTelnet::setPromptParrern(const QRegularExpression &pattern)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setPromptParrern(QRegularExpression) 第 %1 次执行").arg(tmpCnt);
    #endif

    m_d->m_promptp = pattern;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setPromptParrern(QRegularExpression) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::setPromptParrern(const QString &pattern)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setPromptParrern(QString) 第 %1 次执行").arg(tmpCnt);
    #endif

    setPromptParrern(QRegularExpression(QRegularExpression::escape(pattern)));

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setPromptParrern(QString) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::setLoginPattern(const QRegularExpression &pattern)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setLoginPattern(QRegularExpression) 第 %1 次执行").arg(tmpCnt);
    #endif

    m_d->m_loginp = pattern;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setLoginPattern(QRegularExpression) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::setLoginPattern(const QString &pattern)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setLoginPattern(QString) 第 %1 次执行").arg(tmpCnt);
    #endif

    setLoginPattern(QRegularExpression(QRegularExpression::escape(pattern)));

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setLoginPattern(QString) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::setPasswordPattern(const QRegularExpression &pattern)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setPasswordPattern(QRegularExpression) 第 %1 次执行").arg(tmpCnt);
    #endif

    m_d->m_passp = pattern;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setPasswordPattern(QRegularExpression) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::setPasswordPattern(const QString &pattern)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::setPasswordPattern(QString) 第 %1 次执行").arg(tmpCnt);
    #endif

    setPasswordPattern(QRegularExpression(QRegularExpression::escape(pattern)));

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::setPasswordPattern(QString) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::close()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::close() 第 %1 次执行").arg(tmpCnt);
    #endif

    if (m_d->m_connected == false)
    {

#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnet::close() 第 %1 次结束, m_d->m_connected == false").arg(tmpCnt);
        #endif

        return;
    }

    delete m_d->m_notifier;
    m_d->m_notifier = nullptr;
    m_d->m_connected = false;
    m_d->m_socket->close();
//    emit loggedOut();

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::close() 第 %1 次结束").arg(tmpCnt);
    #endif

}

void QtTelnet::logout()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::logout() 第 %1 次执行").arg(tmpCnt);
    #endif

    m_d->sendCommand(Common::DO, Common::Logout);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::logout() 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::sendControl(Control ctrl)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::sendControl(Control) 第 %1 次执行").arg(tmpCnt);
    #endif

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

#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnet::sendControl(Control) 第 %1 次结束, default").arg(tmpCnt);
        #endif

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

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::sendControl(Control) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::sendData(const QString &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::sendData(QString) 第 %1 次执行").arg(tmpCnt);
    #endif

    if (m_d->m_connected == false)
    {

#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnet::sendData(QString) 第 %1 次结束, m_d->m_connected == false").arg(tmpCnt);
        #endif

        return;
    }

    QByteArray byteArray = data.toLocal8Bit();
    m_d->m_socket->write(byteArray);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::sendData(QString) 第 %1 次结束").arg(tmpCnt);
#endif
}

void QtTelnet::sendSync()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnet::sendSync() 第 %1 次执行").arg(tmpCnt);
    #endif

    if (m_d->m_connected == false)
    {

#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnet::sendSync() 第 %1 次结束, m_d->m_connected == false").arg(tmpCnt);
        #endif

        return;
    }

    m_d->m_socket->flush();

//    int s = m_d->m_socket->socketDescriptor();
    char toSend = (char)Common::DM;
    m_d->m_socket->write(&toSend, 1);
    // ::send(s, &toSend, 1, MSG_OOB); // 原版代码中这里使用的是winsock2中的send函数

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnet::sendSync() 第 %1 次结束").arg(tmpCnt);
#endif
}
