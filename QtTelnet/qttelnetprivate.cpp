#include "qttelnetprivate.h"
#include "qttelnetrfc.h"
#include "qttelnet.h"
#include "qttelnetauthnull.h"

#ifdef Q_OS_WIN
//#include <winsock2.h>
#endif
#include <QtEndian>


QtTelnetPrivate::QtTelnetPrivate(QtTelnet *parent)
    : QObject{parent}
    , m_q(parent)
    , m_socket(nullptr)
    , m_notifier(nullptr)
    , m_connected(false)
    , m_nocheckp(false)
    , m_triedLogin(false)
    , m_triedPass(false)
    , m_firstTry(true)
    , m_curAuth(nullptr)
    , m_nullAuth(true)  // 原版是 false, 但是那样会导致华为设备第一次登陆时不能自动输入用户名
    , m_loginp("ogin:\\s*$")
    , m_passp("assword:\\s*$")

{
    setSocket(new QTcpSocket(this));
}

QtTelnetPrivate::~QtTelnetPrivate()
{
    delete m_socket;
    delete m_notifier;
    delete m_curAuth;
}

bool QtTelnetPrivate::allowOption(int /*operation*/, int option)
{
    if (option == Common::Authentication
        || option == Common::SuppressGoAhead
        || option == Common::LineMode
        || option == Common::Status
        || option == Common::Logout
        || option == Common::TerminalType
        ||(option == Common::NAWS && m_q->isValidWindowSize())
        )
    {
        return true;
    }
    return false;
}

void QtTelnetPrivate::sendOptions()
{
    sendCommand(Common::WILL, Common::Authentication);
    sendCommand(Common::DO, Common::SuppressGoAhead);
    sendCommand(Common::WILL, Common::LineMode);
    sendCommand(Common::DO, Common::Status);

    if (m_q->isValidWindowSize())
    {
        sendCommand(Common::WILL, Common::NAWS);
    }
}

void QtTelnetPrivate::sendCommand(const QByteArray &command)
{
    if (!m_connected || command.isEmpty())
    {
        return;
    }

    if (command.size() == 3)
    {
        const char operation = command.at(1);
        const char option = command.at(2);
        if (alreadySent(operation, option))
        {
            return;
        }

        addSent(operation, option);
    }

    m_socket->write(command);

}

void QtTelnetPrivate::sendCommand(const char *command, int length)
{
    QByteArray array(command, length);
    sendCommand(array);
}

void QtTelnetPrivate::sendCommand(const char operation, const char option)
{
    const char ch[3] = {
		(char)Common::IAC
		, operation
		, option
		};
    sendCommand(ch, 3);
}

void QtTelnetPrivate::sendString(const QString &string)
{
    if (!m_connected || string.length() == 0)
    {
        return;
    }

    m_socket->write(string.toLocal8Bit());

}

bool QtTelnetPrivate::replyNeeded(uchar operation, uchar option)
{
    if (operation == Common::DO || operation == Common::DONT)
    {
        // RFC854 requires that we don't acknowledge
        // requests to enter a mode we're already in
        // RFC854 要求我们不对 进入我们已经处于的模式的请求 进行确认
        if ((operation == Common::DO && m_modes[option])
            || (operation == Common::DONT && !m_modes[option]))
        {
            return false;
        }
    }
    return true;
}

void QtTelnetPrivate::setMode(uchar operation, uchar option)
{
    // 如果操作选项不是 DO 或者 DONT, 直接退出
    if (operation != Common::DO && operation != Common::DONT)
    {
        return;
    }

    // 如果操作选项是 DO, 就把对应的选项模式设置为 true,
    // 如果       是 DONT, 就              设置为 false
    m_modes[option] = (operation == Common::DO);

    // 如果选项是 NAWS 且 对应模式是 true, 就发送窗口大小的信息
    // NAWS : 协商窗口尺寸(Negotiate About Window Size)
    if (option == Common::NAWS && m_modes[Common::NAWS])
    {
        sendWindowSize();
    }
}

bool QtTelnetPrivate::alreadySent(uchar operation, uchar option)
{
    QPair<uchar, uchar> value(operation, option);
    if (m_osent.contains(value))
    {
        m_osent.removeAll(value);

        return true;
    }

    return false;
}

void QtTelnetPrivate::addSent(uchar operation, uchar option)
{
    m_osent.append(QPair<uchar, uchar>(operation, option));

}

void QtTelnetPrivate::sendWindowSize()
{
    if (m_modes[Common::NAWS] == false
        || m_q->isValidWindowSize() == false
        )
    {
        return;
    }

//    int height = htons(m_windowSize.height());
//    int width = htons(m_windowSize.width());
    quint16 height = qToBigEndian(m_windowSize.height());
    quint16 width = qToBigEndian(m_windowSize.width());

    const char ch[9] = {
        (char)Common::IAC
        , (char)Common::SB
        , (char)Common::NAWS
        , (char)(width & 0x00ff)
        , (char)(width >> 8)
        , (char)(height & 0x00ff)
        , (char)(height >> 8)
        , (char)Common::IAC
        , (char)Common::SE
    };

    sendCommand(ch, sizeof(ch));

}

