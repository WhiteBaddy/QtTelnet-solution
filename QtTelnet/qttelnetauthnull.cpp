#include "qttelnetauthnull.h"
#include "qttelnetrfc.h"


QtTelnetAuthNull::QtTelnetAuthNull()
    :QtTelnetAuth(char(0))
{
}

QByteArray QtTelnetAuthNull::authStep(const QByteArray &data)
{
    // 生成空认证模式的认证指令

    Q_ASSERT(data[0] == Common::Authentication);

    if (data.size() < 2 || data[1] != Common::SEND)
    {
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

    // 认证状态设置为 成功
    setState(AuthSuccess);

    return QByteArray(buffer, sizeof(buffer));
}
