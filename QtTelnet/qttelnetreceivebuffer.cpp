#include "qttelnetreceivebuffer.h"

QtTelnetReceiveBuffer::QtTelnetReceiveBuffer()
    : m_bytesAvailable(0)
{
}

void QtTelnetReceiveBuffer::append(const QByteArray &data)
{
    m_buffers.append(data);
}

void QtTelnetReceiveBuffer::push_back(const QByteArray &data)
{
    m_buffers.prepend(data);
}

int QtTelnetReceiveBuffer::getSize() const
{
    return m_bytesAvailable;
}

QByteArray QtTelnetReceiveBuffer::readAll()
{
    QByteArray array;
    while (!m_buffers.isEmpty())
    {
        array.append(m_buffers.takeFirst());
    }
    return array;
}
