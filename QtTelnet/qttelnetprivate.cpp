#include "qttelnetprivate.h"
#include "qttelnetrfc.h"
#include "qttelnet.h"
#include "qttelnetauthnull.h"

#include "QtTelnet_global.h"

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
    , m_nullAuth(true)  // 原版是 false, 但是那样会导致第一次登陆时不能正常登录
    , m_loginp("sername:\\s*$")
    , m_passp("assword:\\s*$")

{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::QtTelnetPrivate(QtTelnet*) 第 %1 次执行").arg(tmpCnt);
    #endif

    setSocket(new QTcpSocket(this));

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::QtTelnetPrivate(QtTelnet*) 第 %1 次结束").arg(tmpCnt);
    #endif
}

QtTelnetPrivate::~QtTelnetPrivate()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::~QtTelnetPrivate() 第 %1 次执行").arg(tmpCnt);
    #endif

    delete m_socket;
    delete m_notifier;
    delete m_curAuth;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::~QtTelnetPrivate() 第 %1 次结束").arg(tmpCnt);
    #endif
}

bool QtTelnetPrivate::allowOption(int /*operation*/, int option)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::allowOption(int, int) 第 %1 次执行").arg(tmpCnt);
    #endif

    if (option == Common::Authentication
        || option == Common::SuppressGoAhead
        || option == Common::LineMode
        || option == Common::Status
        || option == Common::Logout
        || option == Common::TerminalType
        ||(option == Common::NAWS && m_q->isValidWindowSize())
        )
    {

#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::allowOption(int, int) 第 %1 次结束, true").arg(tmpCnt);
    #endif

        return true;
    }
#if DUBUG_FUN_RUN

    qDebug() << QString("QtTelnetPrivate::allowOption(int, int) 第 %1 次结束, false").arg(tmpCnt);
    #endif

    return false;
}

void QtTelnetPrivate::sendOptions()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::sendOptions() 第 %1 次执行").arg(tmpCnt);
    #endif

    sendCommand(Common::WILL, Common::Authentication);
    sendCommand(Common::DO, Common::SuppressGoAhead);
    sendCommand(Common::WILL, Common::LineMode);
    sendCommand(Common::DO, Common::Status);

    if (m_q->isValidWindowSize())
    {
        sendCommand(Common::WILL, Common::NAWS);
    }
#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::sendOptions() 第 %1 次结束, false").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::sendCommand(const QByteArray &command)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::sendCommand(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

    if (!m_connected || command.isEmpty())
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::sendCommand(QByteArray) 第 %1 次结束, !m_connected || command.isEmpty()").arg(tmpCnt);
    #endif

        return;
    }

    if (command.size() == 3)
    {
        const char operation = command.at(1);
        const char option = command.at(2);
        if (alreadySent(operation, option))
        {
#if DUBUG_FUN_RUN
            qDebug() << QString("QtTelnetPrivate::sendCommand(QByteArray) 第 %1 次结束, alreadySent(operation, option)").arg(tmpCnt);
    #endif

            return;
        }

        addSent(operation, option);
    }

    m_socket->write(command);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::sendCommand(QByteArray) 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::sendCommand(const char *command, int length)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::sendCommand(char, int) 第 %1 次执行").arg(tmpCnt);
    #endif

    QByteArray array(command, length);
    sendCommand(array);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::sendCommand(char, int) 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::sendCommand(const char operation, const char option)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::sendCommand(char, char) 第 %1 次执行").arg(tmpCnt);
    #endif

    const char ch[3] = {
		(char)Common::IAC
		, operation
		, option
		};
    sendCommand(ch, 3);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::sendCommand(char, char) 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::sendString(const QString &string)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::sendString(QString) 第 %1 次执行").arg(tmpCnt);
    #endif

    if (!m_connected || string.length() == 0)
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::sendString(QString) 第 %1 次结束").arg(tmpCnt);
    #endif

        return;
    }

    m_socket->write(string.toLocal8Bit());

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::sendString(QString) 第 %1 次结束").arg(tmpCnt);
    #endif
}

bool QtTelnetPrivate::replyNeeded(uchar operation, uchar option)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::replyNeeded(uchar, uchar) 第 %1 次执行").arg(tmpCnt);
    #endif

    if (operation == Common::DO || operation == Common::DONT)
    {
        // RFC854 requires that we don't acknowledge
        // requests to enter a mode we're already in
        // RFC854 要求我们不对 进入我们已经处于的模式的请求 进行确认
        if ((operation == Common::DO && m_modes[option])
            || (operation == Common::DONT && !m_modes[option]))
        {
#if DUBUG_FUN_RUN
            qDebug() << QString("QtTelnetPrivate::replyNeeded(uchar, uchar) 第 %1 次结束, false").arg(tmpCnt);
    #endif

            return false;
        }
    }

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::replyNeeded(uchar, uchar) 第 %1 次结束, true").arg(tmpCnt);
    #endif

    return true;
}

