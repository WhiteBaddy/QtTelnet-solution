#include "qttelnetauthnull.h"
#include "qttelnetrfc.h"

#include "QtTelnet_global.h"


QtTelnetAuthNull::QtTelnetAuthNull()
    :QtTelnetAuth(char(0))
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetAuthNull::QtTelnetAuthNull() 第 %1 次执行").arg(tmpCnt);
#endif
#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetAuthNull::QtTelnetAuthNull() 第 %1 次结束").arg(tmpCnt);
#endif
}

QByteArray QtTelnetAuthNull::authStep(const QByteArray &data)
{
    #if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetAuthNull::authStep(QByteArray) 第 %1 次执行").arg(tmpCnt);
#endif
    Q_ASSERT(data[0] == Common::Authentication);

    if (data.size() < 2 || data[1] != Common::SEND)
    {
        #if DUBUG_FUN_RUN
        qDebug() << QString("QtTelnetAuthNull::authStep(QByteArray) 第 %1 次结束, data.size() < 2 || data[1] != Common::SEND").arg(tmpCnt);
#endif
        return QByteArray();
    }

    char buffer[8] = {
        (char)Common::IAC
        , (char)Common::SB
        , (char)Common::Authentication
        , (char)Common::IS
        , (char)Auth::AUTHNULL
        , (char)0 // 客户|单向 CLIENT|ONE-WAY
        , (char)Common::IAC
        , (char)Common::SE
    };

    setState(AuthSuccess);

    QByteArray debugArray = QByteArray(buffer, sizeof(buffer));
#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetAuthNull::authStep(QByteArray) 第 %1 次结束").arg(tmpCnt);
#endif
//    return QByteArray(buffer, sizeof(buffer));
    return debugArray;
}
