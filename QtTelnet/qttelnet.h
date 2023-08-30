#ifndef QTTELNET_H
#define QTTELNET_H

#include "QtTelnet_global.h"

#include <QProcess>
#include <QQueue>
#include <QRegularExpression>
#include <QTcpSocket>

class QtTelnetPrivate;

class QTTELNET_EXPORT QtTelnet : public QObject
{
    Q_OBJECT
    friend class QtTelnetPrivate;
public:
    QtTelnet(QObject *parent = nullptr);
    ~QtTelnet();

    /**
     * @brief The Control enum  : 控制字符枚举
     */
    enum Control {
        GoAhead             // 继续进行操作或发送数据
        , InterruptProcess  // 中断当前进程或操作
        , AreYouThere       // 发送一个询问, 确认远程主机是否仍然可用或响应
        , AbortOutput       // 终止输出操作
        , EraseCharacter    // 擦除一个字符
        , EraseLine         // 擦除一行字符
        , Break             // 发送一个中断信号, 或打断当前操作
        , EndOfFile         // 文件结束, 或输入结束
        , Suspend           // 暂停当前操作或进程
        , Abort             // 终止当前操作或进程
    };

    /**
     * @brief connectToHost : 连接到主机
     * @param host          : 主机地址
     * @param port          : 端口号, telnet 的默认端口是 23
     */
    void connectToHost(const QString &host, quint16 port = 23);

    /**
     * @brief login : 登录
     * @param user  : 用户名
     * @param pass  : 密码
     */
    void login(const QString &user, const QString &pass);

    /**
     * @brief setWindowSize : 设置窗口大小, 通过调用同名(int, int)实现
     * @param size          : 窗口大小
     * @overload setWindowSize(int, int)
     */
    void setWindowSize(const QSize &size);
    /**
     * @brief setWindowSize : 设置窗口大小
     * @param width         : 窗口宽度
     * @param height        : 窗口高度
     */
    void setWindowSize(int width, int height); // 字符数 / In number of characters
    /**
     * @brief windowSize    : 获取当前窗口的大小
     * @return              : 表示窗口大小的 QSize 值
     */
    QSize windowSize() const;
    /**
     * @brief isValidWindowSize : 窗口大小是否有效
     * @return                  : 窗口大小是否有效的 bool 值
     */
    bool isValidWindowSize() const;

    /**
     * @brief setSocket : 赋一个新的 socket 值
     * @param socket    : 一个新的 socket 值
     */
    void setSocket(QTcpSocket *socket);
    /**
     * @brief socket    : 获取当前的 socket 值
     * @return          : 当前的 socket 值
     */
    QTcpSocket *socket() const;

    /**
     * @brief setPromptParrern  : 设置提示模式
     * @param pattern           : 模式
     */
    void setPromptParrern(const QRegularExpression &pattern);
    /**
     * @brief setPromptParrern  : 设置提示模式, 通过调用同名(QRegularExpression)实现
     * @param pattern           : 模式
     * @overload setPromptPattern(QRegularExpression)
     */
    void setPromptParrern(const QString &pattern);
    /**
     * @brief setLoginPattern   : 设置登录提示模式
     * @param pattern           : 模式
     */
    void setLoginPattern(const QRegularExpression &pattern);
    /**
     * @brief setLoginPattern   : 设置登录提示模式, 通过调用同名(QRegularExpression)实现
     * @param pattern           : 模式
     * @overload setLoginPattern(QRegularExpression)
     */
    void setLoginPattern(const QString &pattern);
    /**
     * @brief setPasswordPattern    : 设置登录提示模式
     * @param pattern               : 模式
     */
    void setPasswordPattern(const QRegularExpression &pattern);
    /**
     * @brief setPasswordPattern    : 设置登录提示模式, 通过调用同名(QRegularExpression)实现
     * @param pattern               : 模式
     * @overload setPasswordPattern(QRegularExpression)
     */
    void setPasswordPattern(const QString &pattern);

public slots:
    /**
     * @brief close : 关闭套接字
     */
    void close();
    /**
     * @brief logout    : 发送登出指令
     */
    void logout();
    /**
     * @brief sendControl   : 根据不同的参数发送不同的指令
     * @param ctrl          : 控制字符
     */
    void sendControl(Control ctrl);
    /**
     * @brief sendData  : 发送给定的数据
     * @param data      : 给定的数据
     */
    void sendData(const QString &data);
    /**
     * @brief sendSync  : 发送 sync 指令
     */
    void sendSync();

signals:
    /**
     * @brief loginRequired : 需要输入登录信息
     */
    void loginRequired();
    /**
     * @brief loginFailed   : 登陆失败
     */
    void loginFailed();
    /**
     * @brief loggedIn      :登陆成功
     */
    void loggedIn();
    /**
     * @brief loggedOut     : 登出成功
     */
    void loggedOut();
    /**
     * @brief connectionError   : 连接出错
     * @param error             : 错误代码
     */
    void connectionError(QAbstractSocket::SocketError error);
    /**
     * @brief message   : telnet 接收到的信息
     * @param data      : 信息
     */
    void message(const QString &data);


private:
    QtTelnetPrivate *m_d;

};

#endif // QTTELNET_H