int QtTelnetPrivate::parsePlaintext(const QByteArray &data)
{
    int consumed = 0;
    int length = data.indexOf('\0');

    if (length == -1)
    {
        length = data.size();
        consumed = length;
    }
    else {
        // 把 '\0' 舍去
        consumed = length + 1;
    }

//    QString text = QString::fromLocal8Bit(data.constData(), length);
    QString text = QString::fromUtf8(data.constData(), length);
    QRegularExpression regex("\x1B\\[[0-9;]*[A-Za-z]");
    text = text.replace(regex, "");

    if (!m_nocheckp && m_nullAuth)
    {
        if (!m_promptp.pattern().isEmpty() && m_promptp.match(text).hasMatch())
        {
            emit m_q->loggedIn();
            m_nocheckp = true;
        }
    }
    if (!m_nocheckp && m_nullAuth)
    {
        if (!m_loginp.pattern().isEmpty() && m_loginp.match(text).hasMatch())
        {
            if (m_triedLogin || m_firstTry)
            {
                // 显示登录提示
                emit m_q->message(text);
                text.clear();
                // 获取 (新) 登录
                emit m_q->loginRequired();
                m_firstTry = false;
            }
            if (!m_triedLogin)
            {
                m_q->sendData(m_login);
                m_triedLogin = true;
            }
        }

        if (!m_passp.pattern().isEmpty() && m_passp.match(text).hasMatch())
        {
            if (m_triedPass || m_firstTry)
            {
                // 显示密码提示
                emit m_q->message(text);
                text.clear();
                // 获取 (新) 密码
                emit m_q->loginRequired();
                m_firstTry = false;
            }
            if (!m_triedPass)
            {
                m_q->sendData(m_pass);
                m_triedPass = true;
                // 我们不必再存储密码了
                m_pass.fill(' ');
                m_pass.resize(0);
            }
        }
    }

    if (!text.isEmpty())
    {
        emit m_q->message(text);
    }
    return consumed;

}

// 返回消耗的字节数
int QtTelnetPrivate::parseIAC(const QByteArray &data)
{
    if (data.isEmpty())
    {
        return 0;
    }

    Q_ASSERT(uchar(data.at(0)) == Common::IAC);

    // IAC, Operation, Option
    if (data.size() >= 3 && isOperation(data[1]))
    {
        const uchar operation = data[1];
        const uchar option = data[2];
        if (operation == Common::WONT && option == Common::Logout)
        {
            m_q->close();
        }
        else {
            if (operation == Common::DONT && option == Common::Authentication)
            {
                // question flag
                if (m_loginp.pattern().isEmpty() && m_passp.pattern().isEmpty())
                {
                    emit m_q->loggedIn();
                }
                m_nullAuth = true;
            }
            if (replyNeeded(operation, option))
            {
                bool allowed = allowOption(operation, option);
                sendCommand(opposite(operation, allowed), option);
                setMode(operation, option);
            }
        }
        return 3;
    }
    // IAC Command
    else if (data.size() >= 2 && isCommand(data[1]))
    {
        return 2;
    }

    QByteArray subOption = getSubOption(data);
    if (subOption.isEmpty())
    {
        return 0;
    }

    // IAC SB Operation SubOption [...] IAC SE
    switch (subOption[0])
    {
    case Common::Authentication:
        parseSubAuth(subOption);
        break;
    case Common::TerminalType:
        parseSubTT(subOption);
        break;
    case Common::NAWS:
        parseSubNAWS(data);
        break;
    default:
        qWarning("QtTelnetPrivate::parseIAC: unknown subOption %d",
                 quint8(subOption.at(0)));
    }
    return subOption.size() + 4;
}

bool QtTelnetPrivate::isOperation(const uchar ch)
{
    return (ch == Common::WILL
            || ch == Common::WONT
            || ch == Common::DO
            || ch == Common::DONT
            );
}

bool QtTelnetPrivate::isCommand(const uchar ch)
{
    return (ch == Common::DM);
}

QByteArray QtTelnetPrivate::getSubOption(const QByteArray &data)
{
    Q_ASSERT(!data.isEmpty() && uchar(data[0]) == Common::IAC);

    if (data.size() < 4 || uchar(data[1]) != Common::SB)
    {
        return QByteArray();
    }

    for (int i = 2; i < data.size() - 1; ++i)
    {
        if (uchar(data[i]) == Common::IAC && uchar(data[i + 1]) == Common::SE)
        {
            return data.mid(2, i - 2);
        }
    }

    return QByteArray();
}