void QtTelnetPrivate::setMode(uchar operation, uchar option)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::setMode(uchar, uchar) 第 %1 次执行").arg(tmpCnt);
    #endif

    if (operation != Common::DO && operation != Common::DONT)
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::setMode(uchar, uchar) 第 %1 次结束, operation != Common::DO && operation != Common::DONT").arg(tmpCnt);
    #endif

        return;
    }
	// 20230827 option 写成了 operation 已改正
    m_modes[option] = (operation == Common::DO);

    if (option == Common::NAWS && m_modes[Common::NAWS])
    {
        sendWindowSize();
    }
#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::setMode(uchar, uchar) 第 %1 次结束").arg(tmpCnt);
    #endif
}

bool QtTelnetPrivate::alreadySent(uchar operation, uchar option)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::alreadySent(uchar, uchar) 第 %1 次执行").arg(tmpCnt);
    #endif

    QPair<uchar, uchar> value(operation, option);
    if (m_osent.contains(value))
    {
        m_osent.removeAll(value);

#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::alreadySent(uchar, uchar) 第 %1 次结束, true").arg(tmpCnt);
    #endif

        return true;
    }

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::alreadySent(uchar, uchar) 第 %1 次结束, false").arg(tmpCnt);
    #endif

    return false;
}

void QtTelnetPrivate::addSent(uchar operation, uchar option)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::addSent(uchar, uchar) 第 %1 次执行").arg(tmpCnt);
    #endif

    m_osent.append(QPair<uchar, uchar>(operation, option));

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::addSent(uchar, uchar) 第 %1 次结束, false").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::sendWindowSize()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::sendWindowSize() 第 %1 次执行").arg(tmpCnt);
    #endif

    if (m_modes[Common::NAWS] == false
        || m_q->isValidWindowSize() == false
        )
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::sendWindowSize() 第 %1 次结束, !m_modes[Common::NAWS] || !m_q->isValidWindowSize()").arg(tmpCnt);
    #endif

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

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::sendWindowSize() 第 %1 次结束").arg(tmpCnt);
    #endif
}

int QtTelnetPrivate::parsePlaintext(const QByteArray &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::parsePlaintext(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

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

    QString text = QString::fromLocal8Bit(data.constData(), length);
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

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::parsePlaintext(QByteArray) 第 %1 次结束").arg(tmpCnt);
    #endif

    return consumed;

}

// 返回消耗的字节数
int QtTelnetPrivate::parseIAC(const QByteArray &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::parseIAC(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

    if (data.isEmpty())
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::parseIAC(QByteArray) 第 %1 次结束, 0").arg(tmpCnt);
    #endif

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
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::parseIAC(QByteArray) 第 %1 次结束, 3").arg(tmpCnt);
    #endif

        return 3;
    }
    // IAC Command
    else if (data.size() >= 2 && isCommand(data[1]))
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::parseIAC(QByteArray) 第 %1 次结束, 2").arg(tmpCnt);
    #endif

        return 2;
    }

    QByteArray subOption = getSubOption(data);
    if (subOption.isEmpty())
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::parseIAC(QByteArray) 第 %1 次结束, 0").arg(tmpCnt);
    #endif

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
#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::parseIAC(QByteArray) 第 %1 次结束, %2").arg(tmpCnt).arg(subOption.size() + 4);
    #endif

    return subOption.size() + 4;
}

bool QtTelnetPrivate::isOperation(const uchar ch)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::isOperation(uchar) 第 %1 次执行").arg(tmpCnt);
    #endif

    bool debugBool = (ch == Common::WILL
                      || ch == Common::WONT
                      || ch == Common::DO
                      || ch == Common::DONT
                      );

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::isOperation(uchar) 第 %1 次结束").arg(tmpCnt);
    #endif

//    return (ch == Common::WILL
//            || ch == Common::WONT
//            || ch == Common::DO
//            || ch == Common::DONT
//            );
    return debugBool;
}

bool QtTelnetPrivate::isCommand(const uchar ch)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::isCommand(uchar) 第 %1 次执行").arg(tmpCnt);
    #endif

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::isCommand(uchar) 第 %1 次结束, %2").arg(tmpCnt).arg(ch == Common::DM);
    #endif

    return (ch == Common::DM);
}

