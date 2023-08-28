#ifndef QTTELNETAUTH_H
#define QTTELNETAUTH_H

#include <QByteArray>

class QtTelnetAuth
{
public:
    /**
     * @brief 认证状态
     */
    enum State {
        AuthIntermidiate    // 认证中
        , AuthSuccess       // 认证成功
        , AuthFailure       // 认证失败
    };

    QtTelnetAuth(char code);
    virtual ~QtTelnetAuth();

    int getCode() const;
    State getState() const;
    void setState(State state);

    virtual QByteArray authStep(const QByteArray& data) = 0;

private:
    State m_state;
    int m_code;

};

#endif // QTTELNETAUTH_H
