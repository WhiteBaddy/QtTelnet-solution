#ifndef QTTELNETRECEIVEBUFFER_H
#define QTTELNETRECEIVEBUFFER_H

#include <QByteArray>
#include <QList>



class QtTelnetReceiveBuffer
{
public:
    QtTelnetReceiveBuffer();
    void append(const QByteArray& data);
    void push_back(const QByteArray& data);
    int getSize() const;
    QByteArray readAll();

private:
    QList<QByteArray> m_buffers;
    int m_bytesAvailable;
};

#endif // QTTELNETRECEIVEBUFFER_H
