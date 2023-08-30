#ifndef QTTELNETRFC_H
#define QTTELNETRFC_H

#include <QObject>

#ifdef QTTELNET_DEBUG
#include <QDebug>
#endif

namespace Common // RFC854
{
    // Commands
    const uchar CEOF  = 236;
    const uchar SUSP  = 237;
    const uchar ABORT = 238;
    const uchar SE    = 240; // End of subnegotiation parameters    // 子协商参数结束
    const uchar NOP   = 241; // No Operation                        // 无操作, 无任何实际功能, 用于维持会话连接, 作为心跳信号。
    const uchar DM    = 242; // Data Mark                           // 数据标记, 用于指示数据流的边界。同步的数据流部分。 这应始终伴随 TCP 紧急通知
    const uchar BRK   = 243; // Break, NVT character BRK            // 中断
    const uchar IP    = 244; // Interrupt Process                   // 中断进程
    const uchar AO    = 245; // Abort output                        // 终止输出
    const uchar AYT   = 246; // Are You There                       // 查询在线状态
    const uchar EC    = 247; // Erase character                     // 擦除字符
    const uchar EL    = 248; // Erase Line                          // 擦除线
    const uchar GA    = 249; // Go ahead                            // 继续执行
    const uchar SB    = 250; // Subnegotiation Begin                // 子选项开始, 用于开始 Telnet 子选项处理。
    const uchar WILL  = 251; // Will                                // 接受选项, 用于表示终端愿意启用或允许对方的某个选项。
    const uchar WONT  = 252; // Won’t                               // 拒绝选项, 用于表示终端拒绝或不支持对方的某个选项。
    const uchar DO    = 253; // Do                                  // 请求对方启用选项, 用于向对方请求启用或允许某个选项。
    const uchar DONT  = 254; // Don’t                               // 请求对方禁用选项, 用于向对方请求禁用或关闭某个选项。
    const uchar IAC   = 255; // Interpret As Command                // Telnet命令, 用于将下一个字节解释为Telnet命令。

    // Types
    const char IS    = 0;
    const char SEND  = 1;

    const char Authentication = 37; // RFC1416,
        // implemented to always return NULL
    const char SuppressGoAhead = 3; // RFC858
    const char Echo = 1; // RFC857, not implemented (returns WONT/DONT)
    const char LineMode = 34; // RFC1184, implemented
    const uchar LineModeEOF = 236, // RFC1184, not implemented
        LineModeSUSP = 237,
        LineModeABORT = 238;
    const char Status = 5; // RFC859, should be implemented!
    const char Logout = 18; // RFC727, implemented
    const char TerminalType = 24; // RFC1091,
        // implemented to always return UNKNOWN
    const char NAWS = 31; // RFC1073, implemented                           //RFC 1073, 协商窗口尺寸(Negotiate About Window Size)
    const char TerminalSpeed = 32; // RFC1079, not implemented
    const char FlowControl = 33; // RFC1372, should be implemented?
    const char XDisplayLocation = 35; // RFC1096, not implemented
    const char EnvironmentOld = 36; // RFC1408, should not be implemented!
    const char Environment = 39; // RFC1572, should be implemented
    const char Encrypt = 38; // RFC2946, not implemented

#ifdef QTTELNET_DEBUG
    QString typeStr(char op)
    {
        QString str;
        switch (op) {
        case IS:
            str = "IS";
            break;
        case SEND:
            str = "SEND";
            break;
        default:
            str = QString("Unknown common type (%1)").arg(op);
        }
        return str;
    }
    QString operationStr(char op)
    {
        QString str;
        switch (quint8(op)) {
        case quint8(WILL):
            str = "WILL";
            break;
        case quint8(WONT):
            str = "WONT";
            break;
        case quint8(DO):
            str = "DO";
            break;
        case quint8(DONT):
            str = "DONT";
            break;
        case quint8(SB):
            str = "SB";
            break;
        default:
            str = QString("Unknown operation (%1)").arg(quint8(op));
        }
        return str;
    }

    QString optionStr(char op)
    {
        QString str;
        switch (op) {
        case Authentication:
            str = "AUTHENTICATION";
            break;
        case SuppressGoAhead:
            str = "SUPPRESS GO AHEAD";
            break;
        case Echo:
            str = "ECHO";
            break;
        case LineMode:
            str = "LINEMODE";
            break;
        case Status:
            str = "STATUS";
            break;
        case Logout:
            str = "LOGOUT";
            break;
        case TerminalType:
            str = "TERMINAL-TYPE";
            break;
        case TerminalSpeed:
            str = "TERMINAL-SPEED";
            break;
        case NAWS:
            str = "NAWS";
            break;
        case FlowControl:
            str = "TOGGLE-FLOW-CONTROL";
            break;
        case XDisplayLocation:
            str = "X-DISPLAY-LOCATION";
            break;
        case EnvironmentOld:
            str = "ENVIRON";
            break;
        case Environment:
            str = "NEW-ENVIRON";
            break;
        case Encrypt:
            str = "ENCRYPT";
            break;
        default:
            str = QString("Unknown option (%1)").arg(op);
        }
        return str;
    }
#endif
};


