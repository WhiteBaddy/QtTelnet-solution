#include "qttelnetprivate.h"
#include "qttelnetrfc.h"
#include "qttelnet.h"
#include "qttelnetauthnull.h"

//#ifdef Q_OS_WIN
//#include <winsock2.h>
//#endif
#include <QtEndian>


const QRegularExpression QtTelnetPrivate::m_ctrlPattern = QRegularExpression("\x1B\\[[0-9;]*[A-Za-z]");


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
    , m_loginPattern("ogin:\\s*$")
    , m_passPattern("assword:\\s*$")

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
    if (option == Common::Authentication        // 身份验证
        || option == Common::SuppressGoAhead    // 阻止继续
        || option == Common::LineMode           // 线路模式
        || option == Common::Status             // 状态
        || option == Common::Logout             // 注销
        || option == Common::TerminalType       // 终端类型
        ||(option == Common::NAWS && m_q->isValidWindowSize())  // 协商窗口尺寸 且 窗口尺寸有效
        )
    {
        return true;
    }
    return false;
}

void QtTelnetPrivate::sendOptions()
{
    sendCommand(Common::WILL, Common::Authentication);  // 请求远程主机进行身份验证
    sendCommand(Common::DO, Common::SuppressGoAhead);   // 请求自己阻止继续发送
    sendCommand(Common::WILL, Common::LineMode);        // 请求远程主机线路模式
    sendCommand(Common::DO, Common::Status);            // 请求自己启用状态

    if (m_q->isValidWindowSize())
    {
        sendCommand(Common::WILL, Common::NAWS);        // 如果窗口尺寸有效, 请求远程主机协商窗口尺寸
    }
}

void QtTelnetPrivate::sendCommand(const QByteArray &command)
{
    // 如果当前未连接 或 发送的指令为空, 直接退出
    if (!m_connected || command.isEmpty())
    {
        return;
    }

    // 如果指令长度为 3, 应该是一个请求操作的 telnet 指令
    if (command.size() == 3)
    {
        const char operation = command.at(1);
        const char option = command.at(2);
        // 如果这条指令已经发送过了, 就不再发送
        if (alreadySent(operation, option))
        {
            return;
        }
        // 添加这条指令到列表
        addSent(operation, option);
    }
    // 发送这条指令
    m_socket->write(command);

}

void QtTelnetPrivate::sendCommand(const char *command, int length)
{
    // 把字符数组格式的指令转化成 QByteArray 后发送
    QByteArray array(command, length);
    sendCommand(array);
}

void QtTelnetPrivate::sendCommand(const char operation, const char option)
{
    // 把请求操作的字符 前面加上 IAC 识别字符, 然后封装到字符数组后发送
    const char ch[3] = {
		(char)Common::IAC
		, operation
		, option
		};
    sendCommand(ch, 3);
}

void QtTelnetPrivate::sendString(const QString &string)
{
    // 发送一条字符串命令

    // 如果未连接 或 指令长度为 0 , 直接退出
    if (!m_connected || string.length() == 0)
    {
        return;
    }
    // 发送这条指令
    m_socket->write(string.toLocal8Bit());

}

bool QtTelnetPrivate::replyNeeded(uchar operation, uchar option)
{
    // 判断远程主机发送过来的指令是否需要回复确认

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
    // 设置模式

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
    // 判断指令是否发送过

    QPair<uchar, uchar> value(operation, option);
    // 如果发送过, 清除发送记录并返回 true
    if (m_osent.contains(value))
    {
        m_osent.removeAll(value);

        return true;
    }

    return false;
}

void QtTelnetPrivate::addSent(uchar operation, uchar option)
{
    // 添加一条指令的发送记录
    m_osent.append(QPair<uchar, uchar>(operation, option));

}

void QtTelnetPrivate::sendWindowSize()
{
    // 发送窗口尺寸
    // 如果对应模式不为 true 或 窗口尺寸无效, 就不发送, 直接退出
    if (m_modes[Common::NAWS] == false
        || m_q->isValidWindowSize() == false
        )
    {
        return;
    }

    // 把 数据转换成网络传输格式(大端模式)
//    int height = htons(m_windowSize.height());
//    int width = htons(m_windowSize.width());
    quint16 height = qToBigEndian(m_windowSize.height());
    quint16 width = qToBigEndian(m_windowSize.width());

    // 封装成字符数组后 发送
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
    // 把接下来的一部分数据当作文本数据来处理

    int consumed = 0;                   // 处理的长度, 第一个 '\0' 的位置, 没有 '\0' 就把缓存区全部处理掉
    int length = data.indexOf('\0');    // 寻找第一个 '\0' 的位置, 没有就返回 -1

    if (length == -1)
    {
        length = data.size();
        consumed = length;
    }
    else {
        // 把 '\0' 舍去
        consumed = length + 1;
    }

    // 把数据转化成文本, 然后去除 \x1B 开头的无用控制字符, 多为文字格式设置等
    // 换成 UTF8 是为了处理中文
//    QString text = QString::fromLocal8Bit(data.constData(), length);
    QString text = QString::fromUtf8(data.constData(), length);
    text = text.replace(m_ctrlPattern, "");

    // 如果 不检查 为 false 且 是空验证
    if (m_nocheckp == false && m_nullAuth == true)
    {
        // 如果提示的规则表达式不为空 且 与文本匹配
        if (!m_promptPattern.pattern().isEmpty() && m_promptPattern.match(text).hasMatch())
        {
            // 触发登陆成功的信号, 并把 不检查的变量 m_nocheckp 设置为 true
            emit m_q->loggedIn();
            m_nocheckp = true;
        }
    }
    // 如果 不检查 为 false 且 是空验证
    if (m_nocheckp == false && m_nullAuth == true)
    {
        // 检测是否在提示输入用户名
        if (!m_loginPattern.pattern().isEmpty() && m_loginPattern.match(text).hasMatch())
        {
            // 如果是 正在尝试登录 或者是 首次登录, 就 获取一次用户名和密码
            if (m_triedLogin == true || m_firstTry == true)
            {
                // 显示登录提示
                emit m_q->message(text);
                text.clear();
                // 获取 (新) 登录信息
                emit m_q->loginRequired(); // 这个信号的最终结果是获取了一次 用户名 和 密码
                m_firstTry = false;
            }
            // 如果当前状态是未尝试登录, 证明需要发送一次用户名
            // 发送用户名, 把尝试登录的状态改为 true, 表示已经发送过用户名了
            if (m_triedLogin == false)
            {
                m_q->sendData(m_userName);
                m_triedLogin = true;
            }
        }
        // 和上面的 发送用户名 流程相同
        if (!m_passPattern.pattern().isEmpty() && m_passPattern.match(text).hasMatch())
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
                m_q->sendData(m_passwd);
                m_triedPass = true;
                // 我们不必再存储密码了
                m_passwd.fill(' ');
                m_passwd.resize(0);
            }
        }
    }

    // 输出文本信息
    // 如果是 登录提示 或 密码提示, 当时就已经输出了并且清空了文本, 不会漏掉或者重复输出
    if (!text.isEmpty())
    {
        emit m_q->message(text);
    }
    // 返回本次处理数据的长度
    return consumed;

}

