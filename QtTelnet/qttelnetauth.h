#ifndef QTTELNETAUTH_H
#define QTTELNETAUTH_H

#include <QByteArray>

class QtTelnetAuth
{
public:

    /* State : 认证状态 */
    enum State {
        AuthIntermidiate    // 认证中
        , AuthSuccess       // 认证成功
        , AuthFailure       // 认证失败
    };

    QtTelnetAuth(char code);
    virtual ~QtTelnetAuth();


    int getCode() const;        // unused flag 这个函数好像没有使用过
    State getState() const;
    void setState(State state);

    /*!
     * \brief authStep  : 根据需要, 生成登陆验证指令
     * \param data      : 接收到的数据, 据此判断是否生成
     * \return          : 如果条件符合, 生成对应指令, 否则返回空值
     * \pure 这是一个纯虚函数, 只有在 \class QtTelnetAuthNull 子类中该函数被实现
     */
    virtual QByteArray authStep(const QByteArray& data) = 0;

private:
    State m_state;      // 状态值
    int m_code;         // unused flag 这个值好像没有使用过

};

#endif // QTTELNETAUTH_H