void QtTelnetPrivate::parseSubAuth(const QByteArray &data)
{
    Q_ASSERT(data[0] == Common::Authentication);

    if (!m_curAuth && data[1] == Common::SEND)
    {
        int pos = 2;
        while (pos < data.size() && !m_curAuth)
        {
            m_curAuth = m_auths[data[pos]];
            pos += 2;

            if (m_curAuth)
            {
                emit m_q->loginRequired();
                break;
            }
        }
        if (!m_curAuth)
        {
            m_curAuth = new QtTelnetAuthNull;
            m_nullAuth = true;
            if (m_loginp.pattern().isEmpty() && m_passp.pattern().isEmpty())
            {
                m_nocheckp = true;
            }
        }
    }

    if (m_curAuth)
    {
        const QByteArray array = m_curAuth->authStep(data);
        if (!array.isEmpty())
        {
            sendCommand(array);
        }

        if (m_curAuth->getState() == QtTelnetAuth::AuthFailure)
        {
            emit m_q->loginFailed();
        }
        else if (m_curAuth->getState() == QtTelnetAuth::AuthSuccess)
        {
            if (m_loginp.pattern().isEmpty() && m_passp.pattern().isEmpty())
            {
                emit m_q->loggedIn();
            }
            if (m_nullAuth == false)
            {
                m_nocheckp = true;
            }
        }
    }

}

void QtTelnetPrivate::parseSubTT(const QByteArray &data)
{
    Q_ASSERT(!data.isEmpty() && data[0] == Common::TerminalType);

    if (data.size() < 2 || data[1] != Common::SEND)
    {
        return;
    }

    const char ch1[4] = {
        (char)Common::IAC
        , (char)Common::SB
        , (char)Common::TerminalType
        , (char)Common::IS
    };
    sendCommand(ch1, sizeof(ch1));
    sendString("UNKNOWN");

    const char ch2[2] = {
        (char)Common::IAC
        , (char)Common::SE
    };
    sendCommand(ch2, sizeof(ch2));

}

void QtTelnetPrivate::parseSubNAWS(const QByteArray &data)
{
    Q_UNUSED(data);

}

uchar QtTelnetPrivate::opposite(uchar operation, bool positive)
{
    if (operation == Common::DO)
    {
        return (positive ? Common::WILL : Common::WONT);
    }
    else if (operation == Common::DONT)
    {
        return Common::WONT;    // Not allowed to say WILL
    }
    else if (operation == Common::WILL)
    {
        return (positive ? Common::DO : Common::DONT);
    }
    else if (operation == Common::WONT)
    {
        return Common::DONT;
    }
    return (uchar)0;
}


void QtTelnetPrivate::consume()
{
    const QByteArray data = m_buffer.readAll();
    qDebug() << "接收到的数据是 " << data;

    int currPos = 0;    // current position, 数据处理到了当前位置
    int prevPos = -1;   // previous position, 数据上次处理到的位置

    // 如果数据没有处理完成, 并且还在处理中, 就继续
    while (prevPos < currPos && currPos < data.size())
    {
        prevPos = currPos;
        const uchar uch = uchar(data[currPos]);
        // Data Mark, 标记数据流边界, 无需处理, 跳过
        if (uch == Common::DM)
        {
            ++currPos;
        }
        // Interpret As Command, 下一字节为 telnet 命令
        else if (uch == Common::IAC)
        {
            currPos += parseIAC(data.mid(currPos));
        }
        // 作为文本处理
        else {
            currPos += parsePlaintext(data.mid(currPos));
        }
    }
    // 如果数据没有处理完, 就放回缓存区
    if (currPos < data.size())
    {
        m_buffer.push_back(data.mid(currPos));
    }
}

void QtTelnetPrivate::setSocket(QTcpSocket *socket)
{
    if (m_socket)
    {
        m_q->logout();
        m_socket->flush();
    }
    delete m_socket;
    m_connected = false;

    m_socket = socket;
    if (m_socket)
    {
        connect(m_socket, &QTcpSocket::connected,
                this, &QtTelnetPrivate::socketConnected);
        connect(m_socket, &QTcpSocket::disconnected,
                this, &QtTelnetPrivate::socketConnectionClosed);
        connect(m_socket, &QTcpSocket::readyRead,
                this, &QtTelnetPrivate::socketReadyRead);
        connect(m_socket, &QTcpSocket::errorOccurred,
                this, &QtTelnetPrivate::socketError);
    }

}

void QtTelnetPrivate::socketConnected()
{
    m_connected = true;
    delete m_notifier;

    m_notifier = new QSocketNotifier(m_socket->socketDescriptor(),
                                     QSocketNotifier::Exception,
                                     this);
    connect(m_notifier, &QSocketNotifier::activated,
            this, &QtTelnetPrivate::socketException);
    sendOptions();

}

void QtTelnetPrivate::socketConnectionClosed()
{
    // 注销对套接字的监听
    delete m_notifier;
    m_notifier = nullptr;

    m_connected = false;
    emit m_q->loggedOut();

}

// 接收数据, 然后处理
void QtTelnetPrivate::socketReadyRead()
{
    m_buffer.append(m_socket->readAll());
    consume();
}

void QtTelnetPrivate::socketError(QAbstractSocket::SocketError error)
{
    emit m_q->connectionError(error);
}

void QtTelnetPrivate::socketException(int)
{
    qDebug("out-of-band data received, should handle that here!");
}
