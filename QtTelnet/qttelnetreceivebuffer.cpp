#include "qttelnetreceivebuffer.h"

#include "QtTelnet_global.h"

QtTelnetReceiveBuffer::QtTelnetReceiveBuffer()
    : m_bytesAvailable(0)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetReceiveBuffer::QtTelnetReceiveBuffer() 第 %1 次执行").arg(tmpCnt);
    #endif

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetReceiveBuffer::QtTelnetReceiveBuffer() 第 %1 次结束").arg(tmpCnt);
    #endif

}

void QtTelnetReceiveBuffer::append(const QByteArray &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetReceiveBuffer::append(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

    m_buffers.append(data);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetReceiveBuffer::append(QByteArray) 第 %1 次结束").arg(tmpCnt);
    #endif
}

void QtTelnetReceiveBuffer::push_back(const QByteArray &data)
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetReceiveBuffer::push_back(QByteArray) 第 %1 次执行").arg(tmpCnt);
    #endif

    m_buffers.prepend(data);

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetReceiveBuffer::push_back(QByteArray) 第 %1 次结束").arg(tmpCnt);
    #endif
}

int QtTelnetReceiveBuffer::getSize() const
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetReceiveBuffer::getSize() 第 %1 次执行").arg(tmpCnt);
    #endif

    int debugBytesAvailable = m_bytesAvailable;

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetReceiveBuffer::getSize() 第 %1 次结束").arg(tmpCnt);
    #endif

//    return m_bytesAvailable;
    return debugBytesAvailable;
}

QByteArray QtTelnetReceiveBuffer::readAll()
{
#if DUBUG_FUN_RUN
    static int cnt = 0;
    int tmpCnt = ++cnt;
    qDebug() << QString("QtTelnetReceiveBuffer::readAll() 第 %1 次执行").arg(tmpCnt);
    #endif

    QByteArray array;
    while (!m_buffers.isEmpty())
    {
        array.append(m_buffers.takeFirst());
    }

#if DUBUG_FUN_RUN
    qDebug() << QString("QtTelnetReceiveBuffer::readAll() 第 %1 次结束").arg(tmpCnt);
    #endif

    return array;
}
