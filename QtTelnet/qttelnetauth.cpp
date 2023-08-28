#include "qttelnetauth.h"

#include "QtTelnet_global.h"

QtTelnetAuth::QtTelnetAuth(char code)
    :m_state(AuthIntermidiate)
    , m_code(code)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetAuth::QtTelnetAuth(char) 第 %1 次执行").arg(tmpCnt);
    #endif

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetAuth::QtTelnetAuth(char) 第 %1 次结束").arg(tmpCnt);
    #endif

}

QtTelnetAuth::~QtTelnetAuth()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetAuth::~QtTelnetAuth() 第 %1 次执行").arg(tmpCnt);
    #endif

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetAuth::~QtTelnetAuth() 第 %1 次结束").arg(tmpCnt);
    #endif

}

int QtTelnetAuth::getCode() const
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetAuth::getCode() 第 %1 次执行").arg(tmpCnt);
    #endif

    int debugCode = m_code;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetAuth::getCode() 第 %1 次结束").arg(tmpCnt);
    #endif

//    return m_code;
    return debugCode;
}

QtTelnetAuth::State QtTelnetAuth::getState() const
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetAuth::getCode() 第 %1 次执行").arg(tmpCnt);
    #endif

    QtTelnetAuth::State debugState = m_state;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetAuth::getCode() 第 %1 次结束").arg(tmpCnt);
    #endif

//    return m_state;
    return debugState;
}

void QtTelnetAuth::setState(State state)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetAuth::setState(State) 第 %1 次执行").arg(tmpCnt);
    #endif

    m_state = state;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetAuth::setState(State) 第 %1 次结束").arg(tmpCnt);
    #endif
}
