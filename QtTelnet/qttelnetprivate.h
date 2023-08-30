#ifndef QTTELNETPRIVATE_H
#define QTTELNETPRIVATE_H

#include "qttelnetreceivebuffer.h"
#include "qttelnetauth.h"

#include <QList>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QRegularExpression>
#include <QSize>
#include <QSocketNotifier>
#include <QTcpSocket>

class QtTelnet;

class QtTelnetPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QtTelnetPrivate(QtTelnet *parent);
    ~QtTelnetPrivate();

    /*!
     * \brief allowOption   : 是否允许选项
     * \param operation     : 操作类型
     * \param option        : 选项
     * \return              : 表示是否允许选项的 bool 值
     */
    bool allowOption(int operation, int option);
    /*!
     * \brief sendOptions   : 执行一个发送选项的操作
     */
    void sendOptions();
    /*!
     * \brief sendCommand   : 发送一串指令, 通过 socket 的 write 函数发送
     * \param command       : 要发送的指令
     */
    void sendCommand(const QByteArray& command);
    /*!
     * \brief sendCommand   : 发送一串字符数组格式的指令, 通过调用同名(QByteArray)实现
     * \param command       : 指令的起始地址
     * \param length        : 指令长度
     * \see sendCommand(const QByteArray&)
     */
    void sendCommand(const char* command, int length);
    /*!
     * \brief sendCommand   : 发送一组选项指令, 通过同名(char*, int) 实现
     * \param operation     : 操作
     * \param option        : 选项
     * \see sendCommand(const char*, int)
     */
    void sendCommand(const char operation, const char option);
    /*!
     * \brief sendString    : 发送一条字符串, 通过 socket 的 write 函数实现
     * \param string        : 要发送的字符串
     */
    // 要不要和 sendCommand 合并呢
    void sendString(const QString& string);
    /*!
     * \brief replyNeeded   : 判断对应操作是否需要回复
     * \param operation     : 操作类型
     * \param option        : 选项
     * \return              : 一个表示是否需要回复的 bool 值
     */
    bool replyNeeded(uchar operation, uchar option);
    /*!
     * \brief setMode       : 设置 telnet 连接的模式
     * \param operation     : 操作类型
     * \param option        : 选项
     */
    void setMode(uchar operation, uchar option);
    /*!
     * \brief alreadySent   : 判断该指令是否已经发送过, 如果发送过, 把 m_osent 列表里相同的指令全部清除
     * \param operation     : 操作类型
     * \param option        : 选项
     * \return              : 一个表示是否发送过的 bool 值
     */
    bool alreadySent(uchar operation, uchar option);
    /*!
     * \brief addSent       : 添加一个操作指令到 m_osent 列表
     * \param operation     : 操作类型
     * \param option        : 选项
     */
    void addSent(uchar operation, uchar option);
    /*!
     *  \brief 发送窗口大小
     */
    void sendWindowSize();

    /*!
     * \brief parsePlaintext    : 处理纯文本数据
     * \param data              : 等待处理的数据
     * \return                  : 处理的数据长度
     */
    int parsePlaintext(const QByteArray& data);
    /*!
     * \brief parseIAC          : 处理 telnet 指令
     * \param data              : 等待处理的数据
     * \return                  : 处理的数据长度
     */
    int parseIAC(const QByteArray& data);
    /*!
     * \brief isOperation       : 判断字符是否是操作类型 (WILL, WONT, DO, DONT)
     * \param ch                : 需要被判断的字符
     * \return                  : 一个表示是否是操作字符的 bool 值
     */
    bool isOperation(const uchar ch);
    /*!
     * \brief isCommand         : 判断字符是否是指令 (DM)
     * \param ch                : 需要判断的字符
     * \return                  : 一个表示是否是指令字符的 bool 值
     */
    bool isCommand(const uchar ch);
    /*!
     * \brief getSubOption      : 用于在 \fn parseIAC 中 获取操作的子选项
     * \param data              : 缓存区数据
     * \return                  : 返回获得的子选项
     */
    QByteArray getSubOption(const QByteArray& data);
    /*!
     * \brief parseSubAuth      : 用于在 \fn parseIAC 中 解析授权
     * \param data              : 缓存区数据
     */
    void parseSubAuth(const QByteArray& data);
    /*!
     * \brief parseSubTT        : 用于在 \fn parseIAC 中 解析终端类型
     * \param data              : 缓存区数据
     */
    void parseSubTT(const QByteArray& data);
    /*!
     * \brief parseSubNAWS      : 用于在 \fn parseIAC 中 协商窗口大小
     * \param data              : 缓存区数据
     */
    void parseSubNAWS(const QByteArray& data);
    uchar opposite(uchar operation, bool positive);

    /**
    * @brief consume            : 处理缓存区的数据
    */
    void consume();

    /*!
     * \brief setSocket         : 赋值一个新的 socket 套接字
     * \param socket            : 新的套接字
     */
    void setSocket(QTcpSocket* socket);

public slots:
    void socketConnected();
    void socketConnectionClosed();
    void socketReadyRead();
    void socketError(QAbstractSocket::SocketError error);
    void socketException(int);

public:

    QMap<char, bool> m_modes;
    QList<QPair<uchar, uchar>> m_osent;

    QtTelnet *m_q;
    QTcpSocket* m_socket;
    QtTelnetReceiveBuffer m_buffer;
    QSocketNotifier* m_notifier;

    QSize m_windowSize;

    bool m_connected;
    bool m_nocheckp;
    bool m_triedLogin;
    bool m_triedPass;
    bool m_firstTry;

    QMap<int, QtTelnetAuth*> m_auths;
    QtTelnetAuth* m_curAuth;
    bool m_nullAuth;

    QRegularExpression m_loginp;
    QRegularExpression m_passp;
    QRegularExpression m_promptp;

    QString m_login;
    QString m_pass;

signals:

};

#endif // QTTELNETPRIVATE_H