// 返回消耗的字节数
int QtTelnetPrivate::parseIAC(const QByteArray &data)
{
    // 缓存区为空就没有要处理的指令
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
            // 如果收到的指令是 要求 不进行 登陆验证
            if (operation == Common::DONT && option == Common::Authentication)
            {
                // question flag
                // 如果 用户名提示 和 密码提示 的规则 都为空, 就触发 登录成功的 信号
                if (m_loginPattern.pattern().isEmpty() && m_passPattern.pattern().isEmpty())
                {
                    emit m_q->loggedIn();
                }
                // 此时是 免认证登录 模式
                m_nullAuth = true;
            }

            // 如果远程主机的指令需要回复
            if (replyNeeded(operation, option))
            {
                // 先判断操作是否允许
                // 然后向远程主机回复对应的指令
                // 最后更新模式信息
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
        // 如果是 DM, 消耗掉两个字符后直接退出, 什么也不做
        return 2;
    }

    // 如果以上情况都不符合, 尝试获取命令的子选项
    QByteArray subOption = getSubOption(data);
    if (subOption.isEmpty())
    {
        return 0;
    }

    // IAC SB Operation SubOption [...] IAC SE
    // 通过判断子选项的第一个字符, 进入不同的操作
    switch (subOption[0])
    {
    case Common::Authentication:
        parseSubAuth(subOption);    // 认证
        break;
    case Common::TerminalType:      // 终端类型
        parseSubTT(subOption);
        break;
    case Common::NAWS:              // 协商窗口尺寸
        parseSubNAWS(data);
        break;
    default:                        // 未定义操作, 发出警告
        qWarning("QtTelnetPrivate::parseIAC: unknown subOption %d",
                 quint8(subOption.at(0)));
    }
    // 消耗掉的数据长度为 子操作的长度 以及 子操作指令两头的开始和结束, 分别是 IAC SB ... IAC SE
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

    // 先判断是否满足子操作的条件, 不满足直接返回空
    if (data.size() < 4 || uchar(data[1]) != Common::SB)
    {
        return QByteArray();
    }

    // 寻找子操作指令的终点, 找得到就返回对应指令, 否则还是返回空串
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
    // 子操作 : 认证

    Q_ASSERT(data[0] == Common::Authentication);

    // 如果 认证类型为 NULL, 且 第二个字节数据是 SEND
    if (m_curAuth == nullptr && data[1] == Common::SEND)
    {
        // 从第 3 个字节开始查询
        int pos = 2;
        while (pos < data.size() && m_curAuth == nullptr)
        {
            m_curAuth = m_auths[data[pos]];
            pos += 2;

            if (m_curAuth != nullptr)
            {
                emit m_q->loginRequired();
                break;
            }
        }
        if (m_curAuth == nullptr)
        {
            m_curAuth = new QtTelnetAuthNull;
            m_nullAuth = true;
            if (m_loginPattern.pattern().isEmpty() && m_passPattern.pattern().isEmpty())
            {
                m_nocheckp = true;
            }
        }
    }

    if (m_curAuth != nullptr)
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
            if (m_loginPattern.pattern().isEmpty() && m_passPattern.pattern().isEmpty())
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
    // 如果存在旧的 socket 连接, 就先关闭
    if (m_socket != nullptr)
    {
        m_q->logout();
        m_socket->flush();
        delete m_socket; // 这句本来写在 if 的外面
    }
//    delete m_socket;
    m_connected = false;

    // 换成新的 socket, 并连接相关的信号
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
    // socket 连接状态标记为 true
    m_connected = true;
    // 创建一个新的 对 socket 的监听, 并连接相关信号
    delete m_notifier;
    m_notifier = new QSocketNotifier(m_socket->socketDescriptor(),
                                     QSocketNotifier::Exception,
                                     this);
    connect(m_notifier, &QSocketNotifier::activated,
            this, &QtTelnetPrivate::socketException);
    // 与远程主机进行连接协商
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

void QtTelnetPrivate::socketReadyRead()
{
    // 接收数据, 然后处理
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
