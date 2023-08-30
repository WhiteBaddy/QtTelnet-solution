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
    int getSize() const;    // unused flag 这个函数好像没有使用过
    QByteArray readAll();   // 读取缓存区所有数据

private:
    QList<QByteArray> m_buffers;    // 数据缓存区
    int m_bytesAvailable;           // unused flag 这个值好像没有用过
};

#endif // QTTELNETRECEIVEBUFFER_H
