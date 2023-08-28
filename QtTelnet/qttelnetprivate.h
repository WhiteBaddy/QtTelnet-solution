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

    bool allowOption(int operation, int option);
    void sendOptions();
    void sendCommand(const QByteArray& command);
    void sendCommand(const char* command, int length);
    void sendCommand(const char operation, const char option);
    void sendString(const QString& string);
    bool replyNeeded(uchar operation, uchar option);
    void setMode(uchar operation, uchar option);
    bool alreadySent(uchar operation, uchar option);
    void addSent(uchar operation, uchar option);
    void sendWindowSize();

    int parsePlaintext(const QByteArray& data);
    int parseIAC(const QByteArray& data);
    bool isOperation(const uchar ch);
    bool isCommand(const uchar ch);
    QByteArray getSubOption(const QByteArray& data);
    void parseSubAuth(const QByteArray& data);
    void parseSubTT(const QByteArray& data);
    void parseSubNAWS(const QByteArray& data);
    uchar opposite(uchar operation, bool positive);

    void consume();

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