namespace Auth // RFC1416
{
    enum Auth
    {
        REPLY = 2,
        NAME
    };
    enum Types
    {
        AUTHNULL, // Can't have enum values named NULL :/
        KERBEROS_V4,
        KERBEROS_V5,
        SPX,
        SRA = 6,
        LOKI = 10
    };
    enum Modifiers
    {
        AUTH_WHO_MASK = 1,
        AUTH_CLIENT_TO_SERVER = 0,
        AUTH_SERVER_TO_CLIENT = 1,
        AUTH_HOW_MASK = 2,
        AUTH_HOW_ONE_WAY = 0,
        AUTH_HOW_MUTUAL = 2
    };
    enum SRA
    {
        SRA_KEY = 0,
        SRA_USER = 1,
        SRA_CONTINUE = 2,
        SRA_PASSWORD = 3,
        SRA_ACCEPT = 4,
        SRA_REJECT = 5
    };

#ifdef QTTELNET_DEBUG
    QString authStr(int op)
    {
        QString str;
        switch (op) {
        case REPLY:
            str = "REPLY";
            break;
        case NAME:
            str = "NAME";
            break;
        default:
            str = QString("Unknown auth (%1)").arg(op);
        }
        return str;
    }
    QString typeStr(int op)
    {
        QString str;
        switch (op) {
        case AUTHNULL:
            str = "NULL";
            break;
        case KERBEROS_V4:
            str = "KERBEROS_V4";
            break;
        case KERBEROS_V5:
            str = "KERBEROS_V5";
            break;
        case SPX:
            str = "SPX";
            break;
        case SRA:
            str = "SRA";
            break;
        case LOKI:
            str = "LOKI";
            break;
        default:
            str = QString("Unknown auth type (%1)").arg(op);
        }
        return str;
    }
    QString whoStr(int op)
    {
        QString str;
        op = op & AUTH_WHO_MASK;
        switch (op) {
        case AUTH_CLIENT_TO_SERVER:
            str = "CLIENT";
            break;
        case AUTH_SERVER_TO_CLIENT:
            str = "SERVER";
            break;
        default:
            str = QString("Unknown who type (%1)").arg(op);
        }
        return str;
    }
    QString howStr(int op)
    {
        QString str;
        op = op & AUTH_HOW_MASK;
        switch (op) {
        case AUTH_HOW_ONE_WAY:
            str = "ONE-WAY";
            break;
        case AUTH_HOW_MUTUAL:
            str = "MUTUAL";
            break;
        default:
            str = QString("Unknown how type (%1)").arg(op);
        }
        return str;
    }
    QString sraStr(int op)
    {
        QString str;
        switch (op) {
        case SRA_KEY:
            str = "KEY";
            break;
        case SRA_REJECT:
            str = "REJECT";
            break;
        case SRA_ACCEPT:
            str = "ACCEPT";
            break;
        case SRA_USER:
            str = "USER";
            break;
        case SRA_CONTINUE:
            str = "CONTINUE";
            break;
        case SRA_PASSWORD:
            str = "PASSWORD";
            break;
        default:
            str = QString("Unknown SRA option (%1)").arg(op);
        }
        return str;
    }
#endif
};

namespace LineMode // RFC1184
{
    const char Mode = 1;
    const char ForwardMask = 2;
    const char SLC = 3;
    enum Modes
    {
        EDIT = 1,
        TRAPSIG = 2,
        MODE_ACK = 4,
        SOFT_TAB = 8,
        LIT_ECHO = 16
    };
    enum SLCs
    {
        SLC_SYNCH = 1,
        SLC_BRK = 2,
        SLC_IP = 3,
        SLC_AO =  4,
        SLC_AYT = 5,
        SLC_EOR = 6,
        SLC_ABORT = 7,
        SLC_EOF = 8,
        SLC_SUSP = 9,
        SLC_EC = 10,
        SLC_EL = 11,
        SLC_EW = 12,
        SLC_RP = 13,
        SLC_LNEXT = 14,
        SLC_XON = 15,
        SLC_XOFF = 16,
        SLC_FORW1 = 17,
        SLC_FORW2 = 18,
        SLC_MCL = 19,
        SLC_MCR = 20,
        SLC_MCWL = 21,
        SLC_MCWR = 22,
        SLC_MCBOL = 23,
        SLC_MCEOL = 24,
        SLC_INSRT = 25,
        SLC_OVER = 26,
        SLC_ECR = 27,
        SLC_EWR = 28,
        SLC_EBOL = 29,
        SLC_EEOL = 30,
        SLC_DEFAULT = 3,
        SLC_VALUE = 2,
        SLC_CANTCHANGE = 1,
        SLC_NOSUPPORT = 0,
        SLC_LEVELBITS = 3,
        SLC_ACK = 128,
        SLC_FLUSHIN = 64,
        SLC_FLUSHOUT = 32
    };
};

#endif // QTTELNETRFC_H