QByteArray QtTelnetPrivate::getSubOption(const QByteArray &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::getSubOption(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

    Q_ASSERT(!data.isEmpty() && uchar(data[0]) == Common::IAC);

    if (data.size() < 4 || uchar(data[1]) != Common::SB)
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::getSubOption(QByteArray) 第 %1 次结束, QByteArray()").arg(tmpCnt);
    #endif

        return QByteArray();
    }

    for (int i = 2; i < data.size() - 1; ++i)
    {
        if (uchar(data[i]) == Common::IAC && uchar(data[i + 1]) == Common::SE)
        {
#if DUBUG_FUN_RUN
            qDebug() << QString("QtTelnetPrivate::getSubOption(QByteArray) 第 %1 次结束, data.mid(2, i - 2)").arg(tmpCnt);
    #endif

            return data.mid(2, i - 2);
        }
    }

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::getSubOption(QByteArray) 第 %1 次结束, QByteArray()").arg(tmpCnt);
    #endif

    return QByteArray();
}

void QtTelnetPrivate::parseSubAuth(const QByteArray &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::parseSubAuth(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

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

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::parseSubAuth(QByteArray) 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::parseSubTT(const QByteArray &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::parseSubTT(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

    Q_ASSERT(!data.isEmpty() && data[0] == Common::TerminalType);

    if (data.size() < 2 || data[1] != Common::SEND)
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::parseSubTT(QByteArray) 第 %1 次结束, data.size() < 2 || data[1] != Common::SEND").arg(tmpCnt);
    #endif

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

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::parseSubTT(QByteArray) 第 %1 次结束").arg(tmpCnt);
    #endif

}

void QtTelnetPrivate::parseSubNAWS(const QByteArray &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::parseSubNAWS(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

    Q_UNUSED(data);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::parseSubNAWS(QByteArray) 第 %1 次结束").arg(tmpCnt);
    #endif
}

// 返回与我们传入的值相反的值
uchar QtTelnetPrivate::opposite(uchar operation, bool positive)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::opposite(uchar, bool) 第 %1 次执行").arg(tmpCnt);
    #endif

    if (operation == Common::DO)
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::opposite(uchar, bool) 第 %1 次结束, DO").arg(tmpCnt);
    #endif

        return (positive ? Common::WILL : Common::WONT);
    }
    else if (operation == Common::DONT)
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::opposite(uchar, bool) 第 %1 次结束, DONT").arg(tmpCnt);
    #endif

        return Common::WONT;    // Not allowed to say WILL
    }
    else if (operation == Common::WILL)
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::opposite(uchar, bool) 第 %1 次结束, WILL").arg(tmpCnt);
    #endif

        return (positive ? Common::DO : Common::DONT);
    }
    else if (operation == Common::WONT)
    {
#if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetPrivate::opposite(uchar, bool) 第 %1 次结束, WONT").arg(tmpCnt);
    #endif

        return Common::DONT;
    }

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::opposite(uchar, bool) 第 %1 次结束").arg(tmpCnt);
    #endif

    return (uchar)0;
}

/**
 * @brief QtTelnetPrivate::consume
 * * 处理缓存区的数据
 */
void QtTelnetPrivate::consume()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::consume() 第 %1 次执行").arg(tmpCnt);
    #endif

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
#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::consume() 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::setSocket(QTcpSocket *socket)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::setSocket(QTcpSocket*) 第 %1 次执行").arg(tmpCnt);
    #endif

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

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::setSocket(QTcpSocket*) 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::socketConnected()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::socketConnected() 第 %1 次执行").arg(tmpCnt);
    #endif

    m_connected = true;
    delete m_notifier;

    m_notifier = new QSocketNotifier(m_socket->socketDescriptor(),
                                     QSocketNotifier::Exception,
                                     this);
    connect(m_notifier, &QSocketNotifier::activated,
            this, &QtTelnetPrivate::socketException);
    sendOptions();

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::socketConnected() 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::socketConnectionClosed()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::socketConnectionClosed() 第 %1 次执行").arg(tmpCnt);
    #endif

    // 注销对套接字的监听
    delete m_notifier;
    m_notifier = nullptr;

    m_connected = false;
    emit m_q->loggedOut();

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::socketConnectionClosed() 第 %1 次结束").arg(tmpCnt);
    #endif
}

// 接收数据, 然后处理
void QtTelnetPrivate::socketReadyRead()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::socketReadyRead() 第 %1 次执行").arg(tmpCnt);
    #endif

//    QByteArray debugEcho = m_socket->readAll();
//    m_buffer.append(debugEcho);
//    qDebug() << "socketReadyRead 读取的数据是 : " << debugEcho;
    m_buffer.append(m_socket->readAll());
    consume();

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::socketReadyRead() 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::socketError(QAbstractSocket::SocketError error)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::socketError(QAbstractSocket::SocketError) 第 %1 次执行").arg(tmpCnt);
    #endif

    emit m_q->connectionError(error);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::socketError(QAbstractSocket::SocketError) 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetPrivate::socketException(int)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetPrivate::socketException(int) 第 %1 次执行").arg(tmpCnt);
    #endif

#if DUBUG_FUN_RUN
    qDebug("out-of-band data received, should handle that here!");
#endif

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetPrivate::socketException(int) 第 %1 次结束").arg(tmpCnt);
    #endif
}
